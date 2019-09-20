#!/usr/bin/python

from __future__ import print_function

import numpy as np
import h5py
import sys
if len(sys.argv) == 1:
    print("Usage:{0} feature.dat feature_row feature_column output.h5".format(sys.argv[0]))
    exit(1)

data = np.fromfile(sys.argv[1], dtype='float32')
data = np.reshape(data, (int(sys.argv[2]), int(sys.argv[3])))
h5f = h5py.File(sys.argv[4], 'w')
h5f.create_dataset('data', data = data)
h5f.close()
print("Generate {0}x{1} matrix feature for training to {2}".format(sys.argv[2],sys.argv[3],sys.argv[4]))
