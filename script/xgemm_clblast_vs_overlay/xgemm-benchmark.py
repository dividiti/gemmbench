#!/bin/python
import ck.kernel as ck
import copy
import re

# Layout parameters and number of repetitions.
p={'start':101, 'step':1, 'stop':102, 'default':101, 'repeat':4}

def do(i):
    # Detect basic platform info.
    ii={'action':'detect',
        'module_uoa':'platform',
        'out':'out'}
    r=ck.access(ii)
    if r['return']>0: return r

    # Host and target OS params.
    hos=r['host_os_uoa']
    hosd=r['host_os_dict']

    tos=r['os_uoa']
    tosd=r['os_dict']
    tdid=r['device_id']

    # Search for xgemm programs.
    ii={'action':'search', 'module_uoa':'program', 'tags':'gemmbench,xgemm'}
    r=ck.access(ii)
    if r['return']>0: return r
    programs=r['lst']
    if not programs:
        return {'return':1, 'error':'no xgemm programs found!'}

    # Get deps from one of the xgemm programs.
    program=programs[1]
    ii={'action':'load', 'module_uoa':'program', 'data_uoa':program['data_uoa']}
    r=ck.access(ii)
    if r['return']>0: return r
                                        
    # Update deps from GPGPU or ones remembered during autotuning.
    cdeps=r['dict'].get('compile_deps',{})
    ck.out('---------------------------------------------------------------------------------------')
    ck.out('cdeps: ' + str(cdeps))

    # Resolve CLBlast dep.
    cdeps_clblast=copy.deepcopy(cdeps['lib-clblast'])
    ii={'action':'resolve', 'module_uoa':'env',
        'host_os':hos, 'target_os':tos, 'device_id':tdid,
        'deps':{'lib-clblast':cdeps_clblast}}
    r=ck.access(ii)
    if r['return']>0: return r
    # All UOAs of CLBlast envs.
    envs_clblast=r['deps']['lib-clblast'].get('choices',[])
    if not envs_clblast:
        return {'return':1, 'error':'no CLBlast envs found!'}
    ck.out('clblast: ' + str(envs_clblast))

    # Resolve OpenCL dep.
    cdeps_opencl=copy.deepcopy(cdeps['lib-opencl'])
    ii={'action':'resolve', 'module_uoa':'env',
        'host_os':hos, 'target_os':tos, 'device_id':tdid,
        'deps':{'lib-opencl':cdeps_opencl}}
    r=ck.access(ii)
    if r['return']>0: return r
    # All UOAs of OpenCL envs.
    envs_opencl=r['deps']['lib-opencl'].get('choices',[])
    if not envs_opencl:
        return {'return':1, 'error':'no OpenCL envs found!'}
    ck.out('opencl: ' + str(envs_opencl))

    # Resolve compiler dep.
    cdeps_compiler=copy.deepcopy(cdeps['compiler'])
    ii={'action':'resolve', 'module_uoa':'env',
        'host_os':hos, 'target_os':tos, 'device_id':tdid,
        'deps':{'compiler':cdeps_compiler}}
    r=ck.access(ii)
    if r['return']>0: return r
    # All UOAs of compiler envs.
    envs_compiler=r['deps']['compiler'].get('choices',[])
    if not envs_compiler:
        return {'return':1, 'error':'no compiler envs found!'}
    ck.out('compiler: ' + str(envs_compiler))
    
    # Prepare pipeline.
    cdeps['lib-clblast']['uoa']=envs_clblast[0]
    cdeps['lib-opencl']['uoa']=envs_opencl[0]
    cdeps['compiler']['uoa']=envs_compiler[0]

    ii={'action':'pipeline',
        'prepare':'yes',
       
        'module_uoa':'program',
        'data_uoa':program['data_uoa'],
        'dependencies': cdeps,

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
    xcdeps=r.get('dependencies',{})

    # Clean pipeline.
    if 'ready' in r: del(r['ready'])
    if 'fail' in r: del(r['fail'])
    if 'return' in r: del(r['return'])

    # FIXME: Is deep copy needed? also deep copied below.
    pipeline=copy.deepcopy(r)

    # For each xgemm program.
    for program in programs:
        # Load xgemm program.
        ii={'action':'load', 'module_uoa':'program', 'data_uoa':program['data_uoa']}
        r=ck.access(ii)
        if r['return']>0: return r
        tags=r['dict']['tags']
        ck.out('---------------------------------------------------------------------------------------')
        ck.out(r['data_name']+': '+str(tags))
        
        record_repo='local'
        record_uoa='gemmbench-xgemm-clblast'
        if 'overlay' in tags:
            record_uoa+='-overlay'

        # Prepare pipeline.
        cpipeline=copy.deepcopy(pipeline)

        # Prepare autotuning input.
        ii={'action':'autotune',
 
            'module_uoa':'pipeline',
            'data_uoa':'program',
 
            'choices_order':[
                [
                    "##env#CLBLAST_LAYOUT"
                ]
            ],
            'choices_selection':[
                {'type':'loop', 'start':p['start'], 'stop':p['stop'], 'step':p['step'], 'default':p['default']}
            ],
 
            'features_keys_to_process':['##choices#*'],
 
            'iterations':-1,
            'repetitions':p['repeat'],
 
            'record':'yes',
            'record_failed':'yes',
            'record_params':{
                'search_point_by_features':'yes'
            },
            'record_repo':record_repo,
            'record_uoa':record_uoa,
 
            'tags':tags+['layouts'],
 
            'pipeline':cpipeline,
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
