#Finds Poppler library

find_path ( POPPLER_INCLUDE poppler/poppler-config.h )
find_library ( POPPLER_LIBRARIES NAMES poppler libpoppler )

