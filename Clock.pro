QT       += core gui dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 link_pkgconfig
PKGCONFIG += dtkcore dtkwidget dframeworkdbus

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	src/delegates/stopsdelegate.cpp \
	src/external/file_util.cpp \
	src/external/popup_menu.cpp \
	src/external/popup_menu_delegate.cpp \
	src/external/qtsingleapplication/dsingleapplication.cpp \
	src/external/qtsingleapplication/qtlocalpeer.cpp \
	src/external/qtsingleapplication/qtlockedfile.cpp \
	src/external/qtsingleapplication/qtlockedfile_unix.cpp \
	src/external/sunrise.cpp \
	src/external/timezone.cpp \
	src/external/timezone_map.cpp \
	src/external/timezone_map_util.cpp \
	src/external/timezonechooser.cpp \
	src/external/tooltip_pin.cpp \
	src/external/widgets/basiclistdelegate.cpp \
	src/external/widgets/basiclistmodel.cpp \
	src/external/widgets/basiclistview.cpp \
	src/external/widgets/searchinput.cpp \
	src/delegates/alarmsdelegate.cpp \
	src/delegates/timezonesdelegate.cpp \
	src/lists/alarmslist.cpp \
	src/lists/stopslist.cpp \
	src/lists/timezoneslist.cpp \
	src/models/alarmsmodel.cpp \
	src/models/stopsmodel.cpp \
	src/models/timezonesmodel.cpp \
	src/utils/circlepainter.cpp \
	src/utils/clock.cpp \
	src/utils/configoperator.cpp \
	src/widgets/advancedclasses.cpp \
	src/widgets/alarmswidget.cpp \
	src/widgets/dualwidgetcontainer.cpp \
	src/widgets/inwinpopup.cpp \
	src/widgets/mainwindow.cpp \
	src/widgets/numberspinbox.cpp \
	src/widgets/stopwatch.cpp \
	src/widgets/stopwatchwidget.cpp \
	src/widgets/tabchooser.cpp \
	src/widgets/timeeditor.cpp \
	src/widgets/timerwidget.cpp \
	src/widgets/worldclockswidget.cpp \
	src/main.cpp

HEADERS += \
	src/delegates/stopsdelegate.h \
	src/external/file_util.h \
	src/external/popup_menu.h \
	src/external/popup_menu_delegate.h \
	src/external/qtsingleapplication/dsingleapplication.h \
	src/external/qtsingleapplication/qtlocalpeer.h \
	src/external/qtsingleapplication/qtlockedfile.h \
	src/external/sunrise.h \
	src/external/timezone.h \
	src/external/timezone_map.h \
	src/external/timezone_map_util.h \
	src/external/timezonechooser.h \
	src/external/tooltip_pin.h \
	src/external/widgets/basiclistdelegate.h \
	src/external/widgets/basiclistmodel.h \
	src/external/widgets/basiclistview.h \
	src/external/widgets/searchinput.h \
	src/delegates/alarmsdelegate.h \
	src/delegates/timezonesdelegate.h \
	src/lists/alarmslist.h \
	src/lists/stopslist.h \
	src/lists/timezoneslist.h \
	src/models/alarmsmodel.h \
	src/models/stopsmodel.h \
	src/models/timezonesmodel.h \
	src/utils/circlepainter.h \
	src/utils/clock.h \
	src/utils/configoperator.h \
	src/widgets/advancedclasses.h \
	src/widgets/alarmswidget.h \
	src/widgets/dualwidgetcontainer.h \
	src/widgets/inwinpopup.h \
	src/widgets/mainwindow.h \
	src/widgets/numberspinbox.h \
	src/widgets/stopwatch.h \
	src/widgets/stopwatchwidget.h \
	src/widgets/tabchooser.h \
	src/widgets/timeeditor.h \
	src/widgets/timerwidget.h \
	src/widgets/worldclockswidget.h \
	src/helpers.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += main.qrc
