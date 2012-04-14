#Finds CHM library

find_path ( CHM_INCLUDE_DIR chm_lib.h )
find_library ( CHM_LIBRARY NAMES chm libchm )

