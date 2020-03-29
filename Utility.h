//
// Created by Nicholas Zounis on 14/3/20.
//

#ifndef DBWORKS_UTILITY_H
#define DBWORKS_UTILITY_H


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


WX_DECLARE_OBJARRAY(TableFieldItem, ArrayTableFields);

class myProgressControl;


//The Utility class only holds static functions that can be used throughout the application.
class Utility {
public:
    static wxString Replace(wxString searchString, wxString Old, wxString New, bool all=true);
    static double CalculateProgressStepsforImport(int iCount);
    static void ExtractSelectionItems(wxArrayString &sArray, const wxString& sToSearch);
    static void ExtractSelectionLookupItemsID(wxArrayString &sArray, wxString sFlag);
    static void ExtractSelectionLookupItemsName(wxArrayString &sArray, wxString sFlag);
    static void ExtractSelectionLinkedItemsID(wxArrayString &sArray, wxString sFlag);
    static void ExtractSelectionLinkedItemsName(wxArrayString &sArray, wxString sFlag);
    static bool HasFlag(wxString flags, wxString flag);
    static wxString ReplaceStringWithAsterix(wxString str);
    static bool IsReservedMySQLWord(wxString wordToFind);

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
    static wxString Escape(wxString & str);

    //DATETIME FUNCTIONS
    static wxString DateToString(const wxDateTime & dt);
    static wxDateTime StringToDate(const wxString & str);
    static wxString DateNow();

    //DATABASE UTILITIES
    static void CreateDatabase(wxString sDatabaseToCreate);
    static bool CreateSystemTables(wxString sDatabase);
    static bool DoesDatabaseExist(wxString sDatabase);
    //static bool DoesTableExist(wxString sDatabase,wxString sTable);
    static wxString GetTableNameFromSYS_TABLES(wxString sDatabase, wxString sTableId); // Loads an array with all values from a table given TableId and ColumnNumber
    static wxString GetTableTitleFromSYS_TABLES(wxString sDatabase, wxString sTableId); // Loads an array with all values from a table given TableId and ColumnNumber
    static wxString GetTableIdFromSYS_TABLESByTitle(wxString sDatabase, wxString sTableTitle);
    static wxString GetTableIdFromSYS_TABLES(wxString sDatabase, wxString sTableName); // Will return the table ID given the table name
    static wxString GetTableFieldNameFromTable(wxString sDatabase, wxString sTableName, wxString sColumnNumber); //Get a table fieldname given the TableName and ColumnNumber
    static void GetTableIDFromTableWhereFieldEquals(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldName, wxString value);
    static void GetFieldFromTableWhereFieldEquals(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldToGet, wxString sFieldName, wxString value);
    static void LoadStringArrayFromDatabaseTableByID(wxString sDatabase, wxArrayString &sArray, long lTableId, wxString sColumnNumber);// Load an StringArray with from a table given the tableId and Column number
    static void LoadStringArrayFromDatabaseTableByName(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldName);
    static void LoadStringArrayWidthMySQLDatabaseNames(wxArrayString &sArray);
    static void LoadStringArrayWithDatabaseTableNames( wxString sDatabase,  wxArrayString &sArray);
    static void ExecuteQuery(const wxString& QueryString); //Execute a query string
    static void ExecuteQuery(const wxString& sDatabase , const wxString& QueryString);
    static bool DoesTableExist(wxString sDatabase, wxString sTableName);
    static bool DoesRecordExist(wxString sDatabase, wxString sTable, wxString sFieldname, wxString sValue);// Check to see if a record with a particular value exists.
    static bool DoesFieldExitInTable(const wxString& sTableName, const wxString& sTieldName);
    static bool IsEmpty(wxString str); // This function will remove any white spaces in the string before testing if empty.
    static bool GetFieldList(wxArrayString &fieldList, wxString TableId); // Gets a list of all the fields from a given table from the sys_fields table given a table ID.
    static wxString LoadSystemDocument(int documentId);
    static wxString CreateTable(wxString sDatabase, wxString sTableName, ArrayTableFields m_Fields );
    static bool LoadFieldArrayWithTableFields(wxString sDatabase, wxString sTableName, ArrayTableFields &m_Fields );
    static wxString InsertTableInSYS_TABLES(wxString sDatabase, wxString sTableName); // Insert a new table into the sys_tables
    static void InsertFieldInSYS_FIELDS(wxString sDatabase, wxString sTableId, TableFieldItem fieldItem); //Insert a new table field
    static void AppendDBWorksDatabases(wxArrayString &arrayToAppend);
    static void SaveDatabaseToDBWorks(wxString sDatabaseNameToSave);

};


#endif //DBWORKS_UTILITY_H
