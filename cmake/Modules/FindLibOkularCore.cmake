#Finds Okular core library

find_path ( OKULAR_CORE_INCLUDE_DIR okular/core/document.h okular/core/page.h okular/core/generator.h)
find_library ( OKULAR_CORE_LIBRARY NAMES libokularcore )

IF( NOT("${OKULAR_CORE_INCLUDE_DIR}" STREQUAL "") AND NOT("${OKULAR_CORE_LIBRARY}" STREQUAL "") )	
	message("-- Found Okular core library")
ENDIF()

