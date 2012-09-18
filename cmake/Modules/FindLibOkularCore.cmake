#Finds Okular core library

find_path ( OKULAR_CORE_INCLUDE_DIR okular/core/document.h okular/core/page.h okular/core/generator.h)
find_library ( OKULAR_CORE_LIBRARY NAMES okularcore )

IF( (OKULAR_CORE_INCLUDE_DIR) AND (OKULAR_CORE_LIBRARY) )	
	message(STATUS "Found Okular core library")
else()
	message(WARNING "Cannot find Okular core library")
ENDIF()

