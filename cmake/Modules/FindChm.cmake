#Finds CHM library

find_path ( CHM_INCLUDE chm_lib.h )
find_library ( CHM_LIBRARIES NAMES chm libchm )

