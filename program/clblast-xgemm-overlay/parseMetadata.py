#!/usr/bin/python
import sys
import json
import os

json_file = str(sys.argv[1])
n = int(sys.argv[2])
m = int(sys.argv[3])
k = int(sys.argv[4])

def evaluate(data):
    if (data == "?"):
      return 16
    return eval(data)

with open(json_file) as data_file:    
    metadata = json.load(data_file)

kernelFile = os.path.splitext(json_file)[0]+'.cl'
kernelName = metadata["kernel_name"]
globalX = evaluate(metadata["global_size"][0])
globalY = evaluate(metadata["global_size"][1])
globalZ = evaluate(metadata["global_size"][2])
localX = evaluate(metadata["local_size"][0])
localY = evaluate(metadata["local_size"][1])
localZ = evaluate(metadata["local_size"][2])
kernelArgOrder = ",".join(metadata["argument_order"])

print '-kernelFile {} -kernelName {} -globalX {} -globalY {} -globalZ {} -localX {} -localY {} -localY {} -kernelArgOrder {}'.format(
	kernelFile,    kernelName,    globalX,    globalY,    globalZ,    localX,    localY,    localZ,    kernelArgOrder)
