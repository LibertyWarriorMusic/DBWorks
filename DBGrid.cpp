
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


#include "global.h"
#include "MyEvent.h"
#include "Utility.h"
#include "DBGrid.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayGridItem);

enum {
    ID_MENU_PROPERTIES = wxID_HIGHEST + 500,
    ID_MENU_OPEN,
    ID_MENU_EDIT,
    ID_MENU_FILTER,
    ID_MENU_FILTER_SHOW_ALL,
    ID_MENU_DOCUMENT
};

wxBEGIN_EVENT_TABLE(DBGrid, wxGrid)
    EVT_MENU(ID_MENU_PROPERTIES, DBGrid::OnClickProperties)
    EVT_MENU(ID_MENU_OPEN, DBGrid::OnClickOpen)
    EVT_MENU(ID_MENU_DOCUMENT, DBGrid::OnClickOpenDocument)
    EVT_MENU(ID_MENU_EDIT, DBGrid::OnClickEdit)
    EVT_MENU(ID_MENU_FILTER, DBGrid::OnClickMenuFilter)
    EVT_MENU(ID_MENU_FILTER_SHOW_ALL, DBGrid::OnClickMenuFilterShowAll)
wxEND_EVENT_TABLE()

using namespace mysqlpp;

DBGrid::DBGrid(wxWindow* _parent,wxWindowID _ID,wxPoint _pos,wxSize _size,long _style) : wxGrid( _parent, _ID, _pos, _size, _style )
{
    m_eventType = 0;
    m_iRow = 0;
    m_iCol = 0;
    m_iGetCellRowIndex=0;



    // Connect Events
    Connect(wxEVT_SIZE,wxGridEventHandler( DBGrid::OnSizeGridSpreadSheet ), nullptr, this );
    Connect(wxEVT_GRID_SELECT_CELL,wxGridEventHandler( DBGrid::OnGridEvents ), nullptr, this );
    Connect(wxEVT_GRID_CELL_RIGHT_CLICK,wxGridEventHandler( DBGrid::OnGridRClick ), nullptr, this );
    Connect(wxEVT_GRID_CELL_LEFT_DCLICK,wxGridEventHandler( DBGrid::OnGridDLClick ), nullptr, this );
    //Connect(wxEVT_GRID_CELL_LEFT_CLICK,wxGridEventHandler( DBGrid::OnGridClick ), nullptr, this );

}
 DBGrid::~DBGrid()
{
    Disconnect( wxEVT_SIZE, wxGridEventHandler( DBGrid::OnSizeGridSpreadSheet ), nullptr, this );
    Disconnect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( DBGrid::OnGridEvents ), nullptr, this );
    Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( DBGrid::OnGridRClick ), nullptr, this );
    Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( DBGrid::OnGridDLClick ), nullptr, this );
   // Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( DBGrid::OnGridClick ), nullptr, this );
    //m_GridArray.Empty();
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

            auto *item = new GridItem();

            item->title = m_GridArray[index].title;
            item->field = m_GridArray[index].field;
            item->flags = m_GridArray[index].flags;
            item->defaultValue = m_GridArray[index].defaultValue;

            GridItemList.Add(item);

        }


        return true;
    }
    return false;
}

bool DBGrid::LoadGridFromDatabase(bool bCheckTableExists)
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
        wxString QueryString = "select * from " + m_sTableName + m_sWhereCondition;
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

                            wxString tableName(res[indexRow][m_GridArray[1].field], wxConvUTF8);
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

                                wxString fieldName = m_GridArray[index].field;
                                wxString defaultValue = m_GridArray[index].defaultValue;
                                wxString flag = m_GridArray[index].flags;
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


                                wxString strData1(res[iTracRowIncaseOfSkip][fieldName], wxConvUTF8);


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
                                        SetCellValue(iTracRowIncaseOfSkip,index+1,Utility::ReplaceStringWithAsterix(strData1));
                                    else
                                        SetCellValue(iTracRowIncaseOfSkip,index+1,strData1);
                                }
                                else {
                                    SetCellValue(iTracRowIncaseOfSkip, index + 1, strData1); //Set the value of the cell from the table value.

                                    if(Utility::HasFlag(flag,"WEBLINK")){
                                        SetCellTextColour(iTracRowIncaseOfSkip, index+1, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT));  // hyperlink blue
                                        SetCellFont(iTracRowIncaseOfSkip, index+1, wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).Underlined()); // underline
                                        Connect(wxEVT_GRID_CELL_LEFT_CLICK,wxGridEventHandler( DBGrid::OnGridClick ), nullptr, this );
                                        /*
                                        wxGridCellAttr * at = this->GetCellAttr(iTracRowIncaseOfSkip, index + 1);


                                        wxHyperlinkCtrl * hp = new wxHyperlinkCtrl( this, wxID_ANY,"weblink","https://www.google.com", wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU | wxHL_DEFAULT_STYLE ,wxHyperlinkCtrlNameStr);
                                        //itemArray[i].linkCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );

                                        hp->SetURL(strData1);
                                        hp->SetLabel(strData1);
                                        */

                                        //wxGridCellRenderer * renderer = GetCellRenderer(iTracRowIncaseOfSkip, index+1);
                                        //renderer->SetClientObject()
                                        //Attach a mouse enter event and a mouse leave event to the cell window

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

    ResizeSpreadSheet();
    return true;
    // f->SetStatusText("Database Connected");
}

bool DBGrid::LoadGridRowFromDatabase(int m_gridRow, bool bCheckTableExists)
{
    if(m_gridRow>-1){

        //auto *f = dynamic_cast<wxFrame*>(this->GetParent());

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

                                wxString tableName(res[indexRow][m_GridArray[1].field], wxConvUTF8);
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

                                    wxString fieldName = m_GridArray[index].field;
                                    wxString defaultValue = m_GridArray[index].defaultValue;
                                    wxString flag = m_GridArray[index].flags;
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
                                    else
                                        SetCellValue(m_gridRow,index+1,strData1); //Set the value of the cell from the table value.


                                    SetReadOnly(m_gridRow,index+1);//All entries on the grid are ready only, you can't change values directly via the grid.
                                }
                            }


                            //iTracRowIncaseOfSkip++;

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

        ResizeSpreadSheet();
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

// ADD MENU ITEM TO CONTEXT MENU
void DBGrid::OnGridRClick(wxGridEvent& event )
{
    m_iRow = event.GetRow();
    m_iCol = event.GetCol();
    SetGridCursor(m_iRow,m_iCol);
    wxPoint point = event.GetPosition();
    auto *menu = new wxMenu;
    //For everyone at the moment, create a filter for the current grid.
    wxString sColumnTitle = GetColLabelValue(m_iCol);
    wxString sCellValue = GetCellValue(m_iRow,m_iCol);
    wxString sHTMLDocument = GetCellValue(m_iRow,2);
    wxString menuLabel = "Filter: " + sColumnTitle +" = " + sCellValue;


    if (Utility::IsSystemDatabaseDeveloper() ) {

        if(m_sTableName==SYS_TABLES)
            menu->Append(ID_MENU_OPEN, wxT("Open Table"), wxT("Open the database table."));
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

        menu->AppendSeparator();
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

    if(m_sTableName!=SYS_FIELDS)
        menu->Append(ID_MENU_FILTER_SHOW_ALL,  wxT("Filter: Show All Records."), wxT("Filter Show all recordsRecords."));



    int len = menuLabel.Length();
    if(len > 100){
        menuLabel = menuLabel.Left(100);
        menuLabel << " ....";
    }
    if(m_sTableName!=SYS_FIELDS)
        menu->Append(ID_MENU_FILTER, menuLabel, wxT("Filter Records."));


    PopupMenu( menu, point);
}
//Searches for all the columns in a given row on the grid and checks to see if there is DOCUMENT set in the flag.
// Return -1 if no DOCUMENT flag found, return the ID where the document can be found
int DBGrid::HasRowDocumentFlag(int iRow) {

    for (int i=0;i<m_GridArray.GetCount();i++){
        if(Utility::HasFlag(m_GridArray[i].flags, "DOCUMENT")){
            return i+1;//Because the GridArray doesn't hold the ID of the table, it is generated from the table name automatically.
        }
    }
    return -1;

}
void DBGrid::HideColumn(int colNumber)
{
   // SetColumnWidth(colNumber,0);
   HideCol(colNumber);
}

void DBGrid::CreateFields()
{
     // Don't add any rows yet, they will be added when we query the database.
     CreateGrid( 0, (int)m_GridArray.GetCount()+1,wxGrid::wxGridSelectCells);
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

     // Cell Defaults
     SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
    
     // The database labels
     SetColLabelValue(0, _("ID"));

    int count = m_GridArray.GetCount();


    if (count>0){
        
        for(int i=0;i<count;i++)
             SetColLabelValue(i+1, m_GridArray[i].title);
    }
}
void DBGrid::AddItem(const wxString& fieldTitle, const wxString& field, const wxString& flags,const wxString& defaultVal="")
{
    auto *item = new GridItem();
    item->title = fieldTitle;
    item->field = field;
    item->flags = flags;
    item->defaultValue = defaultVal;
    m_GridArray.Add(item);

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

wxString DBGrid::getSelectedFieldValue(const wxString& fieldname)
{
    
    
    wxArrayInt rowsSelected = GetSelectedRows();

    if(rowsSelected.size()==1){
        int row = rowsSelected[0]; //Get the first selected row
        int count = m_GridArray.GetCount();
        if (count>0){
            for(int index=0;index<count;index++){
               
                //Make sure we don't have an empty field because of a bug that cause field left to overwright the right field
                if(m_GridArray[index].field==fieldname)
                    return GetCellValue(row,index+1);
            }
        }
    }
    
    return "";
    
}



//Adjects the widths of columns to fit the text.
void DBGrid::ResizeSpreadSheet()
{
     wxSize sizeOfFrame= GetVirtualSize();
     
     int num_cols=GetNumberCols();
     int width=GetRowLabelSize();
     int frameSizeWidth=sizeOfFrame.GetWidth();
     int CurrentWidth=0;
     int totalGridWidth = num_cols*width;
     
     if (frameSizeWidth>totalGridWidth){
         for(int col=0;col<num_cols-1;col++)
         {
             //wxNOT_FOUND
             
             int find = wxNOT_FOUND;
             
             if(col>0)
                 find =m_GridArray[col-1].flags.Find("HIDE");
             
             if(col>0 && find != wxNOT_FOUND){

                 //Only set it if it's not allready zero, zero width means it's hidden.
                 int CurrentWidth = GetColumnWidth(col);
                 if (CurrentWidth>0)
                     SetColSize(col,0);


                 //wxString msg;
                 //msg << col;
                 //wxMessageBox("Hiding");
                 
             }else{
                 //Auto size the column to the with so we can read the correct width value from getColSize()
                 AutoSizeColumn(col);
                 
                 // Get the column width and make sure it's over 100.
                 CurrentWidth=GetColSize(col);
                                 
                 if(CurrentWidth < Settings.lMinGridColWidth){

                     int CurrentWidth = GetColumnWidth(col);
                     if (CurrentWidth>0)
                         SetColSize(col,(int)Settings.lMinGridColWidth);
                 }


                  if (CurrentWidth > Settings.lMaxGridColWidth){

                      int CurrentWidth = GetColumnWidth(col);
                      if (CurrentWidth>0)
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
                 SetColSize(num_cols-1,width);
         }
     }

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


void DBGrid::OnSizeGridSpreadSheet( wxGridEvent& event )
{
    ResizeSpreadSheet();
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


    //Accounting for the fact that m_GridArray doesn't store the ID because that can be generated from the table name ie. tablenameId
    if(m_iCol==0)
        my_event.m_cellFieldName=m_sTableName+"Id";

    if(m_iCol>0)
        my_event.m_cellFieldName=m_GridArray[m_iCol-1].field; // The field name for that cell


    wxString str = GetCellValue(m_iRow,0);
    long val;
    str.ToLong(&val);
    my_event.m_lTableID=val;

    wxString sWhereCondition = "";
    sWhereCondition<< my_event.m_cellFieldName << "='" << my_event.m_cellValue + "'";
    my_event.m_sWhereCondition = sWhereCondition;

    //my_event.SetRow(event.GetRow());
    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );


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

    //my_event.SetRow(event.GetRow());
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
    my_event.m_sTableName = GetCellValue(m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.
    wxString str = GetCellValue(m_iRow,0);
    long val;
    str.ToLong(&val);
    my_event.m_lTableID=val;

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
    my_event.m_sTableName = GetCellValue(m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.
    wxString str = GetCellValue(m_iRow,0);
    long val;
    str.ToLong(&val);
    my_event.m_lTableID=val;

    //my_event.SetRow(event.GetRow());
    my_event.SetEventType(m_eventType);
    GetParent()->ProcessWindowEvent( my_event );
}
void DBGrid::OnClickEdit(wxCommandEvent& event)
{
    MyEvent my_event( this );
    my_event.m_iRow = m_iRow;
    my_event.m_iCol = m_iCol;

    my_event.m_bEdit = true;
    my_event.m_sTableName = GetCellValue(m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.
    wxString str = GetCellValue(m_iRow,0);
    long val;
    str.ToLong(&val);
    my_event.m_lTableID=val;

    //my_event.SetRow(event.GetRow());
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
        wxString flags = m_GridArray[col-1].flags;

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

    if(IsCellHighlighted(my_event.m_iRow,2)){
        wxLogMessage(MSG_FIELD_NOT_CREATED);

        return;
    }
    //Change the behaviour of double click cell to view or edit item depending on the system setting.
    if(Settings.sDClickGridCell=="VIEW")
        my_event.m_bOpen = true;
    else if(Settings.sDClickGridCell=="EDIT" && m_sTableName!=SYS_TABLES && !Utility::IsGuest())
        my_event.m_bEdit = true;
    else
        my_event.m_bOpen = true;

    my_event.m_sTableName = GetCellValue(my_event.m_iRow,2);//Get the value of the first column. NOTE: This is generic, I might be able to set the table name differently.
    wxString str = GetCellValue(my_event.m_iRow,0);
    long val;
    str.ToLong(&val);
    my_event.m_lTableID=val;

//my_event.SetRow(event.GetRow());
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