# Finds DJVU library

find_path ( DJVU_INCLUDE libdjvu/ddjvuapi.h )
find_library ( DJVU_LIBRARIES NAMES djvulibre libdjvulibre )

