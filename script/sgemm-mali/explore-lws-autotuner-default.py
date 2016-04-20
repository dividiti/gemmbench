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

    ccur=i.get('choices_current',[])

    # Setting dimensions X,Y,Z

#   Purely random
#    x=mr.randrange(128, 1024, 128)
#    y=mr.randrange(128, 1024, 128)
#    z=mr.randrange(128, 1024, 128)

    # Typical sizes from AlexNet (profiled by Anton)
    sizes=[
      [ 1,   64, 3024,  360],
      [ 2,  192,  728, 1600],
      [ 3,  384,  168, 1728],
      [ 4,  256,  168, 3456],
      [ 5,  256,  168, 2304],

      [ 6, 2304,  168,  256],
      [ 7, 3456,  168,  256],
      [ 8, 1728,  168,  384],
      [ 9, 1600,  728,  192],
      [10,  360, 3024,   64],

      [11,  256, 3456,  168],
      [12,  384, 1728,  168],
      [13,  192, 1600,  728],
      [14,   64,  360, 3024],
      [15,  256, 2304,  168]
    ]

    if len(ccur)==0:
       ind=0
       ccur.append(0) # first index
    else:
       ind=ccur[0]+1
       ccur[0]=ind
       if ind>=len(sizes):
          finish=True

    if not finish:
       cur=sizes[ind]#

       n=cur[0]
       x=cur[1]
       y=cur[2]
       z=cur[3]

       keys['##env#MINDEX']=n

       keys['##env#MTX_A']=str(x)+','+str(y)
       keys['##env#MTX_B']=str(y)+','+str(z)

    # Default LWS
    keys['##env#CK_LWS_X']=4
    keys['##env#CK_LWS_Y']=8

    # GEMM TYPE
    keys['##env#GEMM_TYPE']=mr.randrange(0,2)

    # CK_SKIP_PADDING"
#    keys['##env#CK_SKIP_PADDING']=mr.randrange(0,1)

    return {'return':0, 'keys':keys, 'finish':finish}
