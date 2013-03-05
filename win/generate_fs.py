#!/usr/bin/env python

import shutil as sh
import os
import sys

<<<<<<< HEAD
=======
build_type = 'Debug'#Debug or Release
>>>>>>> 4f0b1d2... add script for copying all needed files
files_folder = "C:\\Users\\Bogdan Cristea\\Documents\\C++\\ebookreader\\win\\file_loc.txt"
src_root_path = "C:\\kderoot"
dst_root_path = "C:\\Users\\Bogdan Cristea\\Documents\\C++\\ebookreader\\build\\src"

<<<<<<< HEAD
def copy_file(src, dst):
    if os.path.isfile(src):
        dir = os.path.dirname(dst)
        if not os.path.exists(dir):
            os.makedirs(dir)
        sh.copy(src, dst)
    else:
=======
def copy_file(in_file):
    if 'release' in build_type.lower():
        file_list = (in_file)
    elif 'debug' in build_type.lower():
        base_fn, ext = os.path.splitext(in_file)
        file_list = (base_fn+'d'+ext,
                     base_fn[:(len(base_fn)-1)]+'d'+base_fn[-1]+ext,
                     in_file)#must be the last
    else:
        sys.error('unknown build type'+buildtype)
    for file in file_list:
        src = os.path.join(src_root_path, file)
        if os.path.isfile(src):
            dst = os.path.join(dst_root_path, file)
            dir = os.path.dirname(dst)
            if not os.path.exists(dir):
                os.makedirs(dir)
            sh.copy(src, dst)
            return
    for file in file_list:
        src = os.path.join(src_root_path, file)
>>>>>>> 4f0b1d2... add script for copying all needed files
        sys.stderr.write(src+" does not exist\n")

fd = open(files_folder)

for line in fd:
    file = line.rstrip()
<<<<<<< HEAD
    copy_file(os.path.join(src_root_path, file), os.path.join(dst_root_path, file))

fd.close()
=======
    copy_file(file)

fd.close()
>>>>>>> 4f0b1d2... add script for copying all needed files
