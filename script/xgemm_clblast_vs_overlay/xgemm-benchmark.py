#!/bin/python
import ck.kernel as ck
import copy
import re

# Layout parameters and number of repetitions.
p={'start':101, 'step':1, 'stop':102, 'default':101, 'repeat':4}

def do(i):
    # Detect basic platform info.
    ii={'action':'detect', 'module_uoa':'platform', 'out':'con'}
    r=ck.access(ii)
    if r['return']>0: return r

    # Host and target OS params.
    hos=r['host_os_uoa']
    hosd=r['host_os_dict']

    tos=r['os_uoa']
    tosd=r['os_dict']
    tdid=r['device_id']
    # FIXME: detect OpenCL device here.

    # Search for xgemm programs.
    ii={'action':'search', 'module_uoa':'program', 'tags':'gemmbench,xgemm'}
    r=ck.access(ii)
    if r['return']>0: return r
    programs=r['lst']
    if not programs:
        return {'return':1, 'error':'no xgemm programs found!'}

    # Get deps from one of the xgemm programs.
    program=programs[0]
    ii={'action':'load', 'module_uoa':'program', 'data_uoa':program['data_uoa']}
    r=ck.access(ii)
    if r['return']>0: return r

    # Update deps from GPGPU or ones remembered during autotuning.
    # FIXME: What does the above comment mean? Autotuning hasn't started yet...
    cdeps=r['dict'].get('compile_deps',{})

    # Show deps.
    ck.out('---------------------------------------------------------------------------------------')
    ck.out('cdeps: ' + str(cdeps))

    # Resolve deps.
    ii={'action':'resolve', 'module_uoa':'env',
        'host_os':hos, 'target_os':tos, 'device_id':tdid,
        'deps':cdeps, 'out':'con'}
    r=ck.access(ii)
    if r['return']>0: return r

    # Prepare pipeline.
    ii={'action':'pipeline',
        'prepare':'yes',

        'module_uoa':'program',
        'data_uoa':program['data_uoa'],
        'dependencies':cdeps,

        'cmd_key':'explore-layouts',

        'cpu_freq':'max',
        'gpu_freq':'max',

        'speed':'yes',
        'energy':'no',

        'no_state_check':'yes',
        'no_compiler_description':'yes',
        'compile_only_once':'yes',
        'skip_calibration':'yes',
        'skip_print_timers':'yes',
        'out':'con'
    }
    r=ck.access(ii)
    if r['return']>0: return r

    fail=r.get('fail','')
    if fail=='yes':
        return {'return':10, 'error':'pipeline failed ('+r.get('fail_reason','')+')'}

    ready=r.get('ready','')
    if ready!='yes':
        return {'return':11, 'error':'pipeline not ready'}

    state=r['state']
    tmp_dir=state['tmp_dir']

    # Remember resolved deps for this benchmarking session.
    # FIXME: Unused anywhere. Can be safely removed?
    xcdeps=r.get('dependencies',{})

    # Clean pipeline.
    if 'ready' in r: del(r['ready'])
    if 'fail' in r: del(r['fail'])
    if 'return' in r: del(r['return'])

    # FIXME: Is deep copy here needed? Also deep copied below for each program.
    pipeline=copy.deepcopy(r)

    # For each xgemm program.
    for program in programs:
        # Load xgemm program.
        ii={'action':'load', 'module_uoa':'program', 'data_uoa':program['data_uoa']}
        r=ck.access(ii)
        if r['return']>0: return r

        # Show program tags.
        tags=r['dict']['tags']
        ck.out('---------------------------------------------------------------------------------------')
        ck.out(r['data_name']+': '+str(tags))

        # Configure repository.
        record_repo='local'
        record_uoa='gemmbench-xgemm-clblast'
        if 'overlay' in tags:
            record_uoa+='-overlay'

        # Configure choices.
        choices_selection=[]
        choices_order=[]
        choices_selection.append({'type':'loop', 'start':p['start'], 'stop':p['stop'], 'step':p['step'], 'default':p['default']})
        choices_order.append(["##env#CLBLAST_LAYOUT"])
        # TODO: Make Lift overlay iterate over multiple generated kernels.
#        if 'overlay' in tags:
#            choices_selection.append({"type":"loop"})
#            choices_order.append("##dataset_file")

        # Prepare pipeline.
        cpipeline=copy.deepcopy(pipeline)
        cpipeline.update({'data_uoa':program['data_uoa']})

        # Prepare autotuning input.
        ii={'action':'autotune',

            'module_uoa':'pipeline',
            'data_uoa':'program',

            'choices_selection':choices_selection,
            'choices_order':choices_order,

            'features_keys_to_process':['##choices#*'],

            'iterations':-1,
            'repetitions':p['repeat'],

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
              'compiler_vars': {}
            },
            'out':'con'
        }

        r=ck.access(ii)
        if r['return']>0: return r

        fail=r.get('fail','')
        if fail=='yes':
            return {'return':10, 'error':'pipeline failed ('+r.get('fail_reason','')+')'}

    return {'return':0}

r=do({})
if r['return']>0: ck.err(r)
