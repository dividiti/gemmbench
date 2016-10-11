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

    found_marker = False
    table = []

    for line in r['lst']:
        match = re.search('^\s*$', line) # check if line is empty
        # if the line is not empty and we have already found the marker we are in the table
        if (not match) and found_marker:
            table.append(line.split(";"))

        # using | <--       CLBlast       --> | as a marker
        header_regex = '(^\s*\|\s*<--\s*CLBlast\s*-->\s*\|$)'
        match = re.search(header_regex, line)
        if match:
            found_marker = True

    # transpose the table
    transposed_table = map(list, zip(*table))

    for column in transposed_table:
    	d[column[0].strip()] = map(lambda c: c.strip(), column[1:])

    if d != {}:
        d["post_processed"] = "yes"

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
