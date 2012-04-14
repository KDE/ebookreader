#Finds Poppler-Qt4 library

find_path ( POPPLER_QT4_INCLUDE_DIR poppler/qt4/poppler-qt4.h )
find_library ( POPPLER_QT4_LIBRARY NAMES poppler-qt4 libpoppler-qt4 )

