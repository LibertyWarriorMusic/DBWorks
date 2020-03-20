#!/bin/bash

#NOTE Before you run this make file, you need need to move the following, from the Build directory to the directory with all the cpp and header files.
# images folder
#SQL folder
#settings.ini file
# To run this bash file, navigate to the folder that contains this file and from the terminal command promt $, run
#$ ./m.sh


clear
Files="DBGrid.cpp GenericItemForm.cpp ImagePanel.cpp MyEvent.cpp GenericTable.cpp PropertyTable.cpp main.cpp Utility.cpp HtmlHelp.cpp"

echo Making Database Works, please wait.....

clang++ -stdlib=libc++ -std=c++11 `wx-config --libs` `wx-config --cxxflags` -I/usr/local/cellar/mysql/8.0.19/include/mysql -I/usr/local/cellar/mysql++/3.2.5_1/include/mysql++ -L/usr/local/cellar/mysql++/3.2.5_1/lib -lmysqlpp.3.2.5 $Files  -o dbworks

# EXAMPLE WHY THIS IS WRONG. The mysql++ library uses the mysql library, the error below is linking with the mysql library and not the mysql++ library. 
#Dynamic Linking
#clang++ -stdlib=libc++ -std=c++11 `wx-config --libs` `wx-config --cxxflags` -I/usr/local/cellar/mysql/8.0.19/include/mysql -I/usr/local/cellar/mysql++/3.2.5_1/include/mysql++ -L/usr/local/cellar/mysql/8.0.19/lib $Files -o SpreadSheet


#otool -L spreadsheet
