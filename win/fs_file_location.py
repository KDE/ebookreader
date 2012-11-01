#!/usr/bin/env python

#lists all files with the relative path wrt the root folder

import os

fd = open("file_loc.txt", "w")
root_install_folder = "C:\\Users\\Bogdan Cristea\\Desktop\\tabletReader\\tabletReader"

def list_files(folder_path):
    for file in os.listdir(folder_path):
        file_path = os.path.join(folder_path, file)
        if os.path.isfile(file_path):
            fd.write(os.path.relpath(file_path, root_install_folder)+'\n')
        elif os.path.isdir(file_path):
            list_files(file_path)

list_files(root_install_folder)
fd.close()