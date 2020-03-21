# CLASSES
The following is a description of the classes and what they do.

###file: main.cpp  - class MyApp 
######The starting point for all wxWidget application that holds the main frame window.


###file: main.cpp - class MainFrame 
######The main frame for the application that contains a DBGrid class which holds all 
######the tables that users create.


###file: PropertyTable.cpp - class PropertyTable 
###### This is the form that you edit your table, creating and altering fields 
###### for your tables. This class inherits from GenericTable class which contains
###### most of the functionality for displaying the data. The tables and fiels are stored
###### in the information database tables DBWORKSTABLES and TABLEFIELD.


###file: GenericTable.cpp - class GenericTable 
###### When users open their table, this form is present with a grid for their data.
###### Users and add, edit, view or delete their own entries. This class creates a
###### DBGrid class to display records from user tables.

###file: GenericItemForm.cpp - class GenericItemForm 
###### When a users edits or views a single entry, this is the form that is presented.
###### The user can update or view a single record.
######

###file: DBGrid.cpp - class DBGrid
###### This is the grid class that displays user or system records. A right context menu is available
###### that can send messages back to the main frame for refresh, edit, update etc.
######

//===========================================

