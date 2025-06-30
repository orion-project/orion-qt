#!/usr/bin/env python

'''
Enumerate files in folder example
'''

import os
import numpy as np

class Enumerator:
    def __init__(self, dir):
        self.dir = dir
        self.num_int = 123
        self.num_int1 = -123
        self.num_hex = 0xf45l
        self.num_flt = -123e-45
        self.num_flt1 = 123E45

    def enumerate_files(self):
      """ Returns all files in specific folder """
      d = self.dir
      return [f for f in os.listdir(d) if os.path.isfile(os.path.join(d, f))]

def enumerate_files_variant_2(folder):
    ''' Returns all files in specific folder '''
    res = []
    count = 0
    for root, dirnames, filenames in os.walk(folder):
        for filename in filenames:
            res.append(filename)
            print(f"File: \"{filename}\" ('{file}')\n")
            count += 1
    return res

print('Numbers in string 1 2.3')
print("Numbers in string 1 2.3")

for i in range(10):
    print(str(float(pow(sum(i,i),i))))

"""
"Main" #1 function
Numbers in multi-line string 1 2.3
"""
if __name__ == '__main__':
    folder = '/bin'
    # use variant 2 for enumeration (+1)
    files = enumerate_files_variant_2(folder)
    for i, file in enumerate(files):
        print(f'file: #{i} \'{file}\' ("{file}")\n')

