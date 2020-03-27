cmake_minimum_required(VERSION 3.15)

project(DBWorks)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        DBGrid.cpp
        GenericItemForm.cpp
        ImagePanel.cpp
        MyEvent.cpp
        GenericTable.cpp
        PropertyTable.cpp
        main.cpp
        Utility.cpp
        HtmlHelp.cpp
        ImportMySQLDatabase.cpp
        myProgressControl.cpp)

find_package(wxWidgets)
include(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES(${minimal_SOURCE_DIR}
        ${wxWidgets_INCLUDE_DIRS} )

add_executable(DBWorks ${SOURCE_FILES})
target_link_libraries(DBWorks ${LIBS} ${wxWidgets_LIBRARIES})
include_directories(/usr/local/cellar/mysql/8.0.19/include/mysql)
include_directories(/usr/local/cellar/mysql++/3.2.5_1/include/mysql++)
target_link_libraries(DBWorks mysqlpp.3.2.5)


