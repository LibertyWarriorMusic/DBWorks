//
// Created by Nicholas Zounis on 14/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include <wx/stdpaths.h>
#include "global.h"
#include "CustomControls/myProgressControl.h"

#include <mysql.h>
#include <mysql++.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-escape-sequence"
using namespace mysqlpp;

#include "Utility.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayTableFields);

bool Utility::IsPointInRect(wxPoint pt, wxRect rect) // returns true if the point lies inside the rectangle.
{
    bool bXInRect = false;
    bool bYInRect = false;

    if(pt.x >= rect.x && pt.x <= (rect.x+rect.width))
        bXInRect = true;

    if(pt.y >= rect.y && pt.y <= (rect.y+rect.height))
        bYInRect = true;

    if(bXInRect && bYInRect)
        return true;

    return false;
}

// When you click inside a rect with the mouse, the pt offset from the top left corner of the rect is calculated.
wxSize Utility::CalcPtInRectOffset(wxPoint pt, wxRect rect)
{
    wxSize size;

    size.x=0;
    size.y=0;

    if(IsPointInRect(pt,rect)){
        size.x = pt.x - rect.x;
        size.y = pt.y - rect.y;
    }

    return size;
}
// Will return the pt at one of the corners of the rect that the point is closest too.
wxPoint Utility::CalPtSnapToClosestCornerInRect(wxPoint pt, wxRect rect)
{

    if(pt.y<=0)
        pt.y=0;

    if(pt.x<0)
        pt.x=0;

    wxPoint ptToReturn;
    ptToReturn.x=0;
    ptToReturn.y=0;
    //Approach, divide the rect into four rects and do a hit test on all of them, then move to the closet corner

    wxRect TopLeftRect;
    TopLeftRect.x = rect.x;
    TopLeftRect.y = rect.y;
    TopLeftRect.width =  rect.width/2;
    TopLeftRect.height = rect.height/2;


    wxRect TopRightRect;
    TopRightRect.x = rect.x + rect.width/2;
    TopRightRect.y = rect.y;
    TopRightRect.width =  rect.width/2;
    TopRightRect.height = rect.height/2;


    wxRect BottomLeftRect;
    BottomLeftRect.x = rect.x;
    BottomLeftRect.y = rect.y+rect.height/2;
    BottomLeftRect.width =  rect.width/2;
    BottomLeftRect.height = rect.height/2;


    wxRect BottomRightRect;
    BottomRightRect.x = rect.x + rect.width/2;
    BottomRightRect.y = rect.y + rect.height/2;
    BottomRightRect.width =  rect.width/2;
    BottomRightRect.height = rect.height/2;

    if(IsPointInRect(pt,TopLeftRect)){
        ptToReturn.x = rect.x;
        ptToReturn.y = rect.y;
    }
    else if(IsPointInRect(pt,TopRightRect)) {
        ptToReturn.x = rect.x+rect.width;
        ptToReturn.y = rect.y;
    }
    else if(IsPointInRect(pt,BottomLeftRect)) {
        ptToReturn.x = rect.x;
        ptToReturn.y = rect.y + rect.height;
    }
    else if(IsPointInRect(pt,BottomRightRect)) {
        ptToReturn.x = rect.x + rect.width;
        ptToReturn.y = rect.y + rect.height;
    }

    return ptToReturn;

}
// Find the grid rect that the point lies. The grid is fixed on the screen.
wxRect Utility::CalcGridRectThatPtLiesInside(wxPoint pt,wxSize gridSize)
{
    int XPos = pt.x / gridSize.x;
    int YPos = pt.y / gridSize.y;

    wxRect rectToReturn;

    rectToReturn.x = XPos * gridSize.x;
    rectToReturn.y = YPos * gridSize.y;

    rectToReturn.width = gridSize.x;
    rectToReturn.height = gridSize.y;


    return rectToReturn;

}


int Utility::StringToInt(const wxString& stringToConvert)
{
 //   if(stringToConvert.IsEmpty())
     //   return 0;


    long value;
    if(!stringToConvert.ToLong(&value)) {
        wxLogMessage("Error converting string to int");
    }
    return (int) value;
}
long Utility::StringToLong(const wxString& stringToConvert)
{
    long value;
    if(!stringToConvert.ToLong(&value)) {
        wxLogMessage("Error converting string to int");
    }
    return  value;
}

wxString Utility::IntToString(const int & num)
{
    wxString sNum;
    sNum << num;
    return sNum;
}
bool Utility::CreateSystemTables(wxString sDatabase)
{
    //Check to see if the database was created.
    if(DoesDatabaseExist(sDatabase)){


        //Check to see if the system tables exist, of not, create them.
        if(!DoesTableExist(sDatabase,"sys_tables")){


            //We can load from a file or write in code here. I think it's better to write it code. The other option is the place this in sys_docs table as a system document type.
            wxString query="";

            //OPTION 1 LOAD FROM FILE
            //wxFile file("SQL/createsystables.sql");
            //file.ReadAll(&query,wxConvUTF8);

            //Option 2 Load from table sys_docs.
            //query = Utility::LoadSystemDocument(Settings.iSysTablesDocID);


            //Option 3 DIRECTLY IN CODE. I think this is the best
            query = "CREATE TABLE `sys_tables` ("
                    "  `sys_tablesId` int NOT NULL AUTO_INCREMENT,"
                    "  `title` varchar(100) NOT NULL,"
                    "  `tablename` varchar(100) NOT NULL,"
                    "  `tabletype` varchar(255) NOT NULL,"
                    "  `comments` varchar(255),"
                    "  `table_data` text ,"
                    "  PRIMARY KEY (`sys_tablesId`)"
                    ") ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=utf8;";


            if(!query.IsEmpty()){

                ExecuteQuery(sDatabase,query);
            }

        }
        if(!DoesTableExist(sDatabase,"sys_fields")){

            //We can load from a file or write in code here. I think it's better to write it code or have it in the sys_docs, much better I think.
            wxString query="";
            //wxFile file("SQL/createsysfields.sql");
            //file.ReadAll(&query,wxConvUTF8);


            //Option 2 Load from table sys_docs.
            //query = Utility::LoadSystemDocument(Settings.iSysFieldsDocID);

            //Option 3 DIRECTLY IN CODE. I think this is the best
            query = "CREATE TABLE `sys_fields` ("
                    "`sys_fieldsId` int NOT NULL AUTO_INCREMENT,"
                    " `sys_tablesId` int NOT NULL,"
                    " `valfield` varchar(100) NOT NULL,"
                    " `valtype` varchar(100) NOT NULL,"
                    " `valnull` varchar(10) NOT NULL,"
                    " `valkey` varchar(10) DEFAULT NULL,"
                    " `valdefault` varchar(255) DEFAULT NULL,"
                    "`valextra` varchar(255) DEFAULT NULL,"
                    " `title` varchar(100) DEFAULT NULL,"
                    " `flags` varchar(200) DEFAULT NULL,"
                    "PRIMARY KEY (`sys_fieldsId`)"
                    ") ENGINE=InnoDB AUTO_INCREMENT=50 DEFAULT CHARSET=utf8;";

            if(!query.IsEmpty())
                ExecuteQuery(sDatabase,query);


        }

        if(!DoesTableExist(sDatabase,"sys_diagrams")){

            //We can load from a file or write in code here. I think it's better to write it code or have it in the sys_docs, much better I think.
            wxString query="";
            //wxFile file("SQL/createsysfields.sql");
            //file.ReadAll(&query,wxConvUTF8);


            //Option 2 Load from table sys_docs.
            //query = Utility::LoadSystemDocument(Settings.iSysFieldsDocID);

            //Option 3 DIRECTLY IN CODE. I think this is the best
            query = "CREATE TABLE `sys_diagrams` ("
                    "`sys_diagramsId` int NOT NULL AUTO_INCREMENT,"
                    " `name` varchar(255) NOT NULL,"
                    " `xPosition` int NOT NULL,"
                    " `yPosition` int NOT NULL,"
                    " `width` int NOT NULL,"
                    " `height` int NOT NULL,"
                    " `diagram_data` text,"
                    "PRIMARY KEY (`sys_diagramsId`)"
                    ") ENGINE=InnoDB AUTO_INCREMENT=50 DEFAULT CHARSET=utf8;";

            if(!query.IsEmpty())
                ExecuteQuery(sDatabase,query);


        }

        if(!DoesTableExist(sDatabase,"sys_docs")){

            //We can load from a file or write in code here. I think it's better to write it code or have it in the sys_docs, much better I think.
            wxString query="";

            //Option 1;
            //wxFile file("SQL/createsysdocs.sql");
            //file.ReadAll(&query,wxConvUTF8);

            //Option 2 Load from table sys_docs.
            //query = Utility::LoadSystemDocument(Settings.iSysDocsDocID);

            //Option 3 DIRECTLY IN CODE. I think this is the best
            query = "CREATE TABLE `sys_docs` ("
                    "`sys_docsId` int NOT NULL AUTO_INCREMENT,"
                    "`Title` varchar(200) NOT NULL,"
                    "`TypeOfDoc` varchar(100) NOT NULL DEFAULT 'Help Document',"
                    "`Document` longtext,"
                    "PRIMARY KEY (`sys_docsId`)"
                    " ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;";

            if(!query.IsEmpty())
                ExecuteQuery(sDatabase,query);

        }

        if(!DoesTableExist(sDatabase,"usr_filters")){

            //We can load from a file or write in code here. I think it's better to write it code or have it in the sys_docs, much better I think.
            wxString query="";


            //Option 3 DIRECTLY IN CODE. I think this is the best
            query = "CREATE TABLE `usr_filters` ("
                    "`usr_filtersId` int NOT NULL AUTO_INCREMENT,"
                    "`filterName` varchar(255) NOT NULL,"
                    "`queryDefinition` text NOT NULL,"
                    "`associatedTableId` int,"
                    "`description` text NOT NULL,"
                    "PRIMARY KEY (`usr_filtersId`)"
                    " ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;";

            if(!query.IsEmpty())
                ExecuteQuery(sDatabase,query);

        }

    }
    else{
        return false;
    }

    return true;
}


// Th
void Utility::EscapeAscii(wxString& QueryString)
{
    QueryString.Replace("_", "`~!@%$#^%",true);//I use this crazy squence of characters in the hope that nobody will use this in a query.
    QueryString = QueryString.ToAscii(); //What this is doing is changing the character ‘ to _
    QueryString.Replace("_", "'", true); //The end result is changing ‘ to '
    QueryString.Replace("`~!@%$#^%", "_", true);

}

wxString Utility::Replace(wxString searchString, wxString Old, wxString New, bool all)
{

    int strLen = searchString.Length();
    int iLen = Old.Length();
    wxString result;

    int pos = searchString.Find(Old);
    while(pos!=wxNOT_FOUND){
        result += searchString.Left(pos) << New << searchString.Right(strLen - pos -iLen);
        searchString = result;

        pos = searchString.Find(Old);

        //int d;
        //d=1;

    }

    return result;
}
bool Utility::DoesSelectionExistInCombobox(wxComboBox *combo,   wxString sSelectionLabel)
{
    for (int i =0 ; i< combo->GetCount() ;i++){

      if(combo->GetString(i)==sSelectionLabel)
        return true;
    }
    return false;
}


double Utility::CalculateProgressStepsforImport(int iCount)
{
    double ProgressStep=0;
   // m_ProgessCount=0;

    if(iCount > 0 && iCount <= 100 ){
        ProgressStep   = 100 / (iCount);
    }
    else if(iCount > 100)
        ProgressStep   = iCount / 100 ;

    return ProgressStep;
}


wxString Utility::PassHTMLDocument(wxString sDocument)
{

    int iStartPosition=0;
    int iEndPosition =0;
    int iLength = 0;

    iLength = sDocument.Length();
    wxString QueryString = "";

    while(FindBetweenTags(QueryString,sDocument,iStartPosition,iEndPosition,"<query>")) {


        if (QueryString.IsEmpty()) {
            //Ok, we found our first query.
            //Remove the query string

            return sDocument; // We didn't find any query, just return
        } else { // We found a query, now we have to extract the record


            QueryString = QueryString.Trim(false);
            QueryString = QueryString.Trim(true);

            //This is crazy stupid but the method I used to overcome a stupid feature (or bug, can't tell) in a wxTextCtl multiline.
            //The apostrophe character ' gets replaced with this unicode character ‘ .This is very frustrating and a stupid thing to do for the wxTextCtl
            // MySQL doesn't understand this character and breaks. I can't even use the wxString replace because that is buggy and only replaces the first occurance of
            // this character. By converting it to Ascii, it changes it to an underscore.
            // I might loose other characters by converting it ToAscii(), I believe MySQL uses utf8 so we might loose other characters.
            // Because this is only a search query for documents, we should be OK here.
            // The only real limitation here is you are restricted to using Ascii for queries in documents.

            Utility::EscapeAscii(QueryString);


            //Now we have the query, we can remove it from the sDocumentString
            sDocument = sDocument.Left(iStartPosition) << sDocument.Right(iLength - iEndPosition);

            iStartPosition = 0;
            iEndPosition = 0;
            iLength = sDocument.Length();
            wxString sRecordSet;

            FindBetweenTags(sRecordSet, sDocument, iStartPosition, iEndPosition, "<recordset>");

            if (sRecordSet.IsEmpty()) {
                //At this point, we have a query and a string that contains fields that need to be replaced with database values.
                return sDocument;
            } else {

                //Remove the recordset tags


                //Now we have a recordset string, we can remove it from the sDocument, it will be replaced multiple times with fields replaced with field values
                wxString beforeRecordset = sDocument.Left(iStartPosition);
                wxString recordsetReplacement = ReplaceFieldTagsWithValuesFromTable(sRecordSet, QueryString);
                wxString afterRecordset = sDocument.Right(iLength - iEndPosition);
                sDocument = beforeRecordset + recordsetReplacement + afterRecordset;

                //The document length has changed, get the new length for the next search.
                iLength = sDocument.Length();

            }
        }
    }//End While

    Utility::EscapeAscii(sDocument);

    return sDocument;
}

//iStartPosition will return the start position just before the tag <example_tag> and iEndPosition is just after </example_tag>
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

    //This has caused an issue when the Bear tested on his mac.


   // wxString strExe = wxStandardPaths::Get().GetExecutablePath(); // Get the path to the images
    //wxLogMessage(strExe);
 //   strExe.Replace("DBWorks", "images/"+ImageFileName); //For mac and linux
 //   strExe.Replace("dbworks", "images/"+ImageFileName); //For mac and linux
  //  strExe.Replace("dbworks.exe", "images/"+ImageFileName); // For windows.

    wxString strExe = "/Applications/DatabaseWorks/images/"+ImageFileName;
    //wxString strExe = "images/"+ImageFileName;
    //  strExe.Replace("view.png", "help.png"); //For mac and linux

    //wxString strExe = "images/"+ImageFileName;
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
//IMPORTANT NOTE: sToSearch might contain "SELECTION{value1;value2;value3}" OR SELECTION_LOOKUP_ID{TableID;ColumnNumber;}, these need to be extracted as a single item, not multiple items.
//NOTE this only processes one line of text, there should be no returns in the passing string.
void Utility::ExtractSelectionItems(wxArrayString &sArray, const wxString& sToSearch)
{

    if(sToSearch.IsEmpty())
        return;

  wxString found;
  int position=0;
  int countSelection=0;
  int countLookupSelection=0;
  int countLinkedSelection=0;
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

            //Account for one of our selections being SELECTION_LOOKUP_ID{TableID;ColumnNumber;}
            if(value == "SELECTION_LOOKUP_ID{TableID")
                countLookupSelection++;
            else if(value == "ColumnNumber" && countLookupSelection > 0)
                countLookupSelection++;

            if(countLookupSelection==2) {
                countLookupSelection=0;
                sArray.Add("SELECTION_LOOKUP_ID{TableID;ColumnNumber;}");//Add value to the array.
                position = endofdata + 1;
                continue;
            }

            //Account for one of our selections being SELECTION_LOOKUP_NAME{TableName;FieldName;}
            if(value == "SELECTION_LOOKUP_NAME{TableName")
                countLookupSelection++;
            else if(value == "FieldName" && countLookupSelection > 0)
                countLookupSelection++;

            if(countLookupSelection==2) {
                countLookupSelection=0;
                sArray.Add("SELECTION_LOOKUP_NAME{TableName;FieldName;}");//Add value to the array.
                position = endofdata + 1;
                continue;
            }

            //Account for one of our selections being SELECTION_LINKED_ID{TableID;ColumnNumber;}
            if(value == "SELECTION_LINKED_ID{TableID")
                countLinkedSelection++;
            else if(value == "ColumnNumber" && countLinkedSelection>0)
                countLinkedSelection++;

            if(countLinkedSelection==2) {
                countLinkedSelection=0;
                sArray.Add("SELECTION_LINKED_ID{TableID;ColumnNumber;}");//Add value to the array.
                position = endofdata + 1;
                continue;
            }

            //Account for one of our selections being SELECTION_LINKED_NAME{TableID;ColumnNumber;}
            if(value == "SELECTION_LINKED_NAME{TableName")
                countLinkedSelection++;
            else if(value == "FieldName" && countLinkedSelection>0)
                countLinkedSelection++;

            if(countLinkedSelection==2) {
                countLinkedSelection=0;
                sArray.Add("SELECTION_LINKED_NAME{TableName;FieldName;}");//Add value to the array.
                position = endofdata + 1;
                continue;
            }



            if(countSelection==0 && countLookupSelection==0 && value!="}"  && countLinkedSelection==0 )
                sArray.Add(value);//Add value to the array.

            position = endofdata + 1;

        }
    }
}

//This function will look at the sFlag, it will contain the value SELECTION_LOOKUP_ID{tableID;ColumnNumber;}
// Once we extract the tableID and ColumnNumber, we load the array with values having the columnNumber
bool Utility::ExtractSelectionLookupItemsID(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables) {

    if(sFlag.IsEmpty()|| sFlag.Find("SELECTION_LOOKUP_ID")==wxNOT_FOUND)
        return false;

    //We can extract the tableID and ColumnNumber using Utility::ExtractSelection if we change SELECTION_LOOKUP_ID to SELECTION
    wxArrayString flagContents;

    sFlag.Replace( "SELECTION_LOOKUP_ID", "SELECTION");
    ExtractSelectionItems(flagContents,sFlag);

    if(flagContents.GetCount()>0){

        if(bExtractListVariables)
        {
            sArray=flagContents;
            return true;
        }

        long TableId;
        flagContents[0].ToLong(&TableId);

      //  long ColumnNumber;
       // flagContents[1].ToLong(&ColumnNumber);
        LoadStringArrayFromDatabaseTableByID(Settings.sDatabase, sArray,TableId,flagContents[1]);
    }

    return true;
}
bool Utility::ExtractSelectionLookupItemsName(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables) {

    if(sFlag.IsEmpty() || sFlag.Find("SELECTION_LOOKUP_NAME")==wxNOT_FOUND)
        return false;

    //We can extract the tableID and ColumnNumber using Utility::ExtractSelection if we change SELECTION_LOOKUP_ID to SELECTION
    wxArrayString flagContents;

    sFlag.Replace( "SELECTION_LOOKUP_NAME", "SELECTION");
    ExtractSelectionItems(flagContents,sFlag);

    if(flagContents.GetCount()>0){

        if(bExtractListVariables)
        {
            sArray=flagContents;
            return true;
        }

        LoadStringArrayFromDatabaseTableByName(Settings.sDatabase, sArray,flagContents[0],flagContents[1]);
    }
    return true;
}

//This function will look at the sFlag, it will contain the value SELECTION_LINKED_ID{tableID;ColumnNumber;}
// Once we extract the tableID and ColumnNumber, we load the array with values having the columnNumber
bool Utility::ExtractSelectionLinkedItemsID(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables) {

    if(sFlag.IsEmpty() || sFlag.Find("SELECTION_LINKED_ID")==wxNOT_FOUND)
        return false;

    //We can extract the tableID and ColumnNumber using Utility::ExtractSelection if we change SELECTION_LINKED_ID to SELECTION
    wxArrayString flagContents;

    sFlag.Replace( "SELECTION_LINKED_ID", "SELECTION");
    ExtractSelectionItems(flagContents,sFlag);

    if(flagContents.GetCount()>0){

        if(bExtractListVariables)
        {
            sArray=flagContents;
            return true;
        }

        long TableId;
        flagContents[0].ToLong(&TableId);

        //long ColumnNumber;
       // flagContents[1].ToLong(&ColumnNumber);
        LoadStringArrayFromDatabaseTableByID(Settings.sDatabase, sArray,TableId,flagContents[1]);
    }
    return true;
}
bool Utility::ExtractSelectionLinkedItemsName(wxArrayString &sArray, wxString sFlag, bool bExtractListVariables) {

    if(sFlag.IsEmpty()|| sFlag.Find("SELECTION_LINKED_NAME")==wxNOT_FOUND)
        return false;

    //We can extract the tableID and ColumnNumber using Utility::ExtractSelection if we change SELECTION_LINKED_ID to SELECTION
    wxArrayString flagContents;

    sFlag.Replace( "SELECTION_LINKED_NAME", "SELECTION");
    ExtractSelectionItems(flagContents,sFlag);

    wxString tableName = flagContents[0];
    wxString fieldName = flagContents[1];

    if(flagContents.GetCount()>0){

        if(bExtractListVariables)
        {
            sArray=flagContents;
            return true;
        }

        LoadStringArrayFromDatabaseTableByName(Settings.sDatabase, sArray,flagContents[0],flagContents[1]);
    }
    return true;
}

bool Utility::HasFlag(wxString flags, wxString flag)
{

    if (flag=="SELECTION" || flag=="SELECTION_ADDITIVE"){

        // Check to see if we have a read only flag set but make sure it's not in the following selection line;
        //FLAG_OPTIONS see global.h
        int findSelection = flags.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
        if (findSelection != wxNOT_FOUND)  {
            return true;
        }

    } else{

        // Check to see if we have a read only flag set but make sure it's not in the following selection line;
        //FLAG_OPTIONS see global.h
        int findSelection = flags.Find(FLAG_OPTIONS);// NOT the FLAG_OPTIONS is defined in global.h
        if (findSelection != wxNOT_FOUND)  {
            return false;
        }
    }

    int find = flags.Find(flag);

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
/*
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
*/
bool Utility::DoesDatabaseExist(wxString sDatabase)
{
    wxString QueryString;
    QueryString = "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = '"+sDatabase+"'";


    wxString database("INFORMATION_SCHEMA");
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

wxString Utility::GetTableNameFromSYS_TABLESById(wxString sDatabase, wxString sTableId)
{

    wxString database(sDatabase);
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
        QueryString << sTableId;

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


wxString Utility::GetTableTitleFromSYS_TABLESById(wxString sDatabase, wxString sTableId)
{

    if(sTableId.IsEmpty())
        return "";

    wxString database(sDatabase);
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
        QueryString << "select title from " << SYS_TABLES << " where sys_tablesId = ";
        QueryString << sTableId;

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
                    wxString strData1(res[currentRow]["title"], wxConvUTF8);
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

wxString Utility::GetTableIdFromSYS_TABLESByTableName(wxString sDatabase, wxString sTableName)
{

    wxString database(sDatabase);
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
        QueryString << "select sys_tablesId from " << SYS_TABLES << " where tablename= '"+sTableName+"'";

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
                    wxString strData1(res[currentRow]["sys_tablesId"], wxConvUTF8);
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

wxString Utility::GetTableIdFromSYS_TABLESByTitle(wxString sDatabase, wxString sTableTitle)
{

    wxString database(sDatabase);
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
        QueryString << "select sys_tablesId from " << SYS_TABLES << " where title= '"+sTableTitle+"'";

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
                    wxString strData1(res[currentRow]["sys_tablesId"], wxConvUTF8);
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


wxString Utility::GetTableFieldNameFromMySQLInfoSchema(wxString sDatabase, wxString sTableName, wxString sColumnNumber)
{
    wxString database(sDatabase);
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

        long lColumnNumber;
        sColumnNumber.ToLong(&lColumnNumber);

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    if(currentRow==lColumnNumber){
                    //if(currentRow==lColumnNumber-1){//I am not sure why we did this, by this isn't correct.
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
void Utility::LoadStringArrayFromDatabaseTableByID(wxString sDatabase, wxArrayString &sArray, long lTableId, wxString sColumnNumber){

    wxString sTableId;
    sTableId << lTableId;
    wxString sTableName = GetTableNameFromSYS_TABLESById(sDatabase, sTableId);
    wxString sFieldName = GetTableFieldNameFromMySQLInfoSchema(sDatabase, sTableName,sColumnNumber);


    wxString database(sDatabase);
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
void Utility::LoadStringArrayFromDatabaseTableByName(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldName)
{


    wxString database(sDatabase);
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

void Utility::GetTableIDFromTableWhereFieldEquals(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldName, wxString value)
{
    wxString database(sDatabase);
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
        wxString QueryString = "select " + sTableName + "Id from " + sTableName + " WHERE " + sFieldName + "='"+value+"' LIMIT 1";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow][sTableName+"Id"], wxConvUTF8);
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
//First we need to find the tablename from the SYS_TABLES table having lTableId.
//Then we need to find the field name correcsponding with lcolumnNumber.
// Once we have the tablename and field name, we can load the table.
void Utility::LoadStringArrayWithMySQLTableNames(wxString sDatabase, wxArrayString &sArray ){

    wxString database(sDatabase);
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
        wxString QueryString = "SHOW TABLES;";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow]["Tables_in_"+sDatabase], wxConvUTF8);

                    //NOTE We don't want to load system tables, they will be in the database, however we
                    // don't wan't the user to see them in the table definitions.
                    // If we ever create more system tables, we need to reject them here.
                    if(!(strData=="sys_tables" || strData=="sys_fields"))
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


void Utility::LoadStringArrayWidthMySQLDatabaseNames(wxArrayString &sArray){

    wxString database("INFORMATION_SCHEMA"); // This doesn't have to be this database, but we know this database is created in MySQL
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
        wxString QueryString = "SHOW DATABASES;";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow]["Database"], wxConvUTF8);
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

bool Utility::GetFieldFromTableWhereFieldEquals(wxString sDatabase, wxArrayString &sArray, wxString sTableName, wxString sFieldToGet, wxString sFieldName, wxString value)
{
    if(sDatabase.IsEmpty() || sTableName.IsEmpty() || sFieldName.IsEmpty() || value.IsEmpty() ||  value=="NULL") //We need to do this more often.
        return false;

    wxString database(sDatabase);
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
        wxString QueryString = "select " + sFieldToGet + " from " + sTableName + " WHERE " + sFieldName + "='"+value+"' LIMIT 1";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow][sFieldToGet], wxConvUTF8);
                    sArray.Add(strData);
                    return true;
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

    return false;
}



// Gets a list of TableFieldItems given table from the sys_fields table given a table ID.
bool Utility::GetFieldList(ArrayTableFields &fieldList, wxString TableId)
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
        QueryString << "SELECT * FROM  sys_fields where sys_tablesId=" << TableId;
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();


        // Display results
        if (res) {


            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < res.num_rows(); ++currentRow) {

                try {
                    //Add a new row to the grid control.


                    TableFieldItem * fieldItem = new TableFieldItem();

                    wxString valfield(res[currentRow]["valfield"], wxConvUTF8);
                    wxString valtype(res[currentRow]["valtype"], wxConvUTF8);
                    wxString valnull(res[currentRow]["valnull"], wxConvUTF8);
                    wxString valkey(res[currentRow]["valkey"], wxConvUTF8);
                    wxString valdefault(res[currentRow]["valdefault"], wxConvUTF8);
                    wxString valextra(res[currentRow]["valextra"], wxConvUTF8);

                    fieldItem->fieldName=valfield;
                    fieldItem->fieldType=valtype;
                    fieldItem->fieldNull=valnull;
                    fieldItem->fieldKey=valkey;
                    fieldItem->fieldDefault=valdefault;
                    fieldItem->fieldExtra=valextra;

                    fieldList.Add(fieldItem);
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



/* //NOT SURE YET
void Utility::DestroyFieldItemList(ArrayTableFields &fieldList) // Runs through the list and destroys all the items.
{
    for (int index=0; index<fieldList.Count();index++){
        TableFieldItem  item;
        item = fieldList[index];



    }

}*/
//Test to see if this field definition exists in our table: name, type, null, key, default, extra
bool Utility::DoesFieldExitInTable(const wxString& sTableName,  const ArrayTableFields& fieldItemList){

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    //bool bEverythingMatches = true;// We are looking for a false

    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        //SetStatusText("Database Connected");

       // Query query = conn.query("SELECT COLUMN_NAME FROM information_schema.columns WHERE table_schema='"+database+"' AND table_name='"+sTableName+"'; ");

        wxString QueryString = "DESCRIBE "+sTableName+";";
        Query query = conn.query( QueryString);


        StoreQueryResult res = query.store();


        // Display results
        if (res) {

                int RowsInTable = res.num_rows();

                // Get each row in result set, and print its contents
                for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                    if( currentRow > 0 ) {

                        //Get the Field Item
                        TableFieldItem fieldItem = fieldItemList[currentRow-1];


                        //We don't need to test the primary key as it's set automatically and we don't save it in the fields item list.
                        try {
                            wxString sField(res[currentRow]["Field"], wxConvUTF8);
                            wxString sType(res[currentRow]["Type"], wxConvUTF8);
                            wxString sNull(res[currentRow]["Null"], wxConvUTF8);
                            wxString sKey(res[currentRow]["Key"], wxConvUTF8);
                            wxString sDefault(res[currentRow]["Default"], wxConvUTF8);
                            wxString sExtra(res[currentRow]["Extra"], wxConvUTF8);

                            //A default value of NULL means nothing
                            if (sDefault=="NULL")
                                sDefault="";

                            fieldItem.fieldType = fieldItem.fieldType.Lower();
                            sType = sType.Lower();


                            //NOTE: You only need to say NOT NULL in the query, so any YES (can be NULL) is the default
                            // and doesn't need to be in the statement.
                            sNull=sNull.Lower();
                            fieldItem.fieldNull = fieldItem.fieldNull.Lower();


                            if(sNull=="yes")
                                sNull="";

                            if(fieldItem.fieldNull=="yes")
                                fieldItem.fieldNull="";

                            if(sDefault=="NULL")
                                sDefault="";

                            if(fieldItem.fieldDefault=="NULL")
                                fieldItem.fieldDefault="";

                            if (sField != fieldItem.fieldName ||
                                 sType != fieldItem.fieldType ||
                                 sNull != fieldItem.fieldNull ||
                                 sKey != fieldItem.fieldKey ||
                                 sDefault != fieldItem.fieldDefault ||
                                 sExtra != fieldItem.fieldExtra)
                                    return false;
                        }
                        catch (int &num) {
                            wxLogMessage(MSG_DATABASE_FAIL_ROW);

                        }
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

    return true;
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

bool Utility::DoesTableExist(wxString sDatabase, wxString sTableName)
{
    wxString database(sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    wxString queryString = "SELECT * FROM " + sTableName +";";
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



void Utility::ExecuteQuery(const wxString& sDatabase , const wxString& QueryString )
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
void Utility::CreateDatabase(wxString sDatabaseToCreate){

    wxString database("INFORMATION_SCHEMA");
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


            wxString QueryString= "CREATE DATABASE "  + sDatabaseToCreate;
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

// Place all the mySQL reserved where
bool Utility::IsReservedMySQLWord(wxString wordToFind)
{

    wxString reserved = MYSQLRESERVEDWORDS;
    reserved = reserved.Lower();
    wordToFind = wordToFind.Lower();
    if (reserved.Find(wordToFind)!=wxNOT_FOUND)
        return true;

    return false;
}
//First we need to find the tablename from the SYS_TABLES table having lTableId.
//Then we need to find the field name correcsponding with lcolumnNumber.
// Once we have the tablename and field name, we can load the table.
void Utility::LoadStringArrayWithTableIdsFromSysTables(wxString sDatabase, wxArrayString &sArray ){

    wxString database(sDatabase);
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
        wxString QueryString = "select sys_tablesId from sys_tables";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow]["sys_tablesId"], wxConvUTF8);

                    //NOTE We don't want to load system tables, they will be in the database, however we
                    // don't wan't the user to see them in the table definitions.
                    // If we ever create more system tables, we need to reject them here.
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
//First we need to find the tablename from the SYS_TABLES table having lTableId.
//Then we need to find the field name correcsponding with lcolumnNumber.
// Once we have the tablename and field name, we can load the table.
// If HiddenTablesOnly is true, then only tables with having the table_data attribute ObTableShow as no will be listed.
void Utility::LoadStringArrayWithTableNamesFromSysTables(wxString sDatabase, wxArrayString &sArray , bool HiddenTablesOnly ){

    wxString database(sDatabase);
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
        wxString QueryString = "select tablename, table_data from sys_tables";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString strData(res[currentRow]["tablename"], wxConvUTF8);
                    wxString tableData(res[currentRow]["table_data"], wxConvUTF8);

                    wxString sData = tableData;

                    sData=sData.Lower();
                    if(sData=="null")
                        tableData="";

                    //NOTE We don't want to load system tables, they will be in the database, however we
                    // don't wan't the user to see them in the table definitions.
                    // If we ever create more system tables, we need to reject them here.
                    // If no data has been set, we wat to list the table.
                    if(tableData.IsEmpty() || !HiddenTablesOnly){
                        sArray.Add(strData);
                    } else{
                        int pos = tableData.Find("<key:ObTableShow>no</key>");
                        if(pos!=wxNOT_FOUND)
                            sArray.Add(strData);
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

}


// If the table doesn't exist, this will prepare a create string.
wxString Utility::CreateTable(wxString sDatabase, wxString sTableName, ArrayTableFields m_FieldsArray )
{
    wxString queryString="";

    wxString fieldName;
    wxString valtype;
    wxString valnull;
    wxString valkey;
    wxString valdefault;
    wxString valextra;


    queryString = "CREATE TABLE IF NOT EXISTS `"+sDatabase+"`.`"+sTableName+"` (";
    queryString += "`"+sTableName+"Id` INT NOT NULL AUTO_INCREMENT, ";

    for (int row = 0 ; row < m_FieldsArray.GetCount(); row++){

        //Reset
        //buildString="";
        fieldName="";
        valtype="";
        valnull="";
        valkey="";
        valdefault="";
        valextra="";

        queryString += "`";


        fieldName=m_FieldsArray[row].fieldName;
        valtype=m_FieldsArray[row].fieldType;

        wxString val=m_FieldsArray[row].fieldNull;
        if(val.Lower()=="no")
            valnull=" NOT NULL ";
        else
            valnull=" ";
        //valnull=m_FieldsArray[row].fieldNull;

        valkey=m_FieldsArray[row].fieldKey;

        val=m_FieldsArray[row].fieldDefault; //Default needs to be DEFAULT = 'value' if a value exists, otherwise it can be empty.


        if(val.Lower()=="null" || val == "0")
            valdefault=" ";  //There is stupid bug in the wxWidgets grid that text in a cell draws over the right cell it that cel is empty.
        else if (Utility::IsEmpty(val))
            valdefault="";
        else
            valdefault= " DEFAULT '" + val + "'";



        valextra=m_FieldsArray[row].fieldExtra;


        queryString += fieldName + "` " + valtype + valnull + " " + valkey + " " + valdefault + " " + valextra + ",";
        //Update the string
    }

    queryString += "PRIMARY KEY (`"+sTableName+"Id`));";

    ExecuteQuery(sDatabase,queryString);

    return Utility::GetTableIdFromSYS_TABLESByTableName(sDatabase,sTableName);
}

//Will need to describe the table and fill the m_fields
// If the table was created by DBWorks and it has a primary key PRI and the primary key name is the tablenameId, then don't create the field.
// Also, we don't want to create primary keys because the dbworks automatically creates them.
// We might want a future option to ask the user to include primary keys fields as none primary keys.
bool Utility::LoadFieldArrayWithTableFields(wxString sDatabase, wxString sTableName, ArrayTableFields &m_Fields )
{
    wxString database(sDatabase);
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
        wxString QueryString = "DESCRIBE " + sTableName + ";";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        //First we need to find the table name
        // Display results
        if (res) {

            int RowsInTable = res.num_rows();

            // Get each row in result set, and print its contents
            for (size_t currentRow = 0; currentRow < RowsInTable; ++currentRow) {

                try {
                    wxString sField(res[currentRow]["Field"], wxConvUTF8);
                    wxString sType(res[currentRow]["Type"], wxConvUTF8);
                    wxString sNull(res[currentRow]["Null"], wxConvUTF8);
                    wxString sKey(res[currentRow]["Key"], wxConvUTF8);
                    wxString sDefault(res[currentRow]["Default"], wxConvUTF8);
                    wxString sExtra(res[currentRow]["Extra"], wxConvUTF8);

                    //If you are importing a databaseworks generated table, you don't want to import the primary key of that that because it automatically gets created
                    // and you don't want duplicate PRI keys.
                    if(sField !=sTableName+"Id"){
                        auto *tableField = new TableFieldItem();
                        if(sKey=="PRI"){
                            //If we have a PRI key, then don't create it because the database creates it's own PRI
                            //Check the option to create a primary key but never create one if dbworks created it, that will be tablenameId

                            if(Settings.bImportPrimaryKey)
                                tableField->fieldKey=sKey;// Don't include the PRI here
                            else
                                tableField->fieldKey="";

                        } else
                            tableField->fieldKey=sKey;

                        tableField->fieldName=sField;
                        tableField->fieldType=sType;
                        tableField->fieldNull=sNull;
                        tableField->fieldDefault = sDefault;
                        m_Fields.Add(tableField);
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

 return true;
}



wxString Utility::InsertTableInSYS_TABLES(wxString sDatabase, wxString sTableName)
{
   wxString QueryString = "INSERT INTO sys_tables (title,tablename,tabletype,comments) VALUES ('"+sTableName+"','"+sTableName+"','user_imported','')";
   ExecuteQuery(sDatabase,QueryString);
   return GetTableIdFromSYS_TABLESByTableName(sDatabase,sTableName);

}


void Utility::InsertFieldInSYS_FIELDS(wxString sDatabase, wxString sTableId, TableFieldItem fieldItem)
{
    wxString tableID;
    tableID << sTableId;

    wxString QueryString = "INSERT INTO sys_fields (sys_tablesId,valfield,valtype,valnull,valkey,valdefault,valextra,title,flags) VALUES ('"+ tableID +"','"+ fieldItem.fieldName +"','"+ Utility::Escape(fieldItem.fieldType) +"','"+ fieldItem.fieldNull +"','"+ fieldItem.fieldKey +"','"+ Utility::Escape(fieldItem.fieldDefault) +"','"+ fieldItem.fieldExtra +"','"+ fieldItem.fieldName +"','')";
    ExecuteQuery(sDatabase,QueryString);

}
//Loads all the database to the array string from the dbworks database table sys_databases
void Utility::AppendDBWorksDatabases(wxArrayString &arrayToAppend)
{

    wxString database("dbworks");
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    // Connect to the sample database.
    Connection conn(false);

    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        wxString QueryString =   "";
        QueryString << "SELECT databasename FROM  sys_databases";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        // Display results
        if (res) {

            // Get each row in result set, and print its contents
            for (size_t i = 0; i < res.num_rows(); ++i) {

                try {

                    wxString strData1(res[i]["databasename"], wxConvUTF8);
                    arrayToAppend.Add(strData1);

                }
                catch (int num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);

                }
            }
        }
        else {
           // wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);

        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);

    }

}

//Used to save our imported databases
void Utility::SaveDatabaseToDBWorks(wxString sDatabaseNameToSave){

    if(!DoesRecordExist("dbworks","sys_databases","databasename",sDatabaseNameToSave)){
        wxString QueryString = "INSERT INTO sys_databases (databasename,comments) VALUES ('"+sDatabaseNameToSave+"','user_imported')";
        ExecuteQuery("dbworks",QueryString);
    }
}

//Generic function that checks if a record in the database exists.
bool Utility::DoesRecordExist(wxString sDatabase, wxString sTable, wxString sFieldname, wxString sValue)// Check to see if a record with a particular value exists.
{
    wxString QueryString;
    QueryString = "SELECT "+sFieldname+" FROM "+sTable+" WHERE sFieldname = '" + sValue + "' LIMIT 1";


    wxString database(sDatabase);
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
            //wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
    }

    return false;

}
//Load all the queries in sys_filters into a combo box. Select selection has an associated ComboDataItem that stores the query and other information about our filter.
void Utility::LoadComboUsrFilters(wxString sDatabase, wxComboBox &pCombo, wxString associatedTableId)
{
    wxString database(sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    // Connect to the sample database.
    Connection conn(false);

    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        wxString QueryString =   "";
        QueryString << "SELECT * FROM  usr_filters where associatedTableID=" + associatedTableId;
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        // Display results
        if (res) {

            // Get each row in result set, and print its contents
            for (size_t i = 0; i < res.num_rows(); ++i) {

                try {

                    wxString filterName(res[i]["filterName"], wxConvUTF8);
                    wxString queryDefinition(res[i]["queryDefinition"], wxConvUTF8);
                    wxString description(res[i]["description"], wxConvUTF8);

                    ComboDataItem* comboItem = new ComboDataItem;
                    Utility::EscapeAscii(queryDefinition); // Convert from unicode to '
                    comboItem->queryDefinition = queryDefinition;
                    comboItem->linkedTableID = associatedTableId;
                    comboItem->title=filterName;
                    comboItem->description=description;



                    //wxClientData * cd = new wxClientData;

                    pCombo.Append(filterName,(ComboDataItem*)comboItem);

                    //arrayToAppend.Add(strData1);

                }
                catch (int num) {
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);

                }
            }
        }
        else {
            // wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);

        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);

    }
}

//Used to destroy all the objects in the combo box because
void Utility::DestroyComboDataObjects(wxComboBox *pCombo)
{
    int len = pCombo->GetCount();

    for(int index=0;index<len;index++){

        ComboDataItem* comboItem = (ComboDataItem*) pCombo->GetClientData(index);
        delete comboItem;
    }
}
// Get table can store system data of any type, this is the generic place to put data.
// You need to define a unique key for the data and supply the table ID where the data is stored.
// that data is stored in the table_data field in sys_tables; The format is as follows
// NOTE: This function can be used to search for the data also, it will return true if data is found and false if no data found.
// <key:keyname>the data to store</key>
bool Utility::LoadTableData(wxString sDatabase, wxString sTableName, wxString KeyName, wxString &sData)
{

    //STEP 1: Get the data from the sys_tables field.
    wxString    tableData = GetTableDataString(sDatabase,Utility::GetTableIdFromSYS_TABLESByTableName(sDatabase,sTableName));



    //STEP 2: Search for <key:KeyName> to find the position

    wxString keyToSearch= "<key:"+KeyName+">";
    int lenKey=keyToSearch.Length();
    int keyPos = tableData.Find(keyToSearch);

    if(keyPos == wxNOT_FOUND){
        return false;
    }
    else{

        int startPosOfData = keyPos + lenKey;

        //STEP 3: Search for</key>
        int endPosOfData=tableData.find("</key>",startPosOfData);;


        //STEP 4: Extract the data
        sData = tableData.Mid(startPosOfData,endPosOfData-startPosOfData);
        return true;
    }
    return true;
}




bool Utility::LoadTableData(wxString sDatabase, wxString sTableName, wxString KeyName, wxString &sData, wxString &entireDataString, int &startPosOfData)
{
    //STEP 1: Get the data from the sys_tables field.

    entireDataString = GetTableDataString(sDatabase,Utility::GetTableIdFromSYS_TABLESByTableName(sDatabase,sTableName));

    //STEP 2: Search for <key:KeyName> to find the position

    wxString keyToSearch= "<key:"+KeyName+">";
    int lenKey=keyToSearch.Length();
    int keyPos = entireDataString.Find(keyToSearch);

    if(keyPos == wxNOT_FOUND){
        return false;
    }
    else{

        startPosOfData = keyPos + lenKey;

        //STEP 3: Search for</key>
        int endPosOfData=entireDataString.find("</key>",startPosOfData);;


        //STEP 4: Extract the data
        sData = entireDataString.Mid(startPosOfData,endPosOfData-startPosOfData);
        return true;
    }
    return true;

}


void Utility::SaveTableData(const wxString& sDatabase, const wxString& sTableName, const wxString& KeyName, wxString data)
{
    //STEP 1 Call LoadTableData to see if data exists and get the entire data string from the sys_tables.
    wxString entireDataString="";
    int startPosOfData=0;
    wxString newData = data;
    if(Utility::LoadTableData(sDatabase, sTableName, KeyName, data,entireDataString, startPosOfData)){
        //Amend the data string
        wxString entireDataStringToWriteBack="";
        int LenEntireString = entireDataString.Length();

        //STEP 3: Search for</key>
        int endPosOfData=entireDataString.find("</key>",startPosOfData);;

        //STEP 4: Replace the data in the entire data string so we can write it back to the database

        entireDataStringToWriteBack = entireDataString.Left(startPosOfData) + newData + entireDataString.Right(LenEntireString-endPosOfData);

        wxString table = "sys_tables";
        wxString field = "table_data";

        //Update the database table sys_fields
        wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTableName(sDatabase,sTableName);
        UpdateTableFieldById(sDatabase,table,sTableId,field,entireDataStringToWriteBack);
    }
    else{

        //Update the database table sys_fields
        wxString dataToUpdata;
        dataToUpdata = entireDataString+"<key:"+KeyName+">"+newData+"</key>";
        wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTableName(sDatabase,sTableName);
        UpdateTableFieldById(sDatabase,"sys_tables",sTableId,"table_data",dataToUpdata);

    }

    //STEP 2 If it exists, we will amend the string, if not, add the new data to the end of the string.
    //STEP 3 Write the string back to sys_tables.

}

//The key data combination example  <key:keyname1>data1</key><key:keyname2>data2</key><key:keyname3>data3</key>
//Write the data in one hit, but it needs to be stored sequencially.
void Utility::SaveTableDataBulk(wxString sDatabase,wxString sTableName, wxString KeyDataCombination)
{
    //STEP 1 Call GetTableData to see if data exists
    //STEP 2 Get the entire data string from the sys_tables.
    //STEP 3 If it exists, we will amend the string, if not, add the new data to the end of the string.
    //STEP 4 Write the string back to sys_tables.

}

//Get the entire data string
wxString Utility::GetTableDataString(wxString sDatabase, wxString sTableId)
{

    wxString database(sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    // Connect to the sample database.
    Connection conn(false);

    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        wxString QueryString =   "";
        QueryString << "SELECT table_data FROM  sys_tables where sys_tablesId=" + sTableId + " LIMIT 1";
        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        // Display results
        if (res) {

            // Get each row in result set, and print its contents
            for (size_t i = 0; i < res.num_rows(); ++i) {

                try {

                    wxString data(res[i]["table_data"], wxConvUTF8);
                    if(data=="NULL")
                        return "";

                    return data;


                }
                catch (int num) {
                    //wxLogMessage(MSG_DATABASE_FAIL_ROW);
                    return "";

                }
            }
        }
        else {
            // wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
            return "";
        }
    }
    else{
        //wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        return "";
    }
    return "";
}


void Utility::UpdateTableFieldById(wxString sDatabase, wxString sTableName, wxString sTableId, wxString sFieldname, wxString sValue)
{
    wxString QueryString="";
    QueryString="UPDATE "+sTableName+" SET "+sFieldname+" = '"+sValue +"' WHERE "+sTableName+"Id="+ sTableId;
    ExecuteQuery(sDatabase,QueryString);

}


