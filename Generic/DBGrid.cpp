
#include <string.h>

#include<wx/wx.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/frame.h>
#include <wx/filename.h>
#include <wx/dynarray.h>
#include <wx/hyperlink.h>
#include <mysql.h>
#include <mysql++.h>

//Add my own classes here


#include "../Shared/global.h"
#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "DBGrid.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayGridItem);

enum {
    ID_MENU_PROPERTIES = wxID_HIGHEST + 500,
    ID_MENU_OPEN,
    ID_MENU_OPEN_FORM_QUERY,
    ID_MENU_RUN_FILTER,
    ID_MENU_EDIT,
    ID_MENU_FILTER,
    ID_MENU_FILTER_SHOW_ALL,
    ID_MENU_DOCUMENT,
    ID_MENU_DESIGN_FORMS,
    ID_MENU_DESIGN_PAGES,
    ID_MENU_RUN_FORM
};

wxBEGIN_EVENT_TABLE(DBGrid, wxGrid)
    EVT_MENU(ID_MENU_PROPERTIES, DBGrid::OnClickProperties)
    EVT_MENU(ID_MENU_OPEN, DBGrid::OnClickOpen)
    EVT_MENU(ID_MENU_RUN_FILTER, DBGrid::OnRunFilter)
    EVT_MENU(ID_MENU_DOCUMENT, DBGrid::OnClickOpenDocument)
    EVT_MENU(ID_MENU_EDIT, DBGrid::OnClickEdit)
    EVT_MENU(ID_MENU_FILTER, DBGrid::OnClickMenuFilter)
    EVT_MENU(ID_MENU_FILTER_SHOW_ALL, DBGrid::OnClickMenuFilterShowAll)
    EVT_MENU(ID_MENU_OPEN_FORM_QUERY, DBGrid::OnOpenFormQuery)
    EVT_MENU(ID_MENU_DESIGN_FORMS, DBGrid::OnDesignForm)
    EVT_MENU(ID_MENU_RUN_FORM, DBGrid::OnRunForm)
    EVT_MENU(ID_MENU_DESIGN_PAGES, DBGrid::OnDesignPage)
wxEND_EVENT_TABLE()

using namespace mysqlpp;

DBGrid::DBGrid(wxWindow* _parent,wxWindowID _ID,wxPoint _pos,wxSize _size,long _style) : wxGrid( _parent, _ID, _pos, _size, _style )
{
    m_eventType = 0;
    m_iRow = 0;
    m_iCol = 0;
    m_iGetCellRowIndex=0;
    bFormQueryMode=false;
    m_sFormQueryTemp="";
    m_sFormQueryOriginal="";

    // Connect Events
    Connect(wxEVT_SIZE,wxGridEventHandler( DBGrid::OnSizeGridSpreadSheet ), nullptr, this );
    Connect(wxEVT_GRID_SELECT_CELL,wxGridEventHandler( DBGrid::OnGridEvents ), nullptr, this );
    Connect(wxEVT_GRID_CELL_RIGHT_CLICK,wxGridEventHandler( DBGrid::OnGridRClick ), nullptr, this );
    Connect(wxEVT_GRID_CELL_LEFT_DCLICK,wxGridEventHandler( DBGrid::OnGridDLClick ), nullptr, this );
    Connect(wxEVT_GRID_LABEL_LEFT_CLICK,wxGridEventHandler( DBGrid::OnGridLabelRightClick ), nullptr, this );
    //Connect(wxEVT_GRID_CELL_LEFT_CLICK,wxGridEventHandler( DBGrid::OnGridClick ), nullptr, this );

}
 DBGrid::~DBGrid()
{
    Disconnect( wxEVT_SIZE, wxGridEventHandler( DBGrid::OnSizeGridSpreadSheet ), nullptr, this );
    Disconnect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( DBGrid::OnGridEvents ), nullptr, this );
    Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( DBGrid::OnGridRClick ), nullptr, this );
    Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( DBGrid::OnGridDLClick ), nullptr, this );
    //m_GridArray.Empty();
}

void DBGrid::OnSizeGridSpreadSheet( wxGridEvent& event )
{
    //Because we are interupting the base class size event, if we don't call this function to set the widths of the columns, then the cells will not be drawn.

    //Generate the size event for the base class
    wxSizeEvent sizeEvent;
    sizeEvent.SetSize(GetVirtualSize());
    OnSize(sizeEvent);

    ResizeSpreadSheet();
}


//Adjusts the widths of columns to fit the text.
void DBGrid::ResizeSpreadSheet()
{
    // There is a sizing issue if we use this funcition because we are interupting the base class size event.
    //
    wxSize sizeOfFrame= GetVirtualSize();

    int RoomForASC_DESC_Button=30;

    int num_cols=GetNumberCols();
    int width=GetRowLabelSize();
    int frameSizeWidth=sizeOfFrame.GetWidth();
    int CurrentWidth=0;
    int totalGridWidth = num_cols*width;

    // If we can't fit all the columns in the width of the frame by text, then we have to assign a width so each column is visible on the screen.
    if (frameSizeWidth>totalGridWidth){

        for(int col=0;col<num_cols-1;col++)
        {
            bool bHide = false;
            if(col>0){
                bHide = Utility::HasFlag(m_GridArray[col-1].Flags,"HIDE");
            }

            if(col>0 && bHide){

                //Only set it if it's not allready zero, zero width means it's hidden.
                int CurrentWidth = GetColumnWidth(col);
                CurrentWidth+=RoomForASC_DESC_Button; // Add a little for the order buttons
                SetColumnWidth(col,CurrentWidth);

                if (CurrentWidth>0)//Only do unhidden columns, hidden columns have zero width
                    SetColSize(col,0);

            }else{
                //Auto size the column to the width so we can read the correct width value from getColSize()
                AutoSizeColumn(col);

                // Get the column width and make sure it's over 100.
                CurrentWidth=GetColSize(col);
                CurrentWidth+=RoomForASC_DESC_Button; // Add a little for the order buttons
                SetColumnWidth(col,CurrentWidth);

                if(CurrentWidth < Settings.lMinGridColWidth){

                    int CurrentWidth = GetColumnWidth(col);
                    if (CurrentWidth>0)//Only do unhidden columns, hidden columns have zero width
                        SetColSize(col,(int)Settings.lMinGridColWidth);
                }
                if (CurrentWidth > Settings.lMaxGridColWidth){

                    int CurrentWidth = GetColumnWidth(col);
                    if (CurrentWidth>0)//Only do unhidden columns, hidden columns have zero width
                        SetColSize(col,(int)Settings.lMaxGridColWidth);
                }
                width+=GetColSize(col);
            }
        }

        // Will set the last column to fit the remaining window.
        if (num_cols>0)
        {
            width=GetClientSize().GetWidth()-width-1;
            if(width>=Settings.lMinGridColWidth)
                SetColSize(num_cols-1,width+RoomForASC_DESC_Button);// A little more for the asc buttons
        }
    } else{

        if(num_cols>0){
            int totalWidth = sizeOfFrame.x;

            if(totalWidth>150)
                totalWidth =totalWidth - GetRowLabelSize(); //Account for the Row Title field

            if( num_cols>0){
                width = totalWidth / num_cols;
                width = width;
            }

            else
                width=30; // Some default

            //Determine the width of each column by the size of the frame
            for(int col=0;col<num_cols;col++) {

                bool bHide = false;
                if(col>0){
                    bHide = Utility::HasFlag(m_GridArray[col-1].Flags,"HIDE");
                }

                if(!bHide){
                    SetColSize(col,width);
                    SetColMinimalWidth(col,width);
                } else{
                    int CurrentWidth = GetColumnWidth(col);
                    if (CurrentWidth>0)
                        SetColSize(col,0);
                }
            }
        }
    }
}

// This will be used to order the grid alphabetically.
void DBGrid::OnGridLabelRightClick(wxGridEvent& event )
{
    int row = event.GetRow();
    int col = event.GetCol();

    if(col==-1 && row ==-1){
        if(bFormQueryMode){
            m_sFormQueryTemp = m_sFormQueryOriginal;
            CreateFormQuery(false);//We don't want to re-create the columns, they exist already

        }else {
            LoadGridFromDatabase();
        }
    }

    if(col>=0){
        int posStart = GetColLeft(col);
        int posRight = GetColRight(col);

        wxPoint pt = event.GetPosition();
        pt.x = pt.x - 60;
        wxString msg;

        int LeftXofRect = posStart +2;

        wxRect up(LeftXofRect,2,15,11);
        wxRect down(LeftXofRect,14,15,11);

        if(Utility::IsPointInRect(pt,up)){

            if(bFormQueryMode){

                wxString field = m_GridArray[col].fieldName;
                m_sFormQueryTemp = m_sFormQueryOriginal + " ORDER BY " + field + " ASC";

                CreateFormQuery(false);//We don't want to re-create the columns, they exist already

            }else{
                //m_sWhereCondition
                wxString field = "";
                if(col==0)
                    field = m_sTableName + "Id";
                else if(col>0)
                    field = m_GridArray[col-1].fieldName;

                wxString QueryString = "select * from " + m_sTableName + m_sWhereCondition + " ORDER BY " + field + " ASC";
                LoadGridFromDatabase(false,QueryString);

            }

            ResizeSpreadSheet();

        }
        else if(Utility::IsPointInRect(pt,down)){

            if(bFormQueryMode){

                wxString field = m_GridArray[col].fieldName;
                m_sFormQueryTemp = m_sFormQueryOriginal + " ORDER BY " + field + " DESC";
                CreateFormQuery(false);//We don't want to re-create the columns, they exist already

            }else{
                wxString field = "";
                if(col==0)
                    field = m_sTableName + "Id";
                else if(col>0)
                    field = m_GridArray[col-1].fieldName;

                wxString QueryString = "select * from " + m_sTableName + m_sWhereCondition + " ORDER BY " + field + " DESC";
                LoadGridFromDatabase(false,QueryString);


            }
            ResizeSpreadSheet();
        }
    }
}

// ADD MENU ITEM TO CONTEXT MENU
void DBGrid::OnGridRClick(wxGridEvent& event )
{
    m_iRow = event.GetRow();
    m_iCol = event.GetCol();


    if(!bFormQueryMode){

        SetGridCursor(m_iRow,m_iCol);
        wxPoint point = event.GetPosition();
        auto *menu = new wxMenu;
        //For everyone at the moment, create a filter for the current grid.
        wxString sColumnTitle = GetColLabelValue(m_iCol);
        wxString sCellValue = GetCellValue(m_iRow,m_iCol);

        if(m_sTableName==SYS_TABLES)
            wxString sHTMLDocument = GetCellValue(m_iRow,2);// NOT SURE WHY THIS IS HEAR

        wxString menuLabel = "Filter: " + sColumnTitle +" = " + sCellValue;

        if (Utility::IsSystemDatabaseDeveloper() ) {

            if(m_sTableName==USR_QUERIES)
                menu->Append(ID_MENU_OPEN_FORM_QUERY, wxT("Run Query"), wxT("Run the form query to view the records."));
            else if(m_sTableName==USR_FILTERS)
                menu->Append(ID_MENU_RUN_FILTER, wxT("Run Filter"), wxT("Run the filter to view the records."));
            else if(m_sTableName==SYS_TABLES)
                menu->Append(ID_MENU_OPEN, wxT("Open Table"), wxT("Open the database table."));
            else if(m_sTableName=="usr_forms"){
                menu->Append(ID_MENU_DESIGN_FORMS, wxT("Design Form"), wxT("Design forms that are used to populate pages."));
                menu->Append(ID_MENU_RUN_FORM, wxT("Run Form"), wxT("Run this form."));
            }
            else if(m_sTableName=="usr_pages")
                menu->Append(ID_MENU_DESIGN_PAGES, wxT("Design Page"), wxT("Design the page."));
            else{
                menu->Append(ID_MENU_OPEN, wxT("View Record"), wxT("View record."));

                m_iDocumentColumn = HasRowDocumentFlag(m_iRow);// This will return -1 if we have not document.

                if(m_iDocumentColumn>=0){
                    menu->Append(ID_MENU_DOCUMENT, wxT("View Document"), wxT("View Document."));
                }
            }

            //Only show for the system tables.
            if(m_sTableName==SYS_TABLES)
                menu->Append(ID_MENU_PROPERTIES, wxT("Field Definitions"), wxT("Edit the field definitions."));


            //menu->AppendSeparator();
            menu->Append(ID_MENU_EDIT, wxT("Edit Record"), wxT("Edit Record."));
        }
        else if(Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest()){

            // If we are advanced or standard user on the main form, we only want to show the open menu
            if(m_sTableName==SYS_TABLES){
                menu->Append(ID_MENU_OPEN, wxT("Open Table"), wxT("Open the database table."));

            }
            else{ //If we are not on the main form, we just want to show the open and edit menu.
                menu->Append(ID_MENU_OPEN, wxT("View Record"), wxT("View record."));

                if(!Utility::IsGuest()){
                    menu->Append(ID_MENU_EDIT, wxT("Edit Record"), wxT("Edit Record."));
                }
            }
        }

        menu->AppendSeparator();

        if(m_sTableName!=SYS_FIELDS || Settings.sDatabase=="dbworkssystem")
            menu->Append(ID_MENU_FILTER_SHOW_ALL,  wxT("Filter: Show All Records."), wxT("Filter Show all recordsRecords."));


        int len = menuLabel.Length();
        if(len > 100){
            menuLabel = menuLabel.Left(100);
            menuLabel << " ....";
        }
        if(m_sTableName!=SYS_FIELDS || Settings.sDatabase=="dbworkssystem")
            menu->Append(ID_MENU_FILTER, menuLabel, wxT("Filter Records."));

        PopupMenu( menu, point);
    }

}

void DBGrid::AddItem(const wxString& fieldTitle, const wxString& field, const wxString& flags,const wxString& defaultVal, const wxString& fieldType, const wxString& fieldNull, const wxString& fieldKey,const wxString& fieldExtra )
{
    auto *item = new TableField();
    item->Title = fieldTitle;
    item->fieldName = field;
    item->Flags = flags;
    item->fieldType = fieldType;
    item->fieldExtra = fieldExtra;
    item->fieldNull = fieldNull;
    item->fieldDefault = defaultVal;
    m_GridArray.Add(item);
}

void DBGrid::SetFormQuery(const wxString& str)
{
    m_sFormQueryTemp=str;
    m_sFormQueryOriginal=str;

    if(!m_sFormQueryTemp.IsEmpty())
        bFormQueryMode=true;
}

void DBGrid::SetSettings(const wxString& database, const wxString& server, const wxString& user, const wxString& password, const wxString& tableName, const wxString& KeyName, const wxString& whereCondition)
{
    m_sTableName = tableName;
    m_sPrimaryKeyName = KeyName;
    m_sWhereCondition = whereCondition;
    //  m_sDatabase=database;
    //  m_sServer=server;
    // m_sUser=user;
    // m_sPassword=password;
}
void DBGrid::SetGridWhereCondition(wxString sWhere)
{
    m_sWhereCondition = sWhere;
}

//Loads an Array of GridItem class. Does a copy can creates all new row items
// Returns false if no items in the list
bool DBGrid::GetGridItemArray(ArrayGridItem &GridItemList)
{
    int count = m_GridArray.GetCount();
    if (count>0){

        for (int index=0;index<count;index++){

            auto *item = new TableField();

            item->Title = m_GridArray[index].Title;
            item->fieldName = m_GridArray[index].fieldName;
            item->Flags = m_GridArray[index].Flags;
            item->fieldDefault = m_GridArray[index].fieldDefault;

            GridItemList.Add(item);
        }
        return true;
    }
    return false;
}

bool DBGrid::LoadGridFromDatabase(bool bCheckTableExists, wxString queryToApply)
{
    //auto *f = dynamic_cast<wxFrame*>(this->GetParent());

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

        if(!queryToApply.IsEmpty())
            QueryString = queryToApply;
        else
            QueryString = "select * from " + m_sTableName + m_sWhereCondition;

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        // Display results
        if (res) {
            //This is where we need to check if our result set has the same number of field definitions as our grid, if not, we need to inform the user to create the table alteration.
            int count = m_GridArray.GetCount();
            int RowsInTable = res.num_rows();

            if (count ==0 ){
                wxLogMessage(MSG_FIELD_NOT_CREATED);
                return false;
            }

            //Remove all the rows from the grid and repopulate them.
            DeleteGridRows();

            int iTracRowIncaseOfSkip = 0;
            // Get each row in result set, and print its contents
            for (size_t indexRow = 0; indexRow < RowsInTable; ++indexRow) {
                bool bSkip=false;
                try {

                    if(bCheckTableExists){

                        if (!Utility::IsSystemDatabaseDeveloper()){

                            wxString tableName(res[indexRow][m_GridArray[1].fieldName], wxConvUTF8);
                            if(!Utility::DoesTableExist(Settings.sDatabase,tableName))
                                bSkip=true;
                        }
                    }

                    if(!bSkip){

                        wxString strData0(res[iTracRowIncaseOfSkip][m_sPrimaryKeyName], wxConvUTF8);
                        //Add a new row to the grid control.
                        AppendRows();

                        SetCellValue(iTracRowIncaseOfSkip,0,strData0); //from wxSmith
                        SetReadOnly(iTracRowIncaseOfSkip,0);




                        if (count>0){

                            for(int index=0;index<count;index++){

                                wxString fieldName = m_GridArray[index].fieldName;
                                wxString defaultValue = m_GridArray[index].fieldDefault;
                                wxString flags = m_GridArray[index].Flags;
                                //wsString strData1 = "";
                                //strData1 =  res[i][fieldName];

                                //CHeck if the field name exist
                                bool bMatch=false;
                                int numField = res.num_fields();

                                for(int i=0;i<numField;i++){
                                    wxString  strName="";

                                    strName = res.field_name(i);

                                    if(strName == fieldName){
                                        bMatch = true;
                                        break;
                                    }
                                }

                                if(!bMatch){
                                    wxLogMessage(MSG_FIELD_NOT_CREATED);
                                    return false;

                                }


                                wxString strData1(res[iTracRowIncaseOfSkip][fieldName], wxConvUTF8);


                                //Make sure we don't have an empty field because of a bug that cause field left to overwright the right field
                                if(strData1.IsEmpty()){
                                    //This is where we can place a default value only if there is no value in the database.
                                    if (defaultValue.IsEmpty())
                                        strData1 = " ";
                                    else
                                        strData1 = defaultValue;
                                }
                                int numCol = GetNumberCols();
                                int numRow = GetNumberRows();
                                if(Utility::HasFlag(flags,"PASSWORD")){

                                    if(!Utility::IsEmpty(strData1))
                                        SetCellValue(iTracRowIncaseOfSkip,index+1,Utility::ReplaceStringWithAsterix(strData1));
                                    else
                                        SetCellValue(iTracRowIncaseOfSkip,index+1,strData1);
                                }
                                else if(Utility::HasFlag(flags,"SELECTION_LINKED_NAME") ){
                                    wxArrayString sArray;
                                    wxString flags = m_GridArray[index].Flags;
                                    flags.Replace( "SELECTION_LINKED_NAME", "SELECTION");
                                    Utility::ExtractSelectionItems(sArray,m_GridArray[index].Flags);


                                    wxString tableName = sArray[0];
                                    wxString fieldName = sArray[1];


                                    if (tableName==SYS_TABLES){
                                        SetCellValue(iTracRowIncaseOfSkip, index + 1, Utility::GetTableTitleFromSYS_TABLESById(Settings.sDatabase, strData1)); //Set the value of the cell from the table value.e
                                    }
                                    else
                                     {
                                         wxArrayString sTableResult;
                                         //Utility::LoadStringArrayFromDatabaseTableByName(Settings.sDatabase, sTableResult, tableName, fieldName);
                                         strData1.Trim();
                                         if(!strData1.IsEmpty() && strData1!="NULL"){
                                             Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName, tableName+"Id",strData1);
                                             if(sTableResult.GetCount()>0)
                                                SetCellValue(iTracRowIncaseOfSkip, index + 1,sTableResult[0]);
                                         }
                                     }
                                }
                                else if(Utility::HasFlag(flags,"SELECTION_LINKED_ID")){

                                    wxArrayString sArray;
                                    wxString flags = m_GridArray[index].Flags;
                                    flags.Replace( "SELECTION_LINKED_NAME", "SELECTION");
                                    Utility::ExtractSelectionItems(sArray,m_GridArray[index].Flags);

                                    wxString tableId = sArray[0];
                                    wxString column = sArray[1];
                                    wxArrayString sTableResult;
                                    wxString tableName = Utility::GetTableNameFromSYS_TABLESById(Settings.sDatabase,tableId);
                                    wxString fieldName = Utility::GetTableFieldNameFromMySQLInfoSchema(Settings.sDatabase,tableName,column);

                                    if(fieldName.IsEmpty()){// A field doesn't exist because the user has enter a column index too high, get out of here or log an error
                                        //LOG and error, index too high.


                                    }
                                    strData1.Trim();

                                    if(Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName, tableName+"Id",strData1)){
                                        if(sTableResult.GetCount()>0)
                                            SetCellValue(iTracRowIncaseOfSkip, index + 1,sTableResult[0]);
                                    }

                                }
                                else {
                                    SetCellValue(iTracRowIncaseOfSkip, index + 1, strData1); //Set the value of the cell from the table value.

                                    if(Utility::HasFlag(flags,"WEBLINK")){
                                        SetCellTextColour(iTracRowIncaseOfSkip, index+1, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT));  // hyperlink blue
                                        SetCellFont(iTracRowIncaseOfSkip, index+1, wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).Underlined()); // underline
                                        Connect(wxEVT_GRID_CELL_LEFT_CLICK,wxGridEventHandler( DBGrid::OnGridClick ), nullptr, this );

                                    }
                                }

                                SetReadOnly(iTracRowIncaseOfSkip,index+1);//All entries on the grid are ready only, you can't change values directly via the grid.
                            }
                        }

                        iTracRowIncaseOfSkip++;
                    }
                }
                catch (int& num) {
                    //f->SetStatusText("Database Connected - Row doesn't exist:");
                    //wxLogMessage(MSG_DATABASE_FAIL_ROW);
                    return false;
                }
            }
        }
        else {
            //cerr << "Failed to get stock table: " << query.error() << endl;
            //return 1;
            //f->SetStatusText("Database Connected - Failed to get item list:");
            //wxLogMessage(MSG_FIELD_NOT_CREATED);
            return false;
        }
    }
    else{
        //wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        //f->SetStatusText("Did not connect to database.");
        return false;
    }

    ResizeSpreadSheet();
    return true;
    // f->SetStatusText("Database Connected");
}

//This is used to upload a single grid row so we don't have to reload the entire grid when we update a single item.
bool DBGrid::LoadGridRowFromDatabase(int m_gridRow, bool bCheckTableExists)
{
    if(m_gridRow>-1){

        wxString database(Settings.sDatabase);
        wxString server(Settings.sServer);
        wxString user(Settings.sDatabaseUser);
        wxString pass(Settings.sPassword);

        //Get the database item that needs to be updated.
        wxString Id = GetCellValue(m_gridRow,0);

        // Connect to the sample database.
        Connection conn(false);

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {

            //SetStatusText("Database Connected");
            wxString QueryString = "select * from " + m_sTableName + " WHERE " +m_sTableName+"Id="+Id + " LIMIT 1";
            Query query = conn.query(QueryString);
            StoreQueryResult res = query.store();

            // Display results
            if (res) {
                //This is where we need to check if our result set has the same number of field definitions as our grid, if not, we need to inform the user to create the table alteration.
                int count = m_GridArray.GetCount();
                int RowsInTable = res.num_rows();

                if (count ==0 ){
                    wxLogMessage(MSG_FIELD_NOT_CREATED);
                    return false;
                }

                //int iTracRowIncaseOfSkip = 0;
                // Get each row in result set, and print its contents
                for (size_t indexRow = 0; indexRow < RowsInTable; ++indexRow) {
                    bool bSkip=false;
                    try {

                        if(bCheckTableExists){

                            if (!Utility::IsSystemDatabaseDeveloper()){

                                wxString tableName(res[indexRow][m_GridArray[1].fieldName], wxConvUTF8);
                                if(!Utility::DoesTableExist(Settings.sDatabase,tableName))
                                    bSkip=true;
                            }
                        }

                        if(!bSkip){

                            wxString strData0(res[0][m_sPrimaryKeyName], wxConvUTF8);

                            SetCellValue(m_gridRow,0,strData0); //from wxSmith
                            SetReadOnly(m_gridRow,0);




                            if (count>0){

                                for(int index=0;index<count;index++){

                                    wxString fieldName = m_GridArray[index].fieldName;
                                    wxString defaultValue = m_GridArray[index].fieldDefault;
                                    wxString flag = m_GridArray[index].Flags;
                                    //wsString strData1 = "";
                                    //strData1 =  res[i][fieldName];

                                    //CHeck if the field name exist
                                    bool bMatch=false;
                                    int numField = res.num_fields();

                                    for(int i=0;i<numField;i++){
                                        String  strName;

                                        strName = res.field_name(i);

                                        if(strName == fieldName){
                                            bMatch = true;
                                            break;
                                        }

                                    }


                                    if(!bMatch){
                                        wxLogMessage(MSG_FIELD_NOT_CREATED);
                                        return false;

                                    }


                                    wxString strData1(res[indexRow][fieldName], wxConvUTF8);


                                    //Make sure we don't have an empty field because of a bug that cause field left to overwright the right field
                                    if(strData1.IsEmpty()){
                                        //This is where we can place a default value only if there is no value in the database.
                                        if (defaultValue.IsEmpty())
                                            strData1 = " ";
                                        else
                                            strData1 = defaultValue;
                                    }

                                    if(Utility::HasFlag(flag,"PASSWORD")){

                                        if(!Utility::IsEmpty(strData1))
                                            SetCellValue(m_gridRow,index+1,Utility::ReplaceStringWithAsterix(strData1));
                                        else
                                            SetCellValue(m_gridRow,index+1,strData1);
                                    }
                                    else if(Utility::HasFlag(flag,"SELECTION_LINKED_NAME") ){
                                        wxArrayString sArray;
                                        wxString flags = m_GridArray[index].Flags;
                                        flags.Replace( "SELECTION_LINKED_NAME", "SELECTION");
                                        Utility::ExtractSelectionItems(sArray,m_GridArray[index].Flags);

                                        wxString tableName = sArray[0];
                                        wxString fieldName = sArray[1];

                                        if (tableName==SYS_TABLES){
                                            SetCellValue(m_gridRow, index + 1, Utility::GetTableTitleFromSYS_TABLESById(Settings.sDatabase, strData1)); //Set the value of the cell from the table value.e
                                        }
                                        else
                                        {
                                            wxArrayString sTableResult;
                                            Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName, tableName+"Id",strData1);
                                            if(sTableResult.GetCount()>0)
                                                SetCellValue(m_gridRow, index + 1,sTableResult[0]);
                                        }
                                    }
                                    else if(Utility::HasFlag(flag,"SELECTION_LINKED_ID")){

                                        wxArrayString sArray;
                                        wxString flags = m_GridArray[index].Flags;
                                        flags.Replace( "SELECTION_LINKED_NAME", "SELECTION");
                                        Utility::ExtractSelectionItems(sArray,m_GridArray[index].Flags);

                                        wxString tableId = sArray[0];
                                        wxString column = sArray[1];
                                        wxArrayString sTableResult;
                                        wxString tableName = Utility::GetTableNameFromSYS_TABLESById(Settings.sDatabase,tableId);
                                        wxString fieldName = Utility::GetTableFieldNameFromMySQLInfoSchema(Settings.sDatabase,tableName,column);

                                        strData1.Trim();
                                        if(!strData1.IsEmpty()){

                                            Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName, tableName+"Id",strData1);
                                            if(sTableResult.GetCount()>0)
                                                SetCellValue(m_gridRow, index + 1,sTableResult[0]);
                                        }

                                    }
                                    else{
                                        SetCellValue(m_gridRow,index+1,strData1); //Set the value of the cell from the table value.

                                    }


                                    SetReadOnly(m_gridRow,index+1);//All entries on the grid are ready only, you can't change values directly via the grid.
                                }
                            }
                        }
                    }
                    catch (int& num) {

                        //f->SetStatusText("Database Connected - Row doesn't exist:");
                        wxLogMessage(MSG_DATABASE_FAIL_ROW);
                        return false;
                    }
                }
            }
            else {
                //cerr << "Failed to get stock table: " << query.error() << endl;
                //return 1;
                //f->SetStatusText("Database Connected - Failed to get item list:");
                wxLogMessage(MSG_FIELD_NOT_CREATED);
                return false;
            }
        }
        else{
            wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
            //f->SetStatusText("Did not connect to database.");
            return false;
        }
        return true;
    }
    return true;
}

//Used in conjunction with GetNextcellValue to retrieve all the rows in a cell
bool DBGrid::GetFirstRowCellValue(wxString& sCellValue,int  ColumnNumber)
{
    m_iGetCellRowIndex=0;
    int count=GetNumberRows();
    if (count>0){

        m_iGetCellColumnNumber = ColumnNumber;
        sCellValue = GetCellValue(0,ColumnNumber);
        return true;
    }
    sCellValue = "";
    return false;
}

bool DBGrid::GetNextRowCellValue(wxString& sCellValue)
{
    int count=GetNumberRows();
    if(m_iGetCellRowIndex < count){
        m_iGetCellRowIndex++; // We do this first because we want GetRowIndex to reflect the value retrieved from this function.

        if(m_iGetCellRowIndex < count){
            sCellValue = GetCellValue(m_iGetCellRowIndex,m_iGetCellColumnNumber);
            return true;
        }
    }
    return false;
}
int DBGrid::GetCurrentRowIndex()
{
    return m_iGetCellRowIndex;
}
wxString DBGrid::GetCurrentRowValue(int iColumnNumber)
{
    wxString sCellValue;
    sCellValue = GetCellValue(m_iGetCellRowIndex,iColumnNumber);
    return sCellValue;
}

void DBGrid::HighlightCell(int iRow, int iCol)
{
    SetCellTextColour(iRow,iCol,wxColour(200,0,0));
    //SetInvalidDBField(iCol,true);
}

bool DBGrid::IsCellHighlighted(int iRow, int iCol)
{
    wxColour col = GetCellTextColour(iRow,iCol);

    if(col == wxColour(200,0,0))
        return true;
    return false;
}

void DBGrid::UnHighlightCell(int iRow, int iCol)
{
    SetCellTextColour(iRow,iCol,wxColour(0,0,0));
}

void DBGrid::SetEventType(long type)
{
    m_eventType = type;
}

//Searches for all the columns in a given row on the grid and checks to see if there is DOCUMENT set in the flag.
// Return -1 if no DOCUMENT flag found, return the ID where the document can be found
int DBGrid::HasRowDocumentFlag(int iRow) {

    for (int i=0;i<m_GridArray.GetCount();i++){
        if(Utility::HasFlag(m_GridArray[i].Flags, "DOCUMENT")){
            return i+1;//Because the GridArray doesn't hold the ID of the table, it is generated from the table name automatically.
        }
    }
    return -1;
}

void DBGrid::HideColumn(int colNumber)
{
   HideCol(colNumber);
}

//Used to setup the grid on creation.
void DBGrid::SetGridProperties()
{
    SetSelectionMode(wxGridSelectRows );
    EnableEditing( false );
    EnableGridLines( true );
    EnableDragGridSize( false );

    // Columns
    EnableDragColMove( false );
    EnableDragColSize( false );

    SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

    // Rows
    EnableDragRowSize( false );
    SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

    SetRowLabelSize(60);
    SetColLabelSize(27);

    // Cell Defaults
    SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );

}

void DBGrid::DrawCornerLabel(wxDC &dc)
{
    wxFontInfo info;
    info.Italic(true);
    info.Family(wxFONTFAMILY_ROMAN );
    wxFont ftItalic(info);

    dc.SetFont(ftItalic);

    dc.SetPen( wxPen( wxColor(45,45,45), 1 ) );
    dc.DrawText("DBWorks",wxPoint(5,5));
}

//The difference between create CreateFields and CreateFormQueryFields is that
// m_GridArray also holds the primary key for CreateFormQueryFields
// Normally, m_GridArray doesn't contain the primary key as it's automatically generated.
void DBGrid::CreateFields()
{
     // Don't add any rows yet, they will be added when we query the database.

     int iNumCol = m_GridArray.GetCount()+1;
     CreateGrid( 0, iNumCol,wxGrid::wxGridSelectCells);

    SetGridProperties();
    
     // The database labels
     SetColLabelValue(0, _("ID"));

    int count = m_GridArray.GetCount();

    if (count>0){
        for(int i=0;i<count;i++)
            SetColLabelValue(i+1, m_GridArray[i].Title);
    }
}
//This is the wxWidgets base class overridden.
void DBGrid::DrawColLabel(wxDC & dc, int col)
{
    wxGrid::DrawColLabel(dc,col);

    int posStart = GetColLeft(col);
    int posRight = GetColRight(col);

    int LeftXofRect = posStart +2;

    int LeftXofTriangle = posStart + 4;
    int MidXofTriangle = posStart + 9;
    int RightXofTriange = posStart + 14;

/*
    int LeftXofRect = posRight-18;
    int LeftXofTriangle = posRight-16;
    int MidXofTriangle = posRight-11;
    int RightXofTriange = posRight-6;
    */

    wxBrush brush(wxColor(0,0,255),wxBRUSHSTYLE_SOLID);
    dc.SetPen( wxPen( wxColor(45,45,45), 1 ) );

    wxRect up(LeftXofRect,2,15,11);
    wxRect down(LeftXofRect,14,15,11);

    dc.DrawRectangle(up);
    dc.DrawRectangle(down);

    dc.DrawLine(LeftXofTriangle,9,RightXofTriange,9);
    dc.DrawLine(RightXofTriange,9,MidXofTriangle,4);
    dc.DrawLine(LeftXofTriangle,9,MidXofTriangle,4);

    dc.DrawLine(LeftXofTriangle,17,RightXofTriange,17);
    dc.DrawLine(LeftXofTriangle,17,MidXofTriangle,22);
    dc.DrawLine(MidXofTriangle,22,RightXofTriange,17);


    //  wxBrush brush(wxColor(0,0,0),wxBRUSHSTYLE_SOLID);
    // dc.SetBrush(brush); // green filling

    //dc.FloodFill(posRight-11,6,wxColor(0,0,0));
    //dc.DrawCheckMark(posStart,10,10,15);

}
//This is used for viewing a query results including the primary keys.
void DBGrid::CreateFormQueryColumns()
{
    // Don't add any rows yet, they will be added when we query the database.

    int iNumCol = m_GridArray.GetCount();
    CreateGrid( 0, iNumCol,wxGrid::wxGridSelectCells);

    SetGridProperties();

    // Create the labels for the grid
    int count = m_GridArray.GetCount();

    if (count>0){
        for(int i=0;i<count;i++)
            SetColLabelValue(i, m_GridArray[i].Title);
    }
}

wxString DBGrid::getSelectedFieldValue(const wxString& fieldname)
{
    wxArrayInt rowsSelected = GetSelectedRows();

    if(rowsSelected.size()==1){
        int row = rowsSelected[0]; //Get the first selected row
        int count = m_GridArray.GetCount();
        if (count>0){
            for(int index=0;index<count;index++){
               
                //Make sure we don't have an empty field because of a bug that cause field left to overwright the right field
                if(m_GridArray[index].fieldName==fieldname)
                    return GetCellValue(row,index+1);
            }
        }
    }
    return "";
}


//Delete all the rows from the grid so it can be repopulated again.
void DBGrid::DeleteGridRows()
{
    int numRows = GetNumberRows();
    if(numRows>0)
        DeleteRows(0,numRows);
}

//Delete a single or multiple selected rows from the grid and DB passwords table and the grid.
void DBGrid::DeleteSelectedRow()
{
    wxArrayInt rowsSelected = GetSelectedRows();
    for(int row : rowsSelected) {
        
        wxString scontactId = GetCellValue(row,0);
        DeleteEntryFromDatabase(scontactId);
    }
    LoadGridFromDatabase();
}


// Delete a single entry from the passwords table given the DB passwordId of that entry
void DBGrid::DeleteEntryFromDatabase(const wxString& Id){

    auto *f = dynamic_cast<wxFrame*>(this->GetParent());

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
            
            wxString QueryString = "DELETE FROM " + m_sTableName + " WHERE " + m_sPrimaryKeyName + "=" + Id;
            
            Query query = conn.query(QueryString);
            query.execute();

        }
        //else SetStatusText("Did not connect to database.");
        
    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const BadConversion& er) {
        // Handle bad conversions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    }
}



void DBGrid::OnGridEvents(wxGridEvent& event)
{
     ResizeSpreadSheet();
}

//We can send a message to the parent that this window is destroyed.
bool DBGrid::Destroy()
{
    //Send the message first before we destroy the grid.
    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    bool bResult = wxGrid::Destroy();
    return bResult;
}

// CONTEXT MENU EVENT FUNCTIONS
void DBGrid::OnClickMenuFilter(wxCommandEvent& event)
{

    //Send this event to the parent.
    MyEvent my_event( this );
    my_event.m_iRow = m_iRow;
    my_event.m_iCol = m_iCol;
    my_event.m_sTableName = m_sTableName;
    my_event.m_bRefreshDatabase=true;

    my_event.m_cellValue=GetCellValue(m_iRow,m_iCol); // The value of the current clicked cell.
    wxString sValue = my_event.m_cellValue;

    //Accounting for the fact that m_GridArray doesn't store the ID because that can be generated from the table name ie. tablenameId
    if(m_iCol==0)
        my_event.m_cellFieldName=m_sTableName+"Id";

    if(m_iCol>0)
        my_event.m_cellFieldName=m_GridArray[m_iCol-1].fieldName; // The field name for that cell


    //We need to determine if we have a linked ID field Utility::HasFlag(sFlags,"SELECTION_LINKED_ID" and Utility::HasFlag(sFlags,"SELECTION_LINKED_NAME"
    //Both of these will have ID's stored in the table and not the cell values.
    wxString flags;
    if(m_iCol>0) {
        flags = m_GridArray[m_iCol - 1].Flags;

        if (Utility::HasFlag(flags, "SELECTION_LINKED_ID")) {
            //sValue = itemArray[i].comCtl->GetValue();

            wxArrayString sArray;
            //wxString flags = itemArray[i].flag;
            flags.Replace("SELECTION_LINKED_NAME", "SELECTION");
            Utility::ExtractSelectionItems(sArray, m_GridArray[m_iCol - 1].Flags);

            wxString tableId = sArray[0]; //Table ID to the table we need to lookup
            wxString column = sArray[1];
            wxArrayString sTableResult;
            wxString tableName = Utility::GetTableNameFromSYS_TABLESById(Settings.sDatabase, tableId);
            wxString fieldName = Utility::GetTableFieldNameFromMySQLInfoSchema(Settings.sDatabase, tableName, column);

            // wxString ID;
            Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName,sValue);
            sValue = sTableResult[0];

            my_event.m_sTableId = sValue;

            wxString sWhereCondition = "";
            wxString type  = m_GridArray[m_iCol - 1].fieldType;
            type=type.Lower();
            if(type=="int")
                sWhereCondition << m_GridArray[m_iCol - 1].fieldName << "=" << sValue;
            else
                sWhereCondition << m_GridArray[m_iCol - 1].fieldName << "='" << sValue << "'";
            my_event.m_sWhereCondition = sWhereCondition;

        } else if (Utility::HasFlag(flags, "SELECTION_LINKED_NAME")) {

            wxArrayString sArray;
            //wxString flags = m_GridArray[m_iCol-1].flag;
            flags.Replace("SELECTION_LINKED_NAME", "SELECTION");
            Utility::ExtractSelectionItems(sArray, m_GridArray[m_iCol - 1].Flags);

            wxString tableName = sArray[0];
            wxString fieldName = sArray[1];

            if (tableName == SYS_TABLES) {
                wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTitle(Settings.sDatabase, sValue);

                my_event.m_sTableId = sTableId;

                wxString sWhereCondition = "";

                //We need to find the type because we don't need ' ' for number values.
                wxString type  = m_GridArray[m_iCol - 1].fieldType;
                type=type.Lower();
                if(type=="int")
                    sWhereCondition << m_GridArray[m_iCol - 1].fieldName << "=" << sTableId;
                else
                    sWhereCondition << m_GridArray[m_iCol - 1].fieldName << "='" << sTableId << "'";


                my_event.m_sWhereCondition = sWhereCondition;

            } else {
                wxArrayString sArray;
                Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sArray, tableName, fieldName,sValue);

                if(sArray.GetCount()>0){
                    sValue = sArray[0];
                    my_event.m_sTableId = sValue;

                    wxString sWhereCondition = "";

                    //We need to find the type
                    wxString type  = m_GridArray[m_iCol - 1].fieldType;
                    type=type.Lower();
                    if(type=="int")
                        sWhereCondition << m_GridArray[m_iCol - 1].fieldName << "=" << sValue;
                    else
                        sWhereCondition << m_GridArray[m_iCol - 1].fieldName << "='" << sValue << "'";
                    my_event.m_sWhereCondition = sWhereCondition;
                }
            }
        }else{

            wxString str = GetCellValue(m_iRow, 0);
            my_event.m_sTableId = str;

            wxString sWhereCondition = "";
            wxString type  = m_GridArray[m_iCol - 1].fieldType;
            type=type.Lower();
            if(type=="int")
                sWhereCondition << my_event.m_cellFieldName << "=" << my_event.m_cellValue;
            else
                sWhereCondition << my_event.m_cellFieldName << "='" << my_event.m_cellValue + "'";
            my_event.m_sWhereCondition = sWhereCondition;
        }
        //my_event.SetRow(event.GetRow());
        my_event.SetEventType(m_eventType);
        GetParent()->ProcessWindowEvent(my_event);
    }
}
void DBGrid::OnClickMenuFilterShowAll(wxCommandEvent& event)
{
    //Send this event to the parent.
    MyEvent my_event( this );
    my_event.m_iRow = m_iRow;
    my_event.m_iCol = m_iCol;
    my_event.m_bRefreshDatabase=true;
    my_event.m_bShowAll=true;
    my_event.m_sTableName = m_sTableName;
    my_event.m_sWhereCondition = "";

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );

}

// CONTEXT MENU EVENT FUNCTIONS
void DBGrid::OnClickProperties(wxCommandEvent& event)
{
    //wxLogMessage("Properties");

    //Send this event to the parent.
    MyEvent my_event( this );
    my_event.m_iRow = m_iRow;
    my_event.m_iCol = m_iCol;
    my_event.m_bTableFieldDefinitions = true;

    if(m_sTableName==SYS_TABLES)
        my_event.m_sTableName = GetCellValue(m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.

    wxString str = GetCellValue(m_iRow,0);
    my_event.m_sTableId=str;

    //my_event.SetRow(event.GetRow());
    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );


}
void DBGrid::OnClickOpen(wxCommandEvent& event)
{
    MyEvent my_event( this );
    my_event.m_iRow = m_iRow;

    if(IsCellHighlighted(m_iRow,2)){
        wxLogMessage(MSG_FIELD_NOT_CREATED);

        return;
    }


    my_event.m_iCol = m_iCol;
    my_event.m_bOpen = true;

    if(m_sTableName==SYS_TABLES)
        my_event.m_sTableName = GetCellValue(m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.

    wxString str = GetCellValue(m_iRow,0);
    my_event.m_sTableId=str;

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}
void DBGrid::OnClickEdit(wxCommandEvent& event)
{
    MyEvent my_event( this );
    my_event.m_iRow = m_iRow;
    my_event.m_iCol = m_iCol;
    my_event.m_bEdit = true;

    if(m_sTableName==SYS_TABLES)
        my_event.m_sTableName = GetCellValue(m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.

    wxString str = GetCellValue(m_iRow,0);
    my_event.m_sTableId=str;

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

//I have a grid click
void DBGrid::OnGridClick(wxGridEvent& event )
{
    //SelectRow(event.GetRow());
    int col = event.GetCol();
    int row = event.GetRow();

    SelectRow(row);

    if(col>0){
        wxString flags = m_GridArray[col-1].Flags;

        if(Utility::HasFlag(flags,"WEBLINK")){
            wxString webLink = GetCellValue(row,col);
            //wxExecute(webLink);
           wxLaunchDefaultBrowser(webLink);
        }
    }
}

void DBGrid::OnGridDLClick(wxGridEvent& event )
{
    //Create a new event to tell the parent to open record or edit the record.
    MyEvent my_event( this );
    my_event.m_iRow = event.GetRow();
    my_event.m_iCol = event.GetCol();
    m_iRow = my_event.m_iRow;
    m_iCol=my_event.m_iCol;
    if(IsCellHighlighted(my_event.m_iRow,2)){
        wxLogMessage(MSG_FIELD_NOT_CREATED);
        return;
    }

    //Change the behaviour of double click cell to view or edit item depending on the system setting.
    if(m_sTableName==USR_QUERIES || m_sTableName==USR_FILTERS ){
        my_event.m_bOpenQueryGrid = true;
        my_event.m_sTitle = GetCellValue(my_event.m_iRow,1);
        my_event.m_sQueryToApply = GetCellValue(my_event.m_iRow,2);
    }
    else if(Settings.sDClickGridCell=="VIEW")
        my_event.m_bOpen = true;
    else if(Settings.sDClickGridCell=="EDIT" && m_sTableName!=SYS_TABLES && !Utility::IsGuest())
        my_event.m_bEdit = true;
    else
        my_event.m_bOpen = true;

    if(m_sTableName==SYS_TABLES)
        my_event.m_sTableName = GetCellValue(my_event.m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.

    wxString str = GetCellValue(my_event.m_iRow,0);
    my_event.m_sTableId=str;

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

//This is where we are going to parse the HTML document.
void DBGrid::OnClickOpenDocument(wxCommandEvent& event)
{
    MyEvent my_event( this );

    if(IsCellHighlighted(m_iRow,m_iDocumentColumn)){
        wxLogMessage(MSG_FIELD_NOT_CREATED);
        return;
    }

    my_event.m_bParseDocument = true;

    //The cell value with the HTML document to parse
    wxString str = GetCellValue(m_iRow,m_iDocumentColumn);
    my_event.m_cellValue=str;

    //my_event.SetRow(event.GetRow());
    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

void DBGrid::OnOpenFormQuery(wxCommandEvent& event)
{
    MyEvent my_event( this );

    my_event.m_bOpenQueryGrid = true;
    my_event.m_sTitle = GetCellValue(m_iRow,1);
    my_event.m_sQueryToApply = GetCellValue(m_iRow,2);

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}
void DBGrid::OnDesignPage(wxCommandEvent& event)
{
    MyEvent my_event( this );

    my_event.m_bOpenDesignPage = true;
    my_event.m_sTableId = GetCellValue(m_iRow,0); //This will be usr_pagesId
    my_event.m_sTableName = GetCellValue(m_iRow,1);

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

void DBGrid::OnDesignForm(wxCommandEvent& event)
{
    MyEvent my_event( this );

    my_event.m_bOpenDesignForm = true;
    my_event.m_sTableId = GetCellValue(m_iRow,0); //This will be usr_formsId
    my_event.m_sTableName = GetCellValue(m_iRow,1);

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

void DBGrid::OnRunForm(wxCommandEvent& event)
{
    MyEvent my_event( this );

    my_event.m_bRunForm = true;
    my_event.m_sTableId = GetCellValue(m_iRow,0); //This will be usr_formsId
    my_event.m_sTableName = GetCellValue(m_iRow,1);

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

void DBGrid::OnRunFilter(wxCommandEvent& event)
{
    MyEvent my_event( this );

    my_event.m_bOpenQueryGrid = true; //This works for filters also.
    my_event.m_sTitle = GetCellValue(m_iRow,1);
    my_event.m_sQueryToApply = GetCellValue(m_iRow,2);

    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}

//This is actually the starting point to create a grid based on a query including the titles because have
//no information about the grid until the query is run.
bool DBGrid::CreateFormQuery(bool bCreateColumns)
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

        //Remove those stupid unicode characters that mySQL doesn't understand.
        Utility::EscapeAscii(m_sFormQueryTemp);

        //SetStatusText("Database Connected");
        Query query = conn.query(m_sFormQueryTemp);
        StoreQueryResult res = query.store();

        // Display results
        if (res) {

            // The first thing we need to do is read all the fields definitions from the resultant recordset. Then we have to define the m_GridArray and load it with all the field.
            // Once we have the definition,
           // int RowsInTable = res.num_rows();

            if(bCreateColumns){
                int num_fields = res.num_fields();
                for (int fieldIndex=0;fieldIndex<num_fields; fieldIndex++){
                    //These are all the field names from our query.
                    wxString sFieldName = res.field_name(fieldIndex);
                    AddItem(sFieldName,sFieldName );
                }
                CreateFormQueryColumns();
            } else
                DeleteGridRows();

            int RowsInTable = res.num_rows();

            //int iTracRowIncaseOfSkip = 0;
            // Get each row in result set, and print its contents
            for (int indexRow = 0; indexRow < RowsInTable; ++indexRow) {

                try {
                        int iCount = m_GridArray.GetCount();

                        //It looks like deleted row show up also. This may be useful in the future but better not toshow them
                        AppendRows();// Add a new grid row

                            for(int Colindex=0;Colindex<iCount;Colindex++){
                                //Get the field from the loaded grid array.
                                wxString fieldName = m_GridArray[Colindex].fieldName;

                                //Get the record data for that field
                                wxString strData1(res[indexRow][fieldName], wxConvUTF8);

                                SetReadOnly(indexRow,Colindex);
                                SetCellValue(indexRow,Colindex,strData1); //from wxSmith
                                //
                            }

                }
                catch (int& num) {

                    //f->SetStatusText("Database Connected - Row doesn't exist:");
                    wxLogMessage(MSG_FAILED_QUERY_FILTER);
                    return false;
                }
            }
        }
        else {
                wxLogMessage(MSG_FAILED_QUERY_FILTER);
                return false;

        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        return false;
        //f->SetStatusText("Did not connect to database.");
    }
    return true;
}