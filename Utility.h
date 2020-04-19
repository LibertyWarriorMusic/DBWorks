//
// Created by Nicholas Zounis on 14/3/20.
//

#ifndef DBWORKS_UTILITY_H
#define DBWORKS_UTILITY_H

//This class is used to completely define a field in a MySQL table. It corresponds directly to the mySql fields.
class TableFieldItem
{
public:
    wxString fieldName;
    wxString fieldType;
    wxString fieldNull;
    wxString fieldKey;
    wxString fieldDefault;
    wxString fieldExtra;
};

//This class is used to store associated data into a combo control
class ComboDataItem : public wxClientData
{
public:
    wxString title;
    wxString tableID;
    wxString linkedTableID;
    wxString queryDefinition;
    wxString description;
};


WX_DECLARE_OBJARRAY(TableFieldItem, ArrayTableFields);

class myProgressControl;


//The Utility class only holds static functions that can be used throughout the application.
class Utility {
public:
    static wxString Replace(wxString searchString, wxString Old, wxString New, bool all=true);
    static double CalculateProgressStepsforImport(int iCount);
    static void ExtractSelectionItems(wxArrayString &sArray, const wxString& sToSearch);
    static bool ExtractSelectionLookupItemsID(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables=false);
    static bool ExtractSelectionLookupItemsName(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables=false);
    static bool ExtractSelectionLinkedItemsID(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables=false);
    static bool ExtractSelectionLinkedItemsName(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables=false);
    static bool HasFlag(wxString flags, wxString flag);
    static wxString ReplaceStringWithAsterix(wxString str);
    static bool IsReservedMySQLWord(wxString wordToFind);

    //Unicode and wxString Conversions
    static void EscapeAscii(wxString& QueryString);
    static wxString Escape(wxString & str);
    static wxString EscapeRemove(wxString & str);
    static int StringToInt(const wxString& stringToConvert);
    static wxString IntToString(const int & num);
    static long StringToLong(const wxString& stringToConvert);



    //Number Functions
    static bool IsExactlyDivisable(double num, double div);

    //Combo loading functions
    static void LoadComboUsrFilters(wxString sDatabase, wxComboBox &pCombo, wxString associatedTableId);
    static void FillComboFromStringArray(wxComboBox *pCombo, const wxArrayString sArray);
    static void DestroyComboDataObjects(wxComboBox *pCombo);

    //Rect and Calc Functions
    static bool IsPointInRect(wxPoint pt, wxRect rect); // returns true if the point lies inside the rectangle.
    static wxSize CalcPtInRectOffset(wxPoint pt, wxRect rect); // When you click inside a rect with the mouse, the pt offset from the top left corner of the rect is calculated.
    static wxRect CalcGridRectThatPtLiesInside(wxPoint pt,wxSize gridSize);// Find the grid rect that the point lies. The grid is fixed on the screen.
    static wxPoint CalPtSnapToClosestCornerInRect(wxPoint pt, wxRect rect); // Will return the pt at one of the corners of the rect that the point is closest too.

    //Parsers
    static wxString PassHTMLDocument(wxString sDocument);
    static bool FindBetweenTags(wxString& ReturnString, wxString& sStringToSearch, int &iStartPosition, int &iEndPosition, wxString tag);
    static wxString ReplaceFieldTagsWithValuesFromTable(wxString Recordset, wxString QueryString);
    static bool DoesSelectionExistInCombobox(wxComboBox *combo,   wxString sSelectionLabel);

    //LOAD FILE
    static bool LoadBitmap(wxBitmap &image, wxString ImageFileName);

    //USER FUNCTIONS
    static bool IsAdvancedUser();
    static bool IsStandardUser();
    static bool IsSystemDatabaseDeveloper();
    static bool IsSystemDatabaseAdministrator();
    static bool IsGuest();
    static bool IsCustomUser();


    //DATETIME FUNCTIONS
    static wxString DateToString(const wxDateTime & dt);
    static wxDateTime StringToDate(const wxString & str);
    static wxString DateNow();

    //TABLE DATA FUNCTIONS
    static bool LoadTableData(wxString sDatabase, wxString sTableName, wxString KeyName, wxString &sData, wxString &entireDataString, int &startPosOfData);
    static bool LoadTableData(wxString sDatabase, wxString sTableName, wxString KeyName, wxString &sData);
    static void SaveTableData(const wxString& sDatabase, const wxString& sTableName, const wxString& KeyName, wxString data);
    static void SaveTableDataBulk(wxString sDatabase,wxString sTableName, wxString KeyDataCombination);
    static wxString GetTableDataString(wxString sDatabase, wxString sTableId);

    //DATABASE UTILITIES
    static void CreateDatabase(wxString sDatabaseToCreate);
    static bool CreateSystemTables(wxString sDatabase);
    static bool DoesDatabaseExist(wxString sDatabase);
    //static bool DoesTableExist(wxString sDatabase,wxString sTable);
    static wxString GetTableNameFromSYS_TABLESById(wxString sDatabase, wxString sTableId); // Loads an array with all values from a table given TableId and ColumnNumber
    static wxString GetTableTitleFromSYS_TABLESById(wxString sDatabase, wxString sTableId); // Loads an array with all values from a table given TableId and ColumnNumber
    static wxString GetTableIdFromSYS_TABLESByTitle(wxString sDatabase, wxString sTableTitle);
    static wxString GetTableIdFromSYS_TABLESByTableName(wxString sDatabase, wxString sTableName); // Will return the table ID given the table name
    static wxString GetTableFieldNameFromMySQLInfoSchema(wxString sDatabase, wxString sTableName, wxString sColumnNumber); //Get a table fieldname given the TableName and ColumnNumber
    static bool GetFieldListFromSysFieldsByTableId(ArrayTableFields &fieldList, wxString TableId); // Gets a list of all the fields from a given table from the sys_fields table given a table ID.



    static void GetTableIDFromTableWhereFieldEquals(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldName, wxString value);
    static bool GetFieldFromTableWhereFieldEquals(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldToGet, wxString sFieldName, wxString value);


    static void LoadStringArrayFromDatabaseTableByID(wxString sDatabase, wxArrayString &sArray, long lTableId, wxString sColumnNumber);// Load an StringArray with from a table given the tableId and Column number
    static void LoadStringArrayFromDatabaseTableByName(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldName);
    static void LoadStringArrayWidthMySQLDatabaseNames( wxArrayString &sArray);
    static void LoadStringArrayWithMySQLTableNames(wxString sDatabase, wxArrayString &sArray );
    static void LoadStringArrayAvailableQueriesFromUsrQueries(wxArrayString &sArray );
    static bool ExecuteQueryEscapeAscii(const wxString& QueryString); //Execute a query string
    static bool ExecuteQuery(const wxString& sDatabase , const wxString& QueryString);
    static bool DoesTableExist(wxString sDatabase, wxString sTableName);
    static bool DoesRecordExist(wxString sDatabase, wxString sTable, wxString sFieldname, wxString sValue);// Check to see if a record with a particular value exists.
    static bool DoesFieldExitInTable(const wxString& sTableName, const ArrayTableFields& fieldItemList); //Checks all the mysql field data, name, type , null ,key, default, extra
    static bool SynFieldDefinitionToTable(const wxString& sTableName,  const ArrayTableFields& fieldItemList);
    static bool DropFieldsInTableThatAreNotInTableDefinition(const wxString& sTableName,  const ArrayTableFields& fieldItemList);
    static bool DoesFieldExitInTable(const wxString& sTableName, const wxString& sFieldName);// Only checks the fieldname
    static bool IsEmpty(wxString str); // This function will remove any white spaces in the string before testing if empty.

    //Query Builder Helper functions
    static wxString RemoveTableFromSelectQuery(wxString sQuery, wxString sTableName);
    static void LoadStringArrayWithTableNamesFromUpdateQuery(wxString sQuery, wxArrayString &sArray);
    static void LoadStringArrayWithTableNamesFromSelectQuery(wxString sQuery, wxArrayString &sArray);
    static wxString GetTableNamesFromInsertQuery(wxString sQuery);
    static bool DoesStringExistInStringArray(const wxString& sToCheck, const wxArrayString &sArray);
    static wxString GetFirstStringFromRight(wxString str);

    static bool GetFieldListByQuery(wxArrayString &ArrayFieldNames, wxString sQueryString);

    static wxString LoadSystemDocument(int documentId);
    static wxString CreateTable(wxString sDatabase, wxString sTableName, ArrayTableFields m_Fields );
    static bool LoadFieldArrayWithTableFields(wxString sDatabase, wxString sTableName, ArrayTableFields &m_Fields );
    static wxString InsertTableInSYS_TABLES(wxString sDatabase, wxString sTableName); // Insert a new table into the sys_tables
    static void InsertFieldInSYS_FIELDS(wxString sDatabase, wxString sTableId, TableFieldItem fieldItem); //Insert a new table field
    static void AppendDBWorksDatabases(wxArrayString &arrayToAppend);
    static bool SaveDatabaseToDBWorks(wxString sDatabaseNameToSave);
   // static void DestroyFieldItemList(ArrayTableFields &fieldList); // Runs through the list and destroys all the items.

   //Generic Database Functions
   static  bool GetSingleFieldRecords(wxString sQueryString, wxString FieldToGet,wxArrayString& ArrayFieldResults);

   //UPDATE
   static void UpdateTableFieldById(wxString sDatabase, wxString sTableName, wxString sTableId, wxString sFieldname, wxString sValue);

    static void LoadStringArrayWithTableNamesFromSysTables( wxString sDatabase,  wxArrayString &sArray, bool HiddenTablesOnly = false);//Gets all the tables in the database, NOT from sys_tables;
    static void LoadStringArrayWithTableIdsFromSysTables( wxString sDatabase,  wxArrayString &sArray);//Gets all the tables in the database, NOT from sys_tables;
};


#endif //DBWORKS_UTILITY_H
