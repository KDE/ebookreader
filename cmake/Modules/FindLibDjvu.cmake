# Finds DJVU library

find_path ( DJVU_INCLUDE_DIR libdjvu/ddjvuapi.h )
find_library ( DJVU_LIBRARY NAMES djvulibre libdjvulibre )

IF( NOT("${DJVU_INCLUDE_DIR}" STREQUAL "") AND NOT("${DJVU_LIBRARY}" STREQUAL "") )
	message("-- Found Djvu library")
ENDIF()

