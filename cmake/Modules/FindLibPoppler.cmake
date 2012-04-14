#Finds Poppler library

find_path ( POPPLER_INCLUDE_DIR poppler/poppler-config.h )
find_library ( POPPLER_LIBRARY NAMES poppler libpoppler )

