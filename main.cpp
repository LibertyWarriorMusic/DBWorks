#include <iostream>

#define MAIN_WIN_TITLE "DBWorks"
#define MAIN_WIN_POSX 200
#define MAIN_WIN_POSY 200

#include <wx/wxprec.h>
#include<wx/wx.h>

#include <wx/string.h>
#include <wx/combo.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/frame.h>
#include <wx/datetime.h>
#include <wx/cmdline.h>
#include <wx/textfile.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/toolbar.h>

#include "Utility.h"
#include "global.h"
#include "MyEvent.h"
#include "GenericItemForm.h"
#include "DBGrid.h"
#include "GenericTable.h"
#include "PropertyTable.h"
#include <mysql.h>
#include <mysql++.h>


using namespace mysqlpp;
//Add my own classes here

// The main header must be included last
#include "main.h"

//All includes must be included before you include main.h because they might be needed in main.h

// Declare the global settings
GlobalSettings Settings;

enum {
    ID_ButtonAction = wxID_HIGHEST + 1000,
    MENU_New,
    MENU_Open,
    MENU_Close,
    MENU_Save,
    MENU_SaveAs,
    MENU_Quit,
    ID_TOOL_ADD,
    ID_TOOL_EDIT,
    ID_TOOL_DELETE,
    ID_TOOL_VIEW,
    ID_HELP
};



wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_ButtonAction, MainFrame::OnButtonAction)
    EVT_MENU(MENU_New, MainFrame::NewFile)
    EVT_MENU(MENU_Open, MainFrame::OpenFile)
    EVT_MENU(MENU_Close, MainFrame::CloseFile)
    EVT_MENU(MENU_Save, MainFrame::SaveFile)
    EVT_MENU(MENU_SaveAs, MainFrame::SaveFileAs)
    EVT_MENU(MENU_Quit, MainFrame::Quit)
    EVT_TOOL(ID_TOOL_ADD, MainFrame::OnbAddItem)
    EVT_TOOL(ID_TOOL_EDIT, MainFrame::OnbEditItem)
    EVT_TOOL(ID_TOOL_DELETE, MainFrame::OnbDeleteItem)
    EVT_TOOL(ID_TOOL_VIEW, MainFrame::OnbViewItem)
    EVT_TOOL(ID_HELP, MainFrame::OnbHelp)
    EVT_MYEVENT(MainFrame::OnMyEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);
///////////////////////////////////////////////////////////////////////////
//=============
// MyApp Class Methods
//

bool MyApp::OnInit()
{
    

    LoadAppSettings();
    
    m_MainFrame = new MainFrame((wxFrame*) nullptr,
                                   -1,
                                  wxT(MAIN_WIN_TITLE),
                                  wxPoint(MAIN_WIN_POSX,MAIN_WIN_POSY),
                                  wxSize((int)Settings.lMainWindowWidth,(int)Settings.lMainWindowHeight),
                                (unsigned)wxDEFAULT_FRAME_STYLE
                                  );
    
    m_MainFrame->Show(true);
    
    return true;
}

void MyApp::LoadAppSettings()
{
    wxString        str;
    wxString strExe = wxStandardPaths::Get().GetExecutablePath();
    
    //This should work fine for windows - linus and mac.
    strExe.Replace( "DBWorks", "settings.ini"); //For mac and linux
    strExe.Replace( "dbworks", "settings.ini"); //For mac and linux
    strExe.Replace( "dbworks.exe", "settings.ini"); // For windows.
    //Settings.Message = strExe;

    // open the file
    wxTextFile      tfile;
    tfile.Open(strExe);

    // read the first line
    str = tfile.GetFirstLine();
    ProcessLine(str); // placeholder, do whatever you want with the string

    // read all lines one by one
    // until the end of the file
    while(!tfile.Eof())
    {
        str = tfile.GetNextLine();
        ProcessLine(str); // placeholder, do whatever you want with the string
    }
}

void MyApp::ProcessLine(wxString line)
{
    //Is the line is blank, then do nothing.
    if (line.IsEmpty())
        return;
    
    line = line.Trim(false);// Remove spaces from the left.
    
    wxString temp;
    temp = line.Trim(1);
    
    // If the first character is a comment, then do nothing.
    if (temp=="#")
        return;
    
    // A block definition, do nothing for now, but will be used to determine which structures to load
    // if the project gets larger.
    if(temp=="[")
        return;
    
    wxString setting = line.BeforeFirst('=');
    wxString value = line.AfterFirst('=');
    value = value.Trim(true); //Remove any spaces at the end
    
    long tempValue;
    
    //Set the correct setting.
    if (setting=="DATABASE"){
        Settings.sDatabase=value;
    }else if (setting=="USER"){
        Settings.sUser=value;
    }else if (setting=="PASSWORD"){
        Settings.sPassword=value;
    }else if (setting=="SERVER"){
        Settings.sServer=value;
    }else if (setting=="USERGROUP"){
        Settings.sUsergroup=value;
    }else if (setting=="MAIN_WINDOW_WIDTH"){
        value.ToLong(&tempValue);
        Settings.lMainWindowWidth=tempValue;
    }else if (setting=="MAIN_WINDOW_HEIGHT"){
        value.ToLong(&tempValue);
        Settings.lMainWindowHeight=tempValue;
    }else if (setting=="MAX_GRID_COL_WIDTH"){
        value.ToLong(&tempValue);
        Settings.lMaxGridColWidth=tempValue;
    }else if (setting=="MIN_GRID_COL_WIDTH"){
        value.ToLong(&tempValue);
        Settings.lMinGridColWidth=tempValue;
    }else if (setting=="SYS_TABLES_DOC_ID"){
        value.ToLong(&tempValue);
        Settings.iSysTablesDocID=tempValue;
    }else if (setting=="SYS_FIELDS_DOC_ID"){
        value.ToLong(&tempValue);
        Settings.iSysFieldsDocID=tempValue;
    }else if (setting=="SYS_DOCS_DOC_ID"){
        value.ToLong(&tempValue);
        Settings.iSysDocsDocID=tempValue;
    }else if (setting=="DATABASE_SELECTION_LIST"){
        value.ToLong(&tempValue);
        Settings.sDatbaseSelectionList=value;
    }else if (setting=="DOUBLE_CLICK_GRID_CELL"){
        Settings.sDClickGridCell=value;
    }





}

//=============
// MyFrame Class Methods
//
void MainFrame::OnButtonAction( wxCommandEvent& event )
{

    SetStatusText(_("Status Text"));
    
    
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_TableForm = nullptr;
    m_HtmlWin = nullptr;
    m_MainGrid = nullptr;

    iOldComboIndex=0;
    m_sDefaultUsergroupFilterCondition="";


    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    //Create the main menubar
      //Create the main menu
      m_Menubar = new wxMenuBar( 0 );

      m_FileMenu = new wxMenu();
/*
      m_FileMenu->Append(MENU_New,
          _("&New"), _("Create a new file"));

      m_FileMenu->AppendSeparator();
      m_FileMenu->Append(MENU_Open,
          _("&Open"), _("Open an existing file"));
      m_FileMenu->Append(MENU_Close,
          _("&Close"), _("Close the current document"));

      m_FileMenu->AppendSeparator();
      m_FileMenu->Append(MENU_Save,
          _("&Save"), _("Save the current document"));
      m_FileMenu->Append(MENU_SaveAs,
          _("Save &As"), _("Save the current document under a new file  name"));

      m_FileMenu->AppendSeparator();*/
      m_FileMenu->Append(MENU_Quit,
          _("&Quit"), _("Quit the editor"));

      m_Menubar->Append(m_FileMenu, _("&File"));


    LoadGrid();
    



    
    //this->Centre( wxBOTH );
    m_StatusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

    //======
    // Setup the toolbar
    m_Toolbar = this->CreateToolBar();
    wxString strExe = wxStandardPaths::Get().GetExecutablePath(); // Get the path to the images
    wxInitAllImageHandlers(); //You need to call this or the images will not load.

    if (Utility::IsSystemDatabaseDeveloper() ) {

        //Load images for the toolbar
        wxBitmap imageAdd;
        strExe.Replace("DBWorks", "images/add.png"); //For mac and linux
        strExe.Replace("dbworks", "images/add.png"); //For mac and linux
        strExe.Replace("dbworks.exe", "images/add.png"); // For windows.
        imageAdd.LoadFile(strExe, wxBITMAP_TYPE_PNG);
        m_Toolbar->AddTool(ID_TOOL_ADD, wxT("Add a new table to the database."), imageAdd,
                           wxT("Add a new table to the database."));

        wxBitmap imageEdit;
        strExe.Replace("add.png", "edit.png");
        imageEdit.LoadFile(strExe, wxBITMAP_TYPE_PNG);
        m_Toolbar->AddTool(ID_TOOL_EDIT, wxT("Edit the selected table."), imageEdit, wxT("Edit the selected table."));


        wxBitmap imageDelete;
        strExe.Replace("edit.png", "delete.png");
        imageDelete.LoadFile(strExe, wxBITMAP_TYPE_PNG);
        m_Toolbar->AddTool(ID_TOOL_DELETE, wxT("Delete the selected table, will also delete all data."), imageDelete,
                           wxT("Delete the selected table, will also delete all data."));

        wxBitmap imageView;
        strExe.Replace("delete.png", "view.png");
        imageView.LoadFile(strExe, wxBITMAP_TYPE_PNG);
        m_Toolbar->AddTool(ID_TOOL_VIEW, wxT("View the selected table."), imageView, wxT("View the selected table."));

    }
    else if (Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest()){


        strExe.Replace("DBWorks", "images/view.png"); //For mac and linux
        strExe.Replace("dbworks", "images/view.png"); //For mac and linux
        strExe.Replace("dbworks.exe", "images/view.png"); // For windows.


        wxBitmap imageView;

        imageView.LoadFile(strExe, wxBITMAP_TYPE_PNG);
        m_Toolbar->AddTool(ID_TOOL_VIEW, wxT("View the selected table."), imageView, wxT("View the selected table."));

        // For standard users, we don't want to see ID and Table name
        if(Utility::IsStandardUser() || Utility::IsGuest())
        {
            m_MainGrid->HideColumn(0);
            m_MainGrid->HideColumn(2);

        } else
            m_MainGrid->HideColumn(2); // For advanced user, we don't want to see the tablename, just show the ID
    }

    wxBitmap bitmapHelp;
    strExe.Replace("view.png", "help.png"); //For mac and linux
    bitmapHelp.LoadFile(strExe, wxBITMAP_TYPE_PNG);

    wxStaticText * txtClt = new wxStaticText( m_Toolbar, wxID_ANY, "Database ", wxDefaultPosition, wxDefaultSize, 0 );

    m_DatabaseCombo = new wxComboBox( m_Toolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,wxCB_READONLY);
    m_DatabaseCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( MainFrame::OnComboChange ), nullptr, this );
    m_DatabaseCombo->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( MainFrame::OnComboDropDown ), nullptr, this );

    m_Toolbar->AddControl(txtClt);
    m_Toolbar->AddControl(m_DatabaseCombo);

    wxArrayString sSelectionItemArray;
    Utility::ExtractSelectionItems(sSelectionItemArray,Settings.sDatbaseSelectionList);

    //Fill the list box with the selection items.
    for ( int index=0; index<sSelectionItemArray.GetCount(); index++ )
        m_DatabaseCombo->Append(sSelectionItemArray[index]);


    m_DatabaseCombo->SetSelection(0);

    //m_Toolbar->AddSeparator();
    m_Toolbar->AddTool(ID_HELP, wxT("Help"), bitmapHelp, wxT("Help."));







    m_Toolbar->Realize();
    this->SetToolBar(m_Toolbar);
    this->SetBackgroundColour(wxColour(70,70,130));


    // Connect Events
    //m_buttonCalculate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnButtonAction ), NULL, this );
    this->SetMenuBar( m_Menubar );

    m_Menubar->Show();
  
}

MainFrame::~MainFrame() = default;

void MainFrame::LoadGrid()
{
    bool bDeleteOldSizer = false;

    if(m_MainGrid != nullptr){
        bDeleteOldSizer = true;
        m_MainGrid->Destroy();
    }

    mainFormSizerForGrid = new wxBoxSizer( wxVERTICAL );
    //Create the spread sheet grid
    m_MainGrid = new DBGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, (unsigned)wxVSCROLL | (unsigned)wxFULL_REPAINT_ON_RESIZE);
    m_MainGrid->AddItem("Title","title","","");
    m_MainGrid->AddItem("Tablename","tablename","","");
    m_MainGrid->AddItem("Type of Table","tabletype","","");
    m_MainGrid->AddItem("Comments","comments","","");
    m_MainGrid->CreateFields();

    m_MainGrid->SetEventType(myEVT_MYEVENT);
    //Remove the horizonal scroll bar
    m_MainGrid->SetWindowStyleFlag( (unsigned)m_MainGrid->GetWindowStyle() &~ (unsigned)wxHSCROLL );

    //Add the spread sheet directly to the main form box grid..
    mainFormSizerForGrid->Add( m_MainGrid, 0, wxGROW, 0);
    //Set the sizer to be attached to the main form
    this->SetSizer( mainFormSizerForGrid , bDeleteOldSizer);

    m_MainGrid->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sUser,Settings.sPassword,SYS_TABLES, "sys_tablesId","");
    SetGridWhereCondition();
    if (Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest())
        m_MainGrid->LoadGridFromDatabase(true);//Check if the table exists before you load it
    else
        m_MainGrid->LoadGridFromDatabase(); //Load all tables.

    CheckIfTableDefinitionsMatchDatabaseTable();
    //Layout the grid.
    this->Layout();
}

wxString MainFrame::GetUserWhereCondition()
{
    // The system Developer and Administrator don't have an initial where condition, they can see all tables.
    if(!(Utility::IsSystemDatabaseDeveloper() || Utility::IsSystemDatabaseAdministrator())){

       return "tabletype <> 'system'";
    }
    return "";
}

void MainFrame::OnComboDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    iOldComboIndex = combo->GetSelection();
}
void MainFrame::OnComboChange( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString value = combo->GetStringSelection();

    //This the database doesn't exist, then we need to create it.
    if(!Utility::DoesDatabaseExist(value)){
        Utility::ExecuteQuery("CREATE DATABASE " + value);
    }

    //Check to see if the database was created.
    if(Utility::DoesDatabaseExist(value)){
        //Check to see if the system tables exist, of not, create them.
        if(!Utility::DoesTableExist(value,"sys_tables")){


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
                    "  `comments` varchar(255) DEFAULT NULL,"
                    "  PRIMARY KEY (`sys_tablesId`)"
                    ") ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=utf8;";


            if(!query.IsEmpty()){

                Utility::ExecuteQuery(query,value);
            }

        }
        if(!Utility::DoesTableExist(value,"sys_fields")){

            //We can load from a file or write in code here. I think it's better to write it code or have it in the sys_docs, much better I think.
            wxString query="";
            wxFile file("SQL/createsysfields.sql");
            file.ReadAll(&query,wxConvUTF8);

            if(!query.IsEmpty())
                Utility::ExecuteQuery(query,value);

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

        }

        if(!Utility::DoesTableExist(value,"sys_docs")){

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
                    "`TypeOfDoc` varchar(100) NOT NULL,"
                    "`Document` longtext,"
                    "PRIMARY KEY (`sys_docsId`)"
                    " ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;";

            if(!query.IsEmpty())
                Utility::ExecuteQuery(query,value);

        }

        Settings.sDatabase = value;
        LoadGrid();
    }
    else{
        combo->SetSelection(iOldComboIndex); // Restore the old selection because the database doesn't exit.
        wxLogMessage(MSG_DATABASE_DOES_NOT_EXIST);
    }
}


void MainFrame::CreateToolBars()
{


}
void MainFrame::NewFile(wxCommandEvent& WXUNUSED(event))
{

}

void MainFrame::OpenFile(wxCommandEvent& WXUNUSED(event))
{

}

void MainFrame::CloseFile(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Close File");
}

void MainFrame::SaveFile(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Save File");
}

void MainFrame::SaveFileAs(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Save File As");
}

void MainFrame::Quit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE); // Close the window
}

void MainFrame::OnbAddItem( wxCommandEvent& event )
{
     
        m_FormItem = new GenericItemForm((wxFrame*) this, -1,"Add Table",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP);
        
        //m_FormItem->SetDatabaseSettings(Settings.Database, Settings.Server, Settings.User, Settings.Password);
         //Define the database
         //Define the database
        m_FormItem->AddItem("Title","title","","","");
        m_FormItem->AddItem("Tablename","tablename","","","");
    m_FormItem->AddItem("Type of Table","tabletype","SELECTION{system;user;}","VARCHAR(255)","user");
         m_FormItem->AddItem("Comments","comments","MULTILINE","","");
        m_FormItem->SetUse("ADD");
        m_FormItem->CreateFields();
        
        m_FormItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sUser,Settings.sPassword,SYS_TABLES,"sys_tablesId");
        
        m_FormItem->Show(true);
        SetStatusText("Add Table.");
}

void MainFrame::OnbEditItem( wxCommandEvent& event )
{
           wxArrayInt rowsSelected = m_MainGrid->GetSelectedRows();
           int size = rowsSelected.size();
           
           if(size==1){
               int row = rowsSelected[0];
               OpenEditForm(row);
           }
}

void MainFrame::OpenEditForm(int sRow) {

    // wxPoint(100,100),
    // wxSize(500,410),

    m_FormItem = new GenericItemForm((wxFrame*) this, -1,"Edit Table",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP);

    //Define the database
    m_FormItem->AddItem("Title","title","","","");
    m_FormItem->AddItem("Tablename","tablename","","","");
    m_FormItem->AddItem("Type of Table","tabletype","SELECTION{system;user;}","VARCHAR(255)","user");
    m_FormItem->AddItem("Comments","comments","MULTILINE","","");

    m_FormItem->SetUse("UPDATE");
    m_FormItem->CreateFields();
    m_FormItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sUser,Settings.sPassword,SYS_TABLES,"sys_tablesId");



    wxString spasswordId = m_MainGrid->GetCellValue(sRow,0);
    SetStatusText(spasswordId);

    m_FormItem->SetID(spasswordId);
    m_FormItem->Show(true);
    m_FormItem->LoadFields();
    SetStatusText("Edit Item");
}



wxString MainFrame::GetSelectedRowTable()
{
    wxString tableName ="";
    
    wxArrayInt rowsSelected = m_MainGrid->GetSelectedRows();
    
    int size = rowsSelected.size();
    
    if(size==1){
        int row = rowsSelected[0];
        tableName = m_MainGrid->GetCellValue(row,2);
    }

    return tableName;
}

void MainFrame::OnbViewItem( wxCommandEvent& event )
{
    wxArrayInt rowsSelected = m_MainGrid->GetSelectedRows();
    int size = rowsSelected.size();

    if(size==1){
        wxString tableName ="";
        wxString tableId = "";
        int row = rowsSelected[0];
        tableName = m_MainGrid->GetCellValue(row,2);
        tableId << m_MainGrid->GetCellValue(row,0);


        OpenForm(tableName, tableId);
    }

}

void MainFrame::OpenForm(wxString sTableName,wxString sTableID)
{

    // We don't want the user to have more than one properties window open at once, it becomes very confusing if you do.
    // Also, you loose track of the pointer and the window doesn't get deleted properly when you close the application.
    if (m_TableForm!=nullptr)
        m_TableForm->Destroy();

    wxString FormName = "";


    //Before we view, we need to make sure that all the fields in the table are created or we will get an error.



    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        wxString QueryString="";
        QueryString << "select "
        << SYS_FIELDS << ".valtype, "
        << SYS_FIELDS << ".title, "
        << SYS_FIELDS << ".flags, "
        << SYS_FIELDS << ".valdefault, "
        << SYS_FIELDS << ".valextra, "
        << SYS_FIELDS << ".valkey, "
        << SYS_FIELDS << ".valfield, "
        << SYS_TABLES << ".title as FormName from " << SYS_FIELDS << " INNER JOIN "
        << SYS_TABLES << " ON " << SYS_FIELDS << ".sys_tablesId="
        << SYS_TABLES << ".sys_tablesId WHERE " << SYS_FIELDS << ".sys_tablesId="+sTableID+";";

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();


        // Display results
        if (res) {


            m_TableForm = new GenericTable((wxFrame*) this, -1,"Contacts",wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE);
            m_TableForm->SetTableDefinition(sTableName, sTableName, "","");// We will grab this from our form.
            //Add the field items

            // Get each row in result set, and print its contents
            for (size_t i = 0; i < res.num_rows(); ++i) {

                try {

                    wxString title(res[i]["title"], wxConvUTF8);
                    wxString flags(res[i]["flags"], wxConvUTF8);
                    wxString valfield(res[i]["valfield"], wxConvUTF8);
                    wxString valtype(res[i]["valtype"], wxConvUTF8);
                    wxString valDefault(res[i]["valdefault"], wxConvUTF8);
                    wxString valKey(res[i]["valkey"], wxConvUTF8);
                    wxString valExtra(res[i]["valextra"], wxConvUTF8);

                    wxString fn(res[i]["FormName"], wxConvUTF8);
                    FormName = fn;
                    m_TableForm->AddField(title,valfield,valtype,flags,valDefault,valKey,valExtra);
                }
                catch (int num) {

                    //f->SetStatusText("Database Connected - Row doesn't exist:");
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);
                    return;

                }
            }
            m_TableForm->SetLabel(FormName);

            if(!m_TableForm->Create()){// Create the table.
                return;

            }


            m_TableForm->Show(true);
        }
        else {
            //cerr << "Failed to get stock table: " << query.error() << endl;
            //return 1;
            //f->SetStatusText("Database Connected - Failed to get item list:");
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
            return;
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        //f->SetStatusText("Did not connect to database.");
        return;
    }


}

/*
select sys_fields.valtype, sys_fields.title, sys_fields.flags, sys_fields.valdefault, sys_fields.valextra, sys_fields.valkey, sys_fields.valfield, sys_tables.title as FormName from sys_fields INNER JOIN sys_tables ON sys_fields.sys_tablesId=sys_tables.sys_tablesId WHERE sys_fields.sys_tablesId=25;
*/
void MainFrame::OnbDeleteItem( wxCommandEvent& event )
{
    auto *dlg = new wxMessageDialog(nullptr, wxT("Are you sure you want to delete this table?"), wxT("Delete Table"), (unsigned)wxYES_NO | (unsigned)wxICON_EXCLAMATION);

    if ( dlg->ShowModal() == wxID_YES ){

        wxString queryString = PrepareDeleteQuery();
        ExecuteQuery(queryString);
        m_MainGrid->DeleteSelectedRow();
    }


    dlg->Destroy();
    m_MainGrid->ResizeSpreadSheet();
    this->Layout();
}
wxString MainFrame::PrepareDeleteQuery()
{

    wxString queryString;

    queryString = "DROP TABLE IF EXISTS `"+Settings.sDatabase+"`.`"+GetSelectedRowTable()+"`;";

    return queryString;
}


void MainFrame::ExecuteQuery(const wxString& queryString)
{
    //wxFrame *f = static_cast<wxFrame*>(this->GetParent());

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sUser);
    wxString pass(Settings.sPassword);



    try{
        // Connect to the sample database.
        Connection conn(false);

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {


            Query query = conn.query(queryString);
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

//CONTEXT MENU EVENT and REFRESH EVENTS
//We created an event to refresh the grid so we can call it from any frame class.
void MainFrame::OnMyEvent(MyEvent& event)
{

    if(event.m_bProperties){
        //We are going to show the field types for this table
        m_MainGrid->SetGridCursor(event.m_iRow,event.m_iCol);

        if (m_TableForm!= nullptr){
            m_TableForm->Destroy();
            m_TableForm = nullptr;
        }

        // Instead of opening contacts, we are going to open the generic form table.
        m_TableForm = new PropertyTable((wxFrame*) this, -1,event.m_sTableName +" fields",wxDefaultPosition,wxDefaultSize,(unsigned)wxDEFAULT_FRAME_STYLE);

        if (m_TableForm != nullptr){

            m_TableForm->SetGridTableName(event.m_sTableName);

            wxString linkID;
            linkID << event.m_lTableID;

            m_TableForm->SetTableDefinition(SYS_FIELDS, SYS_FIELDS, "All the fields of this table"," WHERE sys_tablesId="+linkID);// We will grab this from our form.
            //Add the field items
            m_TableForm->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sUser,Settings.sPassword);

            m_TableForm->AddField("Linking ID","sys_tablesId","int","HIDE-READONLY",linkID,"",""); // This is the linking ID
            m_TableForm->AddField("Field Name","valfield","varchar(100)","","","","");
            m_TableForm->AddField("Type","valtype","varchar(100)",MYSQL_TYPE_OPTIONS,"VARCHAR(255)","","");
            m_TableForm->AddField("Can be Null","valnull","varchar(3)","READONLY - SELECTION{YES;NO;}","YES","","");
            m_TableForm->AddField("Key","valkey","varchar(10)","","","","");
            m_TableForm->AddField("Default","valdefault","varchar(255)","","","","");
            m_TableForm->AddField("Extra","valextra","varchar(255)","","","","");
            m_TableForm->AddField("Title","title","varchar(100)","","","","");
            m_TableForm->AddField("Flags","flags","varchar(200)",FLAG_OPTIONS,"","",""); // FLAG_OPTIONS are in global.h
            m_TableForm->Create();// Create the table.
            m_TableForm->Show(true);

        }

    }
    else if(event.m_bOpen){
        wxString TableID;
        TableID<< event.m_lTableID;
        OpenForm(event.m_sTableName,TableID);
    }
    else if(event.m_bEdit){
        wxString TableID;
        TableID<< event.m_lTableID;
        OpenEditForm(event.m_iRow);
    }
    else if(event.m_bDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
        m_TableForm = nullptr;
        CheckIfTableDefinitionsMatchDatabaseTable(); //When we close the definition grid view, we need to reflect changes made to the definitions
    }
    else{
        // We are showing everything
        wxString userWhere = GetUserWhereCondition();
        if( !userWhere.IsEmpty()){


        }

        SetGridWhereCondition(event.m_sWhereCondition);
        if (Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest())
            m_MainGrid->LoadGridFromDatabase(true);//Check if the table exists before you load it
        else
            m_MainGrid->LoadGridFromDatabase(); //Load all tables.

        CheckIfTableDefinitionsMatchDatabaseTable();
        this->Layout();
    }
}


// This is where we blend the user where condition with the filter where condition
void MainFrame::SetGridWhereCondition(wxString whereToBlend)
{
    wxString userWhere = GetUserWhereCondition();
    wxString sWhereCon = "";

    if (whereToBlend.IsEmpty() && !userWhere.IsEmpty()){
        sWhereCon << " where " << userWhere;
        m_MainGrid->SetGridWhereCondition(sWhereCon); // Make sure you show all records.
    }else if(!whereToBlend.IsEmpty() && userWhere.IsEmpty()){
        sWhereCon << " where " << whereToBlend;
        m_MainGrid->SetGridWhereCondition(sWhereCon); // Make sure you show all records.
    }
    else if(!whereToBlend.IsEmpty() && !userWhere.IsEmpty()){
        sWhereCon << " where " << whereToBlend << " and " << userWhere;
        m_MainGrid->SetGridWhereCondition(sWhereCon); // Make sure you show all records.

    }
    else
        m_MainGrid->SetGridWhereCondition(""); // Make sure we clear it or once we select a filter, it will remain that way even after you show all.



}

//This is called on event refresh grid. It checks to see if table definitions match the actual table in the database.
//If something doesn't match, the database fieldname field text will turn red.
void MainFrame::CheckIfTableDefinitionsMatchDatabaseTable()
{
    wxString sCellValue;

    if(m_MainGrid->GetFirstRowCellValue(sCellValue,2)){
        wxString TableID = m_MainGrid->GetCurrentRowValue(0);
        CheckCellTableDefinitionsMatchDatabaseTable(sCellValue,TableID,m_MainGrid->GetCurrentRowIndex());
        while(m_MainGrid->GetNextRowCellValue(sCellValue)){
            TableID = m_MainGrid->GetCurrentRowValue(0);
            CheckCellTableDefinitionsMatchDatabaseTable(sCellValue,TableID,m_MainGrid->GetCurrentRowIndex());
        }
    }
}

//What we need to do now it check the database to see if table and fields exist.
//If they do, then mark the grid cell text as red.
void MainFrame::CheckCellTableDefinitionsMatchDatabaseTable(wxString sTable, wxString iTableId, int iRowIndex)
{
    if(!Utility::DoesTableExist(sTable)) {
        m_MainGrid->HighlightCell(iRowIndex, 2);
    }
    else{

        m_MainGrid->UnHighlightCell(iRowIndex, 2);
        wxArrayString fieldList;
        if(Utility::GetFieldList(fieldList,iTableId)){ // This is all the fields for that table from sys_fields

            //Now we have a list of all the table field names, we can check the database for the list
            for(int index=0;index<fieldList.GetCount();index++){

                if(!Utility::DoesFieldExitInTable(sTable,fieldList[index]))
                    m_MainGrid->HighlightCell(iRowIndex, 2);
            }
        }
    }
}

void MainFrame::OnbHelp( wxCommandEvent& event )
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(2); //
    m_HtmlWin->Show(true);

}


//We can send a message to the parent that this window is destroyed.
bool MainFrame::Destroy()
{
    //Safely destroy any window that is open so your program doesn't crash on close.

    //This is where we need to shut down any windows that are open.
    if (m_TableForm != nullptr)
        m_TableForm->Destroy();

    if (m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();


    //Always do this last
    bool bResult = wxFrame::Destroy();
    return bResult;
}



