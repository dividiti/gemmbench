#
# Collective Knowledge (basic gemmbench crowdtuning)
#
# See CK LICENSE.txt for licensing details
# See CK COPYRIGHT.txt for copyright details
#
# Developer: Grigori Fursin, Grigori.Fursin@cTuning.org, http://fursin.net
#

cfg={}  # Will be updated by CK (meta description of this module)
work={} # Will be updated by CK (temporal data)
ck=None # Will be updated by CK (initialized CK kernel) 

# Local settings
line='****************************************************************'

rep1='remote-ck'
duoa1='SGEMM_NT-explore-f-n-lws'

##############################################################################
# Initialize module

def init(i):
    """

    Input:  {}

    Output: {
              return       - return code =  0, if successful
                                         >  0, if error
              (error)      - error text if return > 0
            }

    """
    return {'return':0}

##############################################################################
# start gemmbench crowdtuning

def start(i):
    """
    Input:  {
            }

    Output: {
              return       - return code =  0, if successful
                                         >  0, if error
              (error)      - error text if return > 0
            }

    """

    import copy
    import os

    # Setting output
    o=i.get('out','')
    oo=''
    if o=='con': oo='con'

    user=''

    curdir=os.getcwd()

    # Params
    hos=i.get('host_os','')
    tos=i.get('target_os', '')
    tdid=i.get('device_id', '')

    #**************************************************************************************************************
    # Welcome info
    ck.out(line)

    r=ck.load_text_file({'text_file':os.path.join(work['path'],'welcome.txt')})
    if r['return']==0:
       s=r['string']

       ck.out(s)

    user=''

    ck.out(line)
       
    r=ck.inp({'text':'Please, enter your email if you would like to identify your contributions as well as participate in monthly prize draws: '})
    user=r['string'].strip()

    r=ck.inp({'text':'Enter any integer seed: '})
    seed=r['string'].strip()
    if seed!='': seed=int(seed)

    #**************************************************************************************************************
    # Testing remote platform
    ck.out(line)
    ck.out('Testing experiment crowdsourcing server ...')
    ck.out('')

    ii={'action':'test',
        'module_uoa':cfg['module_deps']['program.optimization'],
        'out':'',
        'email':user,
        'repo_uoa':rep1}
    r=ck.access(ii)
    if r['return']>0: return r

    ck.out('  SUCCESS!')

    # Get local platform type
    r=ck.get_os_ck({})
    if r['return']>0: return r

    plat=r['platform']
    sext='.bat'
    if plat=='linux':
       sext='.sh'

    #**************************************************************************************************************
    # Preparing and starting crowd-tuning
    os.environ["CK_CROWDTUNING_USER"] = user

    # Load pipeline json
    r=ck.load_json_file({'json_file':'_setup_program_pipeline_tmp.json'})
    if r['return']>0: 
       if r['return']==16:
          ck.out(line)
          ck.out('Cleaning and setting up pipeline ...')

          os.system('_clean_program_pipeline'+sext)

          os.system('_setup_program_pipeline'+sext)
       else:
          return r

    # Restore path
    os.chdir(curdir)

    # Load autotuning json
    r=ck.load_json_file({'json_file':'explore-f-n-lws-remote.json'})
    if r['return']>0: return r
    d=r['dict']

    d['seed']=seed

    x=os.path.join(curdir, '_setup_program_pipeline_tmp.json')

    # Start crowdtuning
    d['action']='autotune'
    d['module_uoa']='pipeline'
    d['data_uoa']='program'
    d['pipeline_from_file']=x
    d['out']='con'

    r=ck.access(d)
    if r['return']>0: return r

    return {'return':0}

##############################################################################
# clean results

def clean(i):
    """
    Input:  {
            }

    Output: {
              return       - return code =  0, if successful
                                         >  0, if error
              (error)      - error text if return > 0
            }

    """

    import os

    # Find experiment
    r=ck.access({'action':'find',
                 'module_uoa':cfg['module_deps']['experiment'],
                 'data_uoa':duoa1})
    if r['return']==0:
       p=r['path']

       rx=ck.list_all_files({'path':p, 'add_path':'yes'})
       if rx['return']>0: return rx

       lst=rx['list']

       for q in lst:
           qq=lst[q]

           px=os.path.join(qq.get('path',''),q)
           if os.path.isfile(px):
              os.remove(px)

       # update entry with empty dictionary
       rx=ck.access({'action':'update',
                     'module_uoa':cfg['module_deps']['experiment'],
                     'data_uoa':duoa1,
                     'dict':{},
                     'substitute':'yes',
                     'ignore_update':'yes'})
       if rx['return']>0: return rx    

    return {'return':0}
