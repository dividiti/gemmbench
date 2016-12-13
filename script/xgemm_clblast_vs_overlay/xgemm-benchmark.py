#!/bin/python
import ck.kernel as ck
import copy
import re

class CKError(RuntimeError):
   def __init__(self, arg):
      self.args = arg

def ck_do(ii):
  r=ck.access(ii)
  if r['return']>0: 
     ck.err(r['error'])
#raise CKError(r)
  return r

# Layout parameters and number of repetitions.
# layoutParams={'start':101, 'step':1, 'stop':102, 'default':101, 'repeat':4}
repetitions = 4

def do(i):
    r=ck_do({'action':'detect', 'module_uoa':'platform', 'out':'con'})

    # Host and target OS params.
    hos=r['host_os_uoa']
    hosd=r['host_os_dict']

    tos=r['os_uoa']
    tosd=r['os_dict']
    tdid=r['device_id']

    # Detect OpenCL device here.
    rg=ck_do({'action':'detect',
              'module_uoa':'platform.gpgpu',
              'out':'con',
              'host_os':hos,
              'target_os':tos,
              'target_device_id':tdid,
              'type':'opencl',
              'select':'yes'})

    cp_id=rg.get('choices',{}).get('compute_platform_id','')
    cd_id=rg.get('choices',{}).get('compute_device_id','')

    # Search for xgemm programs.
    r=ck_do({'action':'search', 'module_uoa':'program', 'tags':'gemmbench,xgemm'})
    programs=r['lst']
    if not programs:
        raise CKError({'return':1, 'error':'no xgemm programs found!'})

    ck.out('---------------------------------------------------------------------------------------')
    ck.out('Found {} programs:'.format(len(programs)))
    for p in programs:
      ck.out('  {}'.format(p['data_uoa']))

    cdeps = {}
    for p in programs:
      r=ck_do({'action':'load', 'module_uoa':'program', 'data_uoa':p['data_uoa']})
      cdeps[p['data_uoa']] = r['dict'].get('compile_deps',{})

      # Show deps.
      ck.out('---------------------------------------------------------------------------------------')
      ck.out('cdeps for {}: {}'.format(p['data_uoa'], str(cdeps[p['data_uoa']])))

      # Resolve deps.
      ck_do({'action':'resolve', 'module_uoa':'env',
             'host_os':hos, 'target_os':tos, 'device_id':tdid,
             'deps':cdeps[p['data_uoa']], 'out':'con'})

    # Prepare pipeline.
    r=ck_do({
        'action':'pipeline',
        'prepare':'yes',

        'module_uoa':'program',
        'data_uoa':programs[0]['data_uoa'],
        'dependencies':cdeps[programs[0]['data_uoa']],

        'cmd_key':'explore-lift-kernels',

#        'dvdt_prof':'yes',

        'cpu_freq':'max',
        'gpu_freq':'max',

        'speed':'yes',
        'energy':'no',

        'compute_platform_id':cp_id,
        'compute_device_id':cd_id,

        'no_state_check':'yes',
        'no_compiler_description':'yes',
        'compile_only_once':'yes',
        'skip_calibration':'yes',
        'skip_print_timers':'yes',
        'out':'con'
    })

    fail=r.get('fail','')
    if fail=='yes':
        raise CKError({'return':10, 'error':'pipeline failed ('+r.get('fail_reason','')+')'})

    ready=r.get('ready','')
    if ready!='yes':
        raise CKError({'return':11, 'error':'pipeline not ready'})

    state=r['state']
    tmp_dir=state['tmp_dir']

    # Clean pipeline.
    if 'ready' in r: del(r['ready'])
    if 'fail' in r: del(r['fail'])
    if 'return' in r: del(r['return'])

    # FIXME: Is deep copy here needed? Also deep copied below for each program.
    # FGG: I prefer to keep it - when we reuse output dict, there are
    # often references and some parts of dict can later disappear and
    # it's very difficult to debug. Hence I have a clean and full copy here (reference pipeline)
    # and then I created a new clean and full copy of this reference copy at each iteration ...
    pipeline=copy.deepcopy(r)

    # For each xgemm program.
    for program in programs:
        # Load xgemm program.
        r=ck_do({'action':'load', 'module_uoa':'program', 'data_uoa':program['data_uoa']})

        # Show program tags.
        tags=r['dict']['tags']
        ck.out('---------------------------------------------------------------------------------------')
        ck.out(r['data_name']+': '+str(tags))

        # Configure repository.
        ds=[None]
        record_repo='local'
        record_uoa='gemmbench-xgemm-clblast'
        cmd_key='default'
        if 'overlay' in tags:
            ds=[]
            record_uoa+='-overlay'
            cmd_key='explore-lift-kernels'

            # Find related data set
            r=ck_do({'action':'search', 'module_uoa':'dataset', 'tags':'dataset,lift,gemmbench,opencl kernel', 'add_meta':'yes'})

            lst=r['lst']

            for q in lst:
                duid=q['data_uid']
                meta=q['meta']
                for z in meta.get('dataset_files',[]):
                    ds.append({'dataset_uid':duid, 'dataset_file':z})

        # Loop over datasets
        for d in ds:
            ds_uoa=''
            ds_file=''

            if d!=None:
               ds_uoa=d['dataset_uid']
               ds_file=d['dataset_file']

            # Configure choices.
            choices_selection=[]
            choices_order=[]
            #choices_selection.append({'type':'loop', 'start':layoutParams['start'], 'stop':layoutParams['stop'], 'step':layoutParams['step'], 'default':layoutParams['default']})
            #choices_order.append(["##env#CLBLAST_LAYOUT"])
            # TODO: Make Lift overlay iterate over multiple generated kernels.
#            if 'overlay' in tags:
#                choices_selection.append({"type":"loop"})
#                choices_order.append(["##dataset_file"])

            # Prepare pipeline.
            cpipeline=copy.deepcopy(pipeline)
            cpipeline.update({'data_uoa':program['data_uoa']})

            # Prepare autotuning input.
            r=ck.access({
                'action':'autotune',

                'module_uoa':'pipeline',
                'data_uoa':'program',

                'choices_selection':choices_selection,
                'choices_order':choices_order,

                'features_keys_to_process':['##choices#*'],

                'iterations':1,
                'repetitions':repetitions,

                'record':'yes',
                'record_repo':record_repo,
                'record_uoa':record_uoa,
                'record_failed':'yes',
                'record_params':{
                    'search_point_by_features':'yes'
                },
                'record_dict':{
                    'subview_uoa':'1048849e824f668c'
                },

                'tags':tags+['layouts'],

                'pipeline':cpipeline,
                'pipeline_update':{
                  'cpu_freq':'max',
                  'gpu_freq':'max',
                  'compiler_vars': {},
                  'dataset_uoa':ds_uoa,
                  'dataset_file':ds_file,
                  'cmd_key':cmd_key
                },
                'out':'con',
                'ask_enter_after_each_iteration':'yes'
            })
            if r['return']>0: ck.err(r)

            fail=r.get('fail','')
            if fail=='yes':
                raise CKError({'return':10, 'error':'pipeline failed ('+r.get('fail_reason','')+')'})

    return {'return':0}

try:
  r=do({})
  if r['return']>0: ck.err(r)
except CKError as err:
  ck.err(err.args)
