#Finds Poppler library

find_path ( POPPLER_INCLUDE_DIR poppler/poppler-config.h)
find_library ( POPPLER_LIBRARY NAMES poppler libpoppler )

IF( NOT("${POPPLER_INCLUDE_DIR}" STREQUAL "") AND NOT("${POPPLER_LIBRARY}" STREQUAL "") )	
	message("-- Found Poppler library")
ENDIF()

