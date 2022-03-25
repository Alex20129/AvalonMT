CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(AvalonMT)

SET(CMAKE_CXX_FLAGS "-O1 -no-pie")
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

LINK_LIBRARIES(Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
LINK_LIBRARIES(-static-libgcc -static-libstdc++)

ADD_EXECUTABLE(${PROJECT_NAME}
		logger.cpp
		main.cpp
		mainwindow.cpp
		asicdevice.cpp
		asicdevicemenu.cpp
		asictablewidget.cpp
		scannerwindow.cpp
		logger.hpp
		main.hpp
		mainwindow.h
		asicdevice.h
		asicdevicemenu.h
		asictablewidget.h
		scannerwindow.h
		mainwindow.ui
		asicdevicemenu.ui
		scannerwindow.ui
		avalonmt.qrc)

