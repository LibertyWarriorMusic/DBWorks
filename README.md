# DBWorks
Generic database solution using wxWidgets - C++ - MySQL

ACTIVELY UNDER DEVELOPMENT MARCH 2020


//===========================================

# INTRODUCTION:

The goal of this project is for users to easily create and link tables into a database. This project is great for people who want to learn about wxWidgets using C++ and MySQL. wxWidgets is a cross platform library so this project can be compiled on Windows, Mac, Linux, Unix. I am currently developing on the mac.


//===========================================

# PRE REQUIREMENTS:

WxWidgets - Install the WxWidgets library using homebrew > brew install wxwidgets
MySQL - This project requires MySQL to be installed on your machine. > brew install mysql
MYSQL++ - It also requires the MySQL++ library to be installed. > brew install mysql++


//===========================================

# OPEN POSITIONS:

I'm currently looking for people who wish to use and develop this project further with me. The code is free to use, copy, modify and even re-licence for your own personal or company use, but changes to this repository need to be cleared by me initially. You can use this code as a starting point for your own projects, or actively help develop this version with me. 



//===========================================

# GETTING STARTED

-- Mac OS setup --
First you need to install MySQL on your system and then the mysql++ library.
You can do this on a mac with brew, the install commands are -

>brew install wxwidgets

>brew install mysql

>brew install mysql++

Check the paths from my bash file m.sh to see if the headers and libraries match with your system. 

Create a database called information, make sure you change the settings.ini file to match your connection details, especially the password.

To compile the project, navigate to the project folder and run. Note that the project has been setup to also work with CLion, however if you don't have CLion, I provided this compile file bash file.

>./m.sh

It should complete with no errors. If you have linking errors, check that the library files are linked correctly, you can email me for help to get started. 

-- Win Setup --
I am looking for people to contribute here and compile this application on a windows environment.

-- Linux Setup --
I am looking for people to contribute here and compile this application on a linus environment.



//===========================================

# FILE DESCRIPTIONS:

m.sh - Is a bash file that compiles the project on my mac, I haven't compiled on other platforms as yet.

CreateDatabase.sql - This creates the tables that the project relys. You need to first create a information database.

settings.ini - The settings file for this project, you can change the database and connection details here.

global.h - This corresponds directly with the settings.ini file. This structure gets loaded from the settings file on boot up.

xxx.h - All the header files for the project.

xxx.cpp - All the cpp files for the project.


//===========================================

# NOTES:

LINKING: I am currently dynamically linking this project, so you will need the MYSQL++ library installed.
I use xcode on my mac, not to compile or debug, only as an editor. I use the bash file m.sh to compile the project.
If your downloading on another platform, you will have a different IDE platform to configure.



//===========================================

# CONTACT INFORMTION:
If you want to get involved in this project, or need help setting it, you can email me directly.
nickzounis@hotmail.com


