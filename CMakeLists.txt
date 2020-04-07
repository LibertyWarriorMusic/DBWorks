cmake_minimum_required(VERSION 3.15)

project(DBWorks)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        main.cpp
        MyEvent.cpp
        PropertyTable.cpp
        Utility.cpp
        HtmlHelp.cpp
        Generic/DBGrid.cpp
        Generic/GenericItemForm.cpp
        Generic/GenericTable.cpp
        CustomControls/myProgressControl.cpp
        TableDefDiagram/DiagramFrame.cpp
        TableDefDiagram/ObTablePanel.cpp
        TableDefDiagram/ObTable.cpp
        TableDefDiagram/TableDiagramFrame.cpp
        Dialog/DialogBaseClass.cpp
        Dialog/ImportMySQLDatabase.cpp
        Dialog/DialogTemplate.cpp
        Dialog/DlgTableSelect.cpp)

#Un-comment for dynamic linking
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

#SET(CMAKE_PREFIX_PATH /usr/local/opt/mysql++/include/mysql++)
#find_package(mysqlpp REQUIRED)

#Un-comment for dynamic linking
#add_subdirectory(LIBS)


#Leave uncommented for both static and dynamic linking
add_executable(DBWorks ${SOURCE_FILES})
target_link_libraries(DBWorks ${LIBS} ${wxWidgets_LIBRARIES})




# We don't want to use these funcions
include_directories(/usr/local/opt/mysql/include/mysql)
include_directories(/usr/local/opt/mysql++/include/mysql++)
#target_include_directories(DBWORKS PUBLIC /usr/local/opt/mysql++/include/mysql++)



#target_link_directories(DBWorks PUBLIC /usr/local/opt/mysql++/include/mysql++)
#target_link_directories(DBWorks PUBLIC ../mysqlpp.3)

target_link_libraries(DBWorks mysqlpp.3)


