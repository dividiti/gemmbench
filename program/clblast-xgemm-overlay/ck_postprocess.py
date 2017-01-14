#
# Convert raw output of CLBlast
# to the CK timing format.
#
# Developers:
#   - Grigori Fursin, cTuning foundation, 2016
#   - Anton Lokhmotov, dividiti, 2016
#   - Michel Steuwer, University of Edinburgh, 2016
#

import json
import os
import re

def ck_postprocess(i):
    ck=i['ck_kernel']

    d={}

    env=i.get('env',{})

    r=ck.load_text_file({'text_file':'run.stdout','split_to_list':'yes'})
    if r['return']>0: return r

    found_header = False
    table = []
    column_count = 0

    for line in r['lst']:
 	if (not found_header):
	    #  m; n; k; layout; transA; transB; lda; ldb; ldc; offa; offb; offc; alpha; beta; ms_1; GFLOPS_1; GBs_1
	    header_regex = '(^\s+m;\s+n;\s+k;\s+layout;\s+transA;\s+transB;\s+lda;\s+ldb;\s+ldc;\s+offa;\s+offb;\s+offc;\s+alpha;\s+beta;\s+ms_1;\s+GFLOPS_1;\s+GBs_1\s*$)'
            match = re.search(header_regex, line)
  	    if match:
	        found_header = True
                header_line = line.split(";")
                column_count = len(header_line)
	        table.append(header_line)
	else: # we haven found the header
	    candidate_line = line.split(";")
	    if len(candidate_line) == column_count:
		table.append(candidate_line)
	    

    # transpose the table
    transposed_table = map(list, zip(*table))

    for column in transposed_table:
    	d[column[0].strip()] = map(lambda c: c.strip(), column[1:])

    if d != {}:
        d["post_processed"] = "yes"

        lms_1=d.get('ms_1',[])
        if len(lms_1)>0:
           ms=float(lms_1[0])
           d['time_total_ms']=ms
           d['time_total_ms_kernel_0']=ms
           s=ms*1e-3
           d['time_total_s']=s
           d['time_total_s_kernel_0']=s
           # Internal CK key to show overall time.
           d['execution_time']=s

        gbs_1=d.get('GBs_1',[])
        if len(gbs_1)>0:
           gbs=float(gbs_1[0])
           d['gbs']=gbs

        gflops_1=d.get('GFLOPS_1',[])
        if len(gflops_1)>0:
           gflops=float(gflops_1[0])
           d['gflops']=gflops

    rr={}
    rr['return']=0
    if d.get('post_processed','')=='yes':
       # Save to file.
       r=ck.save_json_to_file({'json_file':'tmp-ck-timer.json', 'dict':d})
       if r['return']>0: return r
    else:
       rr['return']=1
       rr['error']='failed to find the time in CLBlast output'

    return rr

# Do not add anything here!
