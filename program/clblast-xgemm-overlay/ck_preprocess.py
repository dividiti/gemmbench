#
# Preprocessing Caffe templates
#
# Developer: Michel Steuwer, University of Edinburgh, 2016
#            Grigori Fursin, cTuning foundation, 2016
#

import os

def ck_preprocess(i):

    global n,m,k # needed for evaluation

    ck=i['ck_kernel']
    rt=i['run_time']
    deps=i['deps']

    b=''

    env=i['env']
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

    globalX = evaluate(metadata["global_size"][0])
    globalY = evaluate(metadata["global_size"][1])
    globalZ = evaluate(metadata["global_size"][2])
    localX = evaluate(metadata["local_size"][0])
    localY = evaluate(metadata["local_size"][1])
    localZ = evaluate(metadata["local_size"][2])
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

    return {'return':0, 'bat':b, 'new_env':nenv}

def evaluate(data):
    if (data == "?"):
      return 16
    return eval(data)

# Do not add anything here!
