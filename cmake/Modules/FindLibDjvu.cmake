# Finds DJVU library

find_path ( DJVU_INCLUDE_DIR libdjvu/ddjvuapi.h )
find_library ( DJVU_LIBRARY NAMES djvulibre libdjvulibre )

