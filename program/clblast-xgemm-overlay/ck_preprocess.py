#
# Script to Preprocess gemmbench templates
# Custom autotuner script (make choices)
#
# Developer: Michel Steuwer, University of Edinburgh, 2016
#            Grigori Fursin, cTuning foundation, 2016
#

import os

# This function is called for each autotuning iteration
# and need to generate choices which will stay the same
# during statistical repetitions 

def make(i):

    keys={} # what to change in the pipeline
    ck=i['ck_kernel']
    pipeline=i['pipeline']

    ai=i['autotuning_iteration']

    # Init global random, unless prepared by CK (with seed)
    from random import Random
    rm=i.get('random_module',None)
    if rm==None: rm=Random()

    deps=pipeline['dependencies']
    ch=pipeline['choices']
    env=ch.get('env',{})

    # Get dataset path
    dduoa=ch['dataset_uoa']
    r=ck.access({'action':'find',
                 'module_uoa':'dataset',
                 'data_uoa':dduoa})
    if r['return']>0: return r
    dp=r['path']

    df=ch['dataset_file']

    # Some default values
    if env.get('CK_XGEMM_DATASET_N','')=='': 
       env['CK_XGEMM_DATASET_N']=512
    if env.get('CK_XGEMM_DATASET_M','')=='': 
       env['CK_XGEMM_DATASET_M']=512
    if env.get('CK_XGEMM_DATASET_K','')=='': 
       env['CK_XGEMM_DATASET_K']=512

    keys['##choices#env#CK_XGEMM_AUTOTUNER']='yes'

    if ai==0:
       new_choices='no'
    else:
       new_choices='yes'

    r=ck_preprocess({'new_choices':new_choices,
                     'ck_kernel':ck,
                     'env':env,
                     'dataset_path':dp,
                     'dataset_file':df,
                     'random_module':rm})
    if r['return']>0: return r

    # Convert env to flat keys to change in pipeline
    for k in r['new_env']:
        v=r['new_env'][k]
        keys['##choices#env#'+k]=v

    return {'return':0, 'keys':keys, 'finish':False}

###########################################################################
# This function is called before each execution of a program/kernel
# I.e. it will be executed for each of the statistical repetition

def ck_preprocess(i):

    env=i['env']

    snc=i.get('new_choices','')

    if env.get('CK_XGEMM_AUTOTUNER','')=='yes' and snc=='':
       return {'return':0}

    nc=False
    if snc=='yes': nc=True

    # Get random module if needed (it is already with prepared seed if needed)
    rm=i.get('random_module',None)

    global n,m,k # needed for evaluation

    ck=i['ck_kernel']

    nenv={}

    # Dataset features
    n = int(env['CK_XGEMM_DATASET_N'])
    m = int(env['CK_XGEMM_DATASET_M'])
    k = int(env['CK_XGEMM_DATASET_K'])

    # Get dataset path and file
    dp=i['dataset_path']
    df=os.path.join(dp,i['dataset_file'])

    # Load json file
    r=ck.load_json_file({'json_file':df})
    if r['return']>0: return r

    metadata=r['dict']

    # Kernel filename
    kernelFile = os.path.splitext(df)[0]+'.cl'
    kernelName = metadata["kernel_name"]

    globalX = evaluate(metadata["global_size"][0], nc, rm)
    globalY = evaluate(metadata["global_size"][1], nc, rm)
    globalZ = evaluate(metadata["global_size"][2], nc, rm)
    localX = evaluate(metadata["local_size"][0], nc, rm)
    localY = evaluate(metadata["local_size"][1], nc, rm)
    localZ = evaluate(metadata["local_size"][2], nc, rm)
    kernelArgOrder = ",".join(metadata["argument_order"])

    # Add environment
    nenv['CK_XGEMM_KERNEL_FILE']=kernelFile
    nenv['CK_XGEMM_KERNEL_NAME']=kernelName
    nenv['CK_XGEMM_GLOBAL_X']=globalX
    nenv['CK_XGEMM_GLOBAL_Y']=globalY
    nenv['CK_XGEMM_GLOBAL_Z']=globalZ
    nenv['CK_XGEMM_LOCAL_X']=localX
    nenv['CK_XGEMM_LOCAL_Y']=localY
    nenv['CK_XGEMM_LOCAL_Z']=localZ
    nenv['CK_XGEMM_KERNEL_ARG_ORDER']=kernelArgOrder

    return {'return':0, 'bat':'', 'new_env':nenv}

###########################################################################
# This function either pre-sets choices or generates them
# It can also evaluate conditions!

def evaluate(data, new_choices, rm):
    if (data == "?"):
      if new_choices:
         pt=rm.randint(0,8)
         return 2**pt
      else:
         # return default value
         return 16
    return eval(data)

# Do not add anything here!
