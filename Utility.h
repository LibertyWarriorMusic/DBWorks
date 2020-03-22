//
// Created by Nicholas Zounis on 14/3/20.
//

#ifndef DBWORKS_UTILITY_H
#define DBWORKS_UTILITY_H

//The Utility class only holds static functions that can be used throughout the application.
class Utility {
public:
    static void ExtractSelectionItems(wxArrayString &sArray, const wxString& sToSearch);
    static void ExtractSelectionLookupItems(wxArrayString &sArray, wxString sFlag);
    static bool HasFlagReadOnly(const wxString& flag);
    static bool HasFlagMultiline(wxString flag);
    static bool HasFlagPassword(wxString flag);
    static bool HasFlagSelection(wxString flag);
    static bool HasFlagSelectionAdditive(wxString flag);
    static bool HasFlagLookupSelection(wxString flag);
    static bool HasFlagWeblink(wxString flag);
    static wxString ReplaceStringWithAsterix(wxString str);

    //Parsers
    static wxString PassHTMLDocument(wxString sDocument);
    static bool FindBetweenTags(wxString& ReturnString, wxString& sStringToSearch, int &iStartPosition, int &iEndPosition, wxString tag);
    static wxString ReplaceFieldTagsWithValuesFromTable(wxString Recordset, wxString QueryString);

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
    static void CreateSystemTables(wxString sDatabase);
    static bool DoesDatabaseExist(wxString sDatabase);
    static bool DoesTableExist(wxString sDatabase,wxString sTable);
    static wxString GetTableNameFromSYS_TABLES(long lTableId); // Loads an array with all values from a table given TableId and ColumnNumber
    static wxString GetTableFieldNameFromTable(wxString sTableName, long lColumnNumber); //Get a table fieldname given the TableName and ColumnNumber
    static void LoadStringArrayFromDatabaseTable(wxArrayString &sArray, long lTableId, long lColumnNumber);// Load an StringArray with from a table given the tableId and Column number
    static void ExecuteQuery(const wxString& QueryString); //Execute a query string
    static void ExecuteQuery(const wxString& QueryString, const wxString& sDatabase);
    static bool DoesTableExist(wxString sTableName);
    static bool DoesFieldExitInTable(const wxString& sTableName, const wxString& sTieldName);
    static bool IsEmpty(wxString str); // This function will remove any white spaces in the string before testing if empty.
    static bool GetFieldList(wxArrayString &fieldList, wxString TableId); // Gets a list of all the fields from a given table from the sys_fields table given a table ID.
    static wxString LoadSystemDocument(int documentId);
};


#endif //DBWORKS_UTILITY_H
