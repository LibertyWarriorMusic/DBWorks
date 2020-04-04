cmake_minimum_required(VERSION 3.15)

project(DBWorks)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        DBGrid.cpp
        GenericItemForm.cpp
        MyEvent.cpp
        GenericTable.cpp
        PropertyTable.cpp
        main.cpp
        Utility.cpp
        HtmlHelp.cpp
        myProgressControl.cpp
        ObTablePanel.cpp
        ObTable.cpp DiagramFrame.cpp
        TableDiagramFrame.cpp
        DialogBaseClass.cpp
        ImportMySQLDatabase.cpp)



#Un-comment for dynamic linking
find_package(wxWidgets)
include(${wxWidgets_USE_FILE})


#Un-comment for dynamic linking
INCLUDE_DIRECTORIES(${minimal_SOURCE_DIR}
        ${wxWidgets_INCLUDE_DIRS} )



#TRYING TO STATIC LINK WXWIDGETS LIBRARY
#link_directories(/Users/nicholaszounis/documents/github/databaseworks/wxWidgets-3.0.4/build-cocoa-debug/lib)


#Leave uncommented for both static and dynamic linking
add_executable(DBWorks ${SOURCE_FILES})

#Un-comment for dynamic linking
target_link_libraries(DBWorks ${LIBS} ${wxWidgets_LIBRARIES})

#In-comment for static linking - TRYING TO STATIC LINK WXWIDGETS LIBRARY
#include_directories(/Users/nicholaszounis/documents/github/databaseworks/wxWidgets-3.0.4/include/wx)
#include_directories(/Users/nicholaszounis/documents/github/databaseworks/wxWidgets-3.0.4/include/wx/cocoa)

#target_link_libraries(DBWorks wx_osx_cocoau_stc-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_adv-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_core-3.0)
#target_link_libraries(DBWorks wx_baseu-3.0)


#target_link_libraries(DBWorks wx_baseu_net-3.0)
#target_link_libraries(DBWorks wx_baseu_xml-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_aui-3.0)

#target_link_libraries(DBWorks wx_osx_cocoau_gl-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_html-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_media-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_propgrid-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_qa-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_ribbon-3.0)
#target_link_libraries(DBWorks wx_osx_cocoau_richtext-3.0)


#target_link_libraries(DBWorks wxregexu-3.0)
#target_link_libraries(DBWorks wxscintilla-3.0)
#-- END TRYING TO LINK STATICALLY

#Keep commented - I combined the dynamic library in my distribution package but the application is still linking to the following directory
# when I use otool -L  DBWorks to examine the executable linking libraries.
#include_directories(/usr/local/cellar/mysql/8.0.19/include/mysql)
#include_directories(/usr/local/cellar/mysql++/3.2.5_1/include/mysql++)

#Un-comment for DYNAMIC LINKING - mysql
link_libraries(/usr/local/opt/mysql/lib)

#Unfortunatly, only the dynamic libraries are available.
link_libraries(/usr/local/opt/mysql++/lib)

#target_link_libraries(DBWorks libmysqlclient.a)
#target_link_libraries(DBWorks libmysqlcppconn-static.a)

include_directories(/usr/local/opt/mysql/include/mysql)
include_directories(/usr/local/opt/mysql++/include/mysql++)

target_link_libraries(DBWorks mysqlpp.3)


