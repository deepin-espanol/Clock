# Clock
Simple clock application for Deepin. It offers viewing different clocks of the timezones all over the world. You can also add alarms, use the stopwatch or set a timer.

![Preview](https://raw.githubusercontent.com/deepin-espanol/Clock/master/preview.gif)

## Dependencies
```
 - Qt Core
 - Qt Widgets
 - Qt Gui
 - Qt Network
 - Qt DBus
 - Dtk Core
 - Dtk Widgets
 - DFrameworkDBus (Dtk)
```
*If you want to compile this project, you'll need to get the development files too.*

## How to compile
In the source directory, where Clock.pro is located, use the following to get the binary in a build directory:
```
$ mkdir build
$ cd buil
$ qmake ../
$ make
```

## To execute
If you've followed the compilation process mentioned before, you can use:
```
$ ./Clock
```
