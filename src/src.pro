TARGET	  =	tabletReader

HEADERS   = 	documentwidget.h \
                window.h \
                SlidingStackedWidget.h \
                flickable.h \
                worker.h \
                logger.h \
                filebrowsermodel.h \
		pdfdocument.h \
		document.h \
		djvudocument.h \
		kdjvu.h \
		chmdocument.h \
		chmreply.h
SOURCES   = 	documentwidget.cpp \
                main.cpp \
                window.cpp \
                SlidingStackedWidget.cpp \
                flickable.cpp \
                worker.cpp \
                logger.cpp \
                filebrowsermodel.cpp \
		pdfdocument.cpp \
		djvudocument.cpp \
		kdjvu.cpp \
		chmdocument.cpp \
		chmreply.cpp

QT           +=     declarative xml webkit
INCLUDEPATH  +=     /usr/include/poppler/qt4
INCLUDEPATH  +=     ../3rdparty_linux/libchmfile
INCLUDEPATH  +=     /usr/include/libdjvu
LIBS         +=     -L/usr/lib -lpoppler-qt4
LIBS         +=     -L/usr/lib -ldjvulibre
LIBS         +=     -L../3rdparty_linux/libchmfile -lchmfile -lchm
DEFINES      +=     NO_APPUP_AUTH_CODE
DEFINES      +=     QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
#DEFINES	     +=     DESKTOP_APP

CONFIG       +=     mobility
MOBILITY     +=     systeminfo

TRANSLATIONS += tabletReader_fr.ts \
                tabletReader_ro.ts

#install
target.path = /usr/bin
icon.files = tabletReader.png
icon.path = $$[install_prefix]/share/icons
desktop.files = tabletReader.desktop
desktop.path = $$[install_prefix]/share/applications

INSTALLS += target icon desktop

RESOURCES += images.qrc

OTHER_FILES += \
    qml/gotopage.qml \
    qml/Core/images/display.png \
    qml/Core/images/button-purple.png \
    qml/Core/images/button-green.png \
    qml/Core/images/button-blue.png \
    qml/Core/images/button-.png \
    qml/Core/Display.qml \
    qml/Core/calculator.js \
    qml/Core/Button.qml \
    qml/popupmenu.qml \
    qml/PopupMenu/PopupListModel.qml \
    qml/PopupMenu/PopupList.qml \
    qml/PopupMenu/ListItem.qml \
    qml/filebrowser.qml \
    qml/maintoolbar.qml \
    qml/Toolbar/ToolbarView.qml \
    qml/Toolbar/ToolbarItem.qml \
    qml/Toolbar/Toolbar.qml \
    qml/Toolbar/toolbar.js \
    qml/zoompage.qml \
    qml/zoompage/ZoomReel.qml \
    qml/zoompage/Reel.qml \
    qml/zoompage/Button.qml \
    qml/aboutdialog.qml \
    qml/ModalDialog/Visual.qml \
    qml/ModalDialog/ModalDialog.qml \
    qml/ModalDialog/Fader.qml \
    qml/ModalDialog/Button.qml \
    qml/ModalDialog/gfx/button_pressed.png \
    qml/ModalDialog/gfx/button_active.png \
    qml/ModalDialog/gfx/button.png \
    qml/wait.qml \
    qml/WaitDialog/WaitIndicator.qml \
    qml/WaitDialog/gfx/rotation_icon.png \
    qml/waitdialog.qml
