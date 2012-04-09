#Finds Poppler-Qt4 library

find_path ( POPPLERQT4_INCLUDE poppler/qt4/poppler-qt4.h )
find_library ( POPPLERQT4_LIBRARIES NAMES poppler-qt4 libpoppler-qt4 )

