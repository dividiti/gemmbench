#
# Collective Knowledge (customized OpenCL autotuner for SGEMM Mali)
#
# See CK LICENSE.txt for licensing details
# See CK COPYRIGHT.txt for copyright details
#
# Developer: Grigori Fursin, Grigori.Fursin@cTuning.org, http://fursin.net
#

##############################################################################
# Make next multi-dimensional choice (with state)

def make(i):
    """
    Input:  {
              (pipeline)      - update pipeline with new values
              (random_module) - global random module (to set global seed)
              ck_kernel       - CK kernel
            }

    Output: {
              return       - return code =  0, if successful
                                         >  0, if error
              (error)      - error text if return > 0
              finish       - if True, stop autotuner
            }

    """

    # Init global random
    from random import Random

    mr=i.get('random_module',None)
    if mr==None: mr=Random()

    ck=i['ck_kernel']

    o=i.get('out','')

    finish=False

    pipeline=i.get('pipeline',{})

    keys={}

    # Setting dimensions X,Y,Z
    x=mr.randrange(128, 1024, 128)
    y=mr.randrange(128, 1024, 128)
    z=mr.randrange(128, 1024, 128)

    keys['##env#MTX_A']=str(x)+','+str(y)
    keys['##env#MTX_B']=str(y)+','+str(z)

    # Setting LWS
    lx=mr.randrange(2,32,2)
    keys['##env#CK_LWS_X']=lx
    keys['##env#CK_LWS_Y']=mr.randrange(2,128/lx,2)

    # GEMM TYPE
    keys['##env#GEMM_TYPE']=mr.randrange(0,2)

    # CK_SKIP_PADDING"
#    keys['##env#CK_SKIP_PADDING']=mr.randrange(0,1)

    return {'return':0, 'keys':keys, 'finish':finish}
