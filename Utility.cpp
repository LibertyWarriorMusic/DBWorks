//
// Created by Nicholas Zounis on 14/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include <wx/stdpaths.h>
#include "global.h"
#include "Utility.h"
#include <mysql.h>
#include <mysql++.h>

using namespace mysqlpp;

wxString Utility::PassHTMLDocument(wxString sDocument)
{

    int iStartPosition=0;
    int iEndPosition =0;
    int iLength = 0;

    iLength = sDocument.Length();
    wxString QueryString = "";

    FindBetweenTags(QueryString,sDocument,iStartPosition,iEndPosition,"<query>");

    if (QueryString.IsEmpty()){
        //Ok, we found our first query.
        //Remove the query string

        return sDocument; // We didn't find any query, just return
    }
    else { // We found a query, now we have to extract the record

        //Trim any white spaces from the start and end of the query.
        QueryString = QueryString.Trim(false);
        QueryString = QueryString.Trim(true);
        //Now we have the query, we can remove it from the sDocumentString
        sDocument = sDocument.Left(iStartPosition) << sDocument.Right(iLength-iEndPosition);

        iStartPosition = 0;
        iEndPosition =0;
        iLength = sDocument.Length();
        wxString sRecordSet;

        FindBetweenTags(sRecordSet, sDocument,iStartPosition,iEndPosition,"<recordset>");

        if(sRecordSet.IsEmpty()){
            //At this point, we have a query and a string that contains fields that need to be replaced with database values.
            return sDocument;
        }
        else {

            //Now we have a recordset string, we can remove it from the sDocument, it will be replaced multiple times with fields replaced with field values
            wxString beforeRecordset = sDocument.Left(iStartPosition);
            wxString recordsetReplacement = ReplaceFieldTagsWithValuesFromTable(sRecordSet,QueryString);
            wxString afterRecordset = sDocument.Right(iLength-iEndPosition);
            sDocument = beforeRecordset + recordsetReplacement + afterRecordset;
        }
    }
    return sDocument;
}

bool Utility::FindBetweenTags(wxString& ReturnString, wxString& sStringToSearch, int &iStartPosition, int &iEndPosition, wxString tag)
{
    //First generate the end tag
    int tagLength = tag.Length();
    wxString endTag = "</" + tag.Mid(1,tagLength-1);

    int iStart=wxNOT_FOUND;
    int iEnd=wxNOT_FOUND;

    wxString RecordSetString = "";

    iStart = sStringToSearch.Find(tag);
    if(iStart!=wxNOT_FOUND){

        iEnd = sStringToSearch.Find(endTag);
        if(iEnd!=wxNOT_FOUND){

            //We now have a query
            //<query>select * from blah</query>
            //Remove the query tags and return the query.
            RecordSetString = sStringToSearch.Mid(iStart+tagLength,iEnd-(iStart+tagLength));

            iEnd += tagLength+1;
            iStartPosition = iStart;
            iEndPosition = iEnd;

        } else
            return false;
    }
    else
        return false;

    ReturnString = RecordSetString;
    return true;
}


wxString Utility::ReplaceFieldTagsWithValuesFromTable(wxString Recordset, wxString QueryString)
{
    wxString result="";

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            wxString RecordSetTemp = Recordset;

            //NOTE: You should get only one row of data.
            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                //Reset the record set
                RecordSetTemp = Recordset;
                try {

                    //OK, we have to loop search our recordset and extract all the values between the field tables <field>value</field>, then replace them with the value from the database.
                    int iStartPosition=0;
                    int iEndPosition =0;
                    int iLength = 0;

                    iLength = RecordSetTemp.Length();
                    wxString FieldName="";

                   while (FindBetweenTags(FieldName,RecordSetTemp,iStartPosition,iEndPosition,"<field>")) {

                       FieldName.Trim(false);
                       FieldName.Trim(true);
                       wxString strData1(res[currentRow][FieldName], wxConvUTF8);

                       RecordSetTemp = RecordSetTemp.Left(iStartPosition) << strData1 << RecordSetTemp.Right(iLength-iEndPosition);
                       iStartPosition=0;
                       iEndPosition =0;
                       iLength = RecordSetTemp.Length();

                   }

                   result += RecordSetTemp; //Join up all the record sets.
                }
                catch (int& num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }


    return result;
}



bool Utility::LoadBitmap(wxBitmap &image, wxString ImageFileName)
{
    wxString strExe = wxStandardPaths::Get().GetExecutablePath(); // Get the path to the images
    strExe.Replace("DBWorks", "images/"+ImageFileName); //For mac and linux
    strExe.Replace("dbworks", "images/"+ImageFileName); //For mac and linux
    strExe.Replace("dbworks.exe", "images/"+ImageFileName); // For windows.
    //  strExe.Replace("view.png", "help.png"); //For mac and linux
    return image.LoadFile(strExe, wxBITMAP_TYPE_PNG);
}

wxString Utility::Escape(wxString & str)
{
    wxString strToRetur;
    strToRetur = str;
    strToRetur.Replace('"','\"',true);
    strToRetur.Replace("'","\\'",true);

    return strToRetur;
}

// System database developers can do everything.
bool Utility::IsSystemDatabaseDeveloper()
{
    if(Settings.sUsergroup=="SYSTEM_DATABASE_DEVELOPER")
        return true;

    return false;
}

// System Administators can add, edit and delete forms as well as manage users for the database
bool Utility::IsSystemDatabaseAdministrator()
{
    if(Settings.sUsergroup=="SYSTEM_DATABASE_ADMINISTRATOR")
        return true;

    return false;
}
// Advanced users can add, edit and delete forms.
bool Utility::IsAdvancedUser()
{
    if(Settings.sUsergroup=="ADVANCED_USER")
        return true;

    return false;
}
// Standard users can add and edit field, but not delete them.
bool Utility::IsStandardUser()
{
    if(Settings.sUsergroup=="STANDARD_USER")
        return true;

    return false;
}

// A guest can only view fields
bool Utility::IsGuest()
{
    if(Settings.sUsergroup=="GUEST")
        return true;

    return false;
}

// A guest can only view fields
bool Utility::IsCustomUser()
{
    if(Settings.sUsergroup=="CUSTOM_USER")
        return true;

    return false;
}

// This function extra's SELECTION item having the format SELECTION{item1;,item2;,etc;}
//IMPORTANT NOTE: sToSearch might contain "SELECTION{value1;value2;value3}" OR LOOKUP_SELECTION{TableID;ColumnNumber;}, these need to be extracted as a single item, not multiple items.
//NOTE this only processes one line of text, there should be no returns in the passing string.
void Utility::ExtractSelectionItems(wxArrayString &sArray, const wxString& sToSearch)
{
  wxString found;
  int position=0;
  int countSelection=0;
  int countLookupSelection=0;
    position = sToSearch.Find("SELECTION");

    if (position != wxNOT_FOUND){
        //We found the term SELECTION at position;
        position = sToSearch.find("{",position);
        position += 1;

        while(position != wxNOT_FOUND){
            //position now contains the first of our
            int endofdata=0;
            endofdata= sToSearch.find(";",position+1);

            if(endofdata == wxNOT_FOUND)
                break;

            wxString value = sToSearch.SubString(position,endofdata-1); //We don't want to include the ; char as part of the value

            //Account for one of our selections being SELECTION{val1;val2;etc;}
            if(value == "SELECTION{val1")
                countSelection++;
            else if(value == "val2")
                countSelection++;
            else if(value == "etc")
                countSelection++;

            if(countSelection==3) {
                countSelection=0;
                sArray.Add("SELECTION{val1;val2;etc;}");//Add value to the array.
                position = endofdata + 1;
                continue;
            }

            //Account for one of our selections being LOOKUP_SELECTION{TableID;ColumnNumber;}
            if(value == "LOOKUP_SELECTION{TableID")
                countLookupSelection++;
            else if(value == "ColumnNumber")
                countLookupSelection++;

            if(countLookupSelection==2) {
                countLookupSelection=0;
                sArray.Add("LOOKUP_SELECTION{TableID;ColumnNumber;}");//Add value to the array.
                position = endofdata + 1;
                continue;
            }

            if(countSelection==0 && countLookupSelection==0 && value!="}")
                sArray.Add(value);//Add value to the array.

            position = endofdata + 1;

        }
    }
}

//This function will look at the sFlag, it will contain the value SELECTION_LOOKUP{tableID;ColumnNumber;}
// Once we extract the tableID and ColumnNumber, we load the array with values having the columnNumber
void Utility::ExtractSelectionLookupItems(wxArrayString &sArray, wxString sFlag) {

    //We can extract the tableID and ColumnNumber using Utility::ExtractSelection if we change SELECTION_LOOKUP to SELECTION
    wxArrayString flagContents;

    sFlag.Replace( "SELECTION_LOOKUP", "SELECTION");
    ExtractSelectionItems(flagContents,sFlag);

    if(flagContents.GetCount()>0){
        long TableId;
        flagContents[0].ToLong(&TableId);

        long ColumnNumber;
        flagContents[1].ToLong(&ColumnNumber);
        LoadStringArrayFromDatabaseTable(sArray,TableId,ColumnNumber);
    }
}



// Check to see if we have a read only flag set but make sure it's not in the following selection line;
//SELECTION{MULTILINE;HIDE;READONLY;SELECTION{val1;val2;etc;}}
bool Utility::HasFlagReadOnly(const wxString& flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);
    if (findSelection != wxNOT_FOUND)  {
        return false;
    }

    int find = flag.Find("READONLY");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;

}

// Check to see if we have a read only flag set but make sure it's not in the following selection line;
//SELECTION{MULTILINE;HIDE;READONLY;SELECTION{val1;val2;etc;}}
bool Utility::HasFlagMultiline(wxString flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
    if (findSelection != wxNOT_FOUND)  {
        return false;
    }

    int find = flag.Find("MULTILINE");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;

}

// Check to see if we have a read only flag set but make sure it's not in the following selection line;
//SELECTION{MULTILINE;HIDE;READONLY;SELECTION{val1;val2;etc;}}
bool Utility::HasFlagPassword(wxString flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
    if (findSelection != wxNOT_FOUND)  {
        return false;
    }

    int find = flag.Find("PASSWORD");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;

}

bool Utility::HasFlagWeblink(wxString flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
    if (findSelection != wxNOT_FOUND)  {
        return false;
    }

    int find = flag.Find("WEBLINK");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;

}

// Check to see if we have a read only flag set but make sure it's not in the following selection line;
//SELECTION{MULTILINE;HIDE;READONLY;SELECTION{val1;val2;etc;}}
bool Utility::HasFlagSelection(wxString flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
    if (findSelection != wxNOT_FOUND)  {
        return true; //NOTE This returns true because PropertyTable-Edit-flag control is a selection and needs to display all the options after SELECTION{
        //It just happens to be that SELECTION is one of the options, yes a bit of a head spin. This might cause some stupid bug or it might be fine, not sure until testing.
    }

    int find = flag.Find("SELECTION");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;
}

bool Utility::HasFlagSelectionAdditive(wxString flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
    if (findSelection != wxNOT_FOUND)  {
        return true; //NOTE This returns true because PropertyTable-Edit-flag control is a selection and needs to display all the options after SELECTION{
        //It just happens to be that SELECTION is one of the options, yes a bit of a head spin. This might cause some stupid bug or it might be fine, not sure until testing.
    }

    int find = flag.Find("SELECTION_ADDITIVE");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;
}

// Check to see if we have a read only flag set but make sure it's not in the following selection line;
//SELECTION{MULTILINE;HIDE;READONLY;SELECTION{val1;val2;etc;}}
bool Utility::HasFlagLookupSelection(wxString flag)
{
    int findSelection = flag.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
    if (findSelection != wxNOT_FOUND)  {
        return true; //NOTE This returns true because PropertyTable-Edit-flag control is a selection and needs to display all the options after SELECTION{
        //It just happens to be that SELECTION is one of the options, yes a bit of a head spin. This might cause some stupid bug or it might be fine, not sure until testing.
    }

    int find = flag.Find("LOOKUP_SELECTION");

    if (find != wxNOT_FOUND)  {
        return true;
    }

    return false;
}

bool Utility::IsEmpty(wxString str){

    wxString val = str;
    val.Trim(); // The string might have a field val of " ". This is because of a stupid grid bug  issue BUG:00001 , so we need to account for this.
    int length = val.Length();
    if(length==0)
        return true;

    return false;
}

wxString Utility::ReplaceStringWithAsterix(wxString str){
    wxString val = str;
    val.Trim(); // The string might have a field val of " ". This is because of a stupid grid bug  issue BUG:00001 , so we need to account for this.
    int length = val.Length();
    val = "";

    for (int i=0;i<length;i++)
        val += "*";

    return val;
}

// DATE TIME FUNCTIONS

wxString Utility::DateToString(const wxDateTime & dt)
{
    /* THERE IS A wxWidgets Bug that gives you the wrong month.
    //THIS IS SOMETHING WRONG WITH THIS FUNCTION.
    wxString str;
    str <<  dt.GetYear() << "-" << dt.GetMonth() << "-" << dt.GetDay();
    return str;
*/

    wxString s = "";
    s << dt.FormatDate(); //This function works fine, but I need to reverse the format.

    //FORMAT 15/06/2020
    // We need to turn this into 2020-06-15

    wxString str;
    str <<  s.Mid(6,4) <<"-"<<s.Mid(3,2) << "-" << s.Mid(0,2);

    return str;
}

wxDateTime Utility::StringToDate(const wxString & str)
{
    //This is going to work properly.
    wxDateTime dt;
    dt.ParseDate(str);

    return dt;
}

wxString Utility::DateNow() {
    wxDateTime dt = wxDateTime::Now();
    wxString str;
    str <<  dt.GetYear() << "-" << dt.GetMonth() << "-" << dt.GetDay();
    return str;
}
//DATABASE UTILITY FUNCTIONS:
bool Utility::DoesTableExist(wxString sDatabase, wxString sTable)
{

    wxString QueryString;
    QueryString = "SELECT * FROM information_schema.tables WHERE table_schema = '" + sDatabase + "' AND table_name = '" +sTable + "' LIMIT 1";


    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {
            if( res.num_rows() > 0 )
                return true;
            else
                return false;
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }

    return false;
}

bool Utility::DoesDatabaseExist(wxString sDatabase)
{
    wxString QueryString;
    QueryString = "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = '"+sDatabase+"'";


    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {
            if( res.num_rows() > 0 )
                return true;
            else
                return false;
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }

    return false;
}

wxString Utility::GetTableNameFromSYS_TABLES(long lTableId)
{

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");
        wxString QueryString;
        QueryString << "select tablename from " << SYS_TABLES << " where sys_tablesId = ";
        QueryString << lTableId;

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            //NOTE: You should get only one row of data.
            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData1(res[currentRow]["tablename"], wxConvUTF8);
                    return strData1;
                }
                catch (int& num) {
                    wxLogMessage("UTILITY.CPP:GetTableNameFromSYS_TABLES: Row doesn't exist:");

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }
    return "";
}

wxString Utility::GetTableFieldNameFromTable(wxString sTableName, long lColumnNumber)
{
    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");
        wxString QueryString="";
        QueryString = "SELECT COLUMN_NAME, ORDINAL_POSITION FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = ";
        QueryString << "'" << sTableName << "' " << "ORDER BY ORDINAL_POSITION ASC";

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    if(currentRow==lColumnNumber-1){
                        wxString strData(res[currentRow]["COLUMN_NAME"], wxConvUTF8);
                        return strData;
                    }
                }
                catch (int& num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }
    return "";
}


//First we need to find the tablename from the SYS_TABLES table having lTableId.
//Then we need to find the field name correcsponding with lcolumnNumber.
// Once we have the tablename and field name, we can load the table.
void Utility::LoadStringArrayFromDatabaseTable(wxArrayString &sArray, long lTableId, long lColumnNumber){


    wxString sTableName = GetTableNameFromSYS_TABLES(lTableId);
    wxString sFieldName = GetTableFieldNameFromTable(sTableName,lColumnNumber);


    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");
        wxString QueryString = "select " + sFieldName + " from " + sTableName;
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow][sFieldName], wxConvUTF8);
                    sArray.Add(strData);
                }
                catch (int& num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }

}
// Gets a list of all the fields from a given table from the sys_fields table given a table ID.
bool Utility::GetFieldList(wxArrayString &fieldList, wxString TableId)
{
    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    bool bFoundRecord=false;
    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");
        wxString QueryString =   "";
        QueryString << "SELECT valfield FROM  sys_fields where sys_tablesId=" << TableId;
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();


        // Display results
        if (res) {


            // Get each row in result set, and print its contents
            for (size_t i = 0; i < res.num_rows(); ++i) {

                try {
                    //Add a new row to the grid control.
                    mysqlpp::Row row = res[i];

                    wxString field(row[0], wxConvUTF8);
                    fieldList.Add(field);
                    bFoundRecord = true;

                }
                catch (int num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);
                    return false;

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
            return false;
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        return false;
    }

    return bFoundRecord;

}


//Test to see if this field definition exists in our table
bool Utility::DoesFieldExitInTable(const wxString& sTableName, const wxString& sFieldName){

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");

        Query query = conn.query("SELECT COLUMN_NAME FROM information_schema.columns WHERE table_schema='"+database+"' AND table_name='"+sTableName+"'; ");
        StoreQueryResult res = query.store();


        // Display results
        if (res) {


            // Get each row in result set, and print its contents
            for (size_t i = 0; i < res.num_rows(); ++i) {

                try {
                    //Add a new row to the grid control.
                    mysqlpp::Row row = res[i];

                    wxString field(row[0], wxConvUTF8);
                    //We have the field name;
                    if(field == sFieldName)
                        return true;


                }
                catch (int num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);
                    return false;

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
            return false;
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        return false;
    }

    return false;
}

bool Utility::DoesTableExist(wxString sTableName)
{
    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    wxString queryString = "select * from " + sTableName +";";
    Connection  conn;
    try{

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {


            Query query = conn.query(queryString);
            StoreQueryResult res = query.store();

            // Display results
            if (res)
                return true;
            else
                return false;

        }
        //else SetStatusText("Did not connect to database.");

    }catch (BadQuery& er) { // handle any connection or
        //wxMessageBox( "Error: "+ wxString(er.what()));
    } catch (const BadConversion& er) {
        // Handle bad conversions
        //wxMessageBox( "Error: "+ wxString(er.what()));
    } catch (const Exception& er) {
        //wxMessageBox( "Error: "+ wxString(er.what()));
    }

    return false;
}


wxString Utility::LoadSystemDocument(int documentId)
{
    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");
        wxString QueryString;
        QueryString << "select Document from " << SYS_DOCS << " where sys_docsId = ";
        QueryString << documentId;

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            //NOTE: You should get only one row of data.
            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData1(res[currentRow]["Document"], wxConvUTF8);
                    return strData1;
                }
                catch (int& num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);

                }
            }
        }
        else {
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }
    return "";

}



void Utility::ExecuteQuery(const wxString& QueryString, const wxString& sDatabase)
{
    wxString database(sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    try{
        // Connect to the sample database.
        Connection conn(false);

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {


            Query query = conn.query(QueryString);
            query.execute();

        }
        //else SetStatusText("Did not connect to database.");

    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
        wxMessageBox( "Error: "+ wxString(er.what()));
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const BadConversion& er) {
        // Handle bad conversions
        wxMessageBox( "Error: "+ wxString(er.what()));
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const Exception& er) {
        wxMessageBox( "Error: "+ wxString(er.what()));
        // Catch-all for any other MySQL++ exceptions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    }
}

void Utility::ExecuteQuery(const wxString& QueryString)
{
    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    try{
        // Connect to the sample database.
        Connection conn(false);

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {


            Query query = conn.query(QueryString);
            query.execute();

        }
        //else SetStatusText("Did not connect to database.");

    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
        wxMessageBox( "Error: "+ wxString(er.what()));
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const BadConversion& er) {
        // Handle bad conversions
        wxMessageBox( "Error: "+ wxString(er.what()));
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const Exception& er) {
        wxMessageBox( "Error: "+ wxString(er.what()));
        // Catch-all for any other MySQL++ exceptions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    }
}
