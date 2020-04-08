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
#include <wx/checkbox.h>


#include "Utility.h"
#include "global.h"
#include "MyEvent.h"
#include "Generic/GenericItemForm.h"
#include "Generic/DBGrid.h"
#include "Generic/GenericTable.h"
#include "PropertyTable.h"
#include "CustomControls/myProgressControl.h"
#include "Dialog/ImportMySQLDatabase.h"
#include "TableDefDiagram/TableDiagramFrame.h"


//#include "ObTableDiagram.h"

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
    MENU_IMPORT,
    MENU_Close,
    MENU_Save,
    MENU_SaveAs,
    MENU_Quit,
    MENU_DELETE_DATABASE,
    ID_TOOL_ADD,
    ID_TOOL_EDIT,
    ID_TOOL_DELETE,
    ID_TOOL_VIEW,
    ID_TOOL_FILTER,
    ID_HELP,
    ID_AUTO_CHECK_DEFINITION,
    ID_OPEN_TABLE_DIAGRAM
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_ButtonAction, MainFrame::OnButtonAction)
    EVT_MENU(MENU_New, MainFrame::NewFile)
    EVT_MENU(MENU_IMPORT, MainFrame::OnImportDatabase)
    EVT_MENU(MENU_Close, MainFrame::CloseFile)
    EVT_MENU(MENU_Save, MainFrame::SaveFile)
    EVT_MENU(MENU_SaveAs, MainFrame::SaveFileAs)
    EVT_MENU(MENU_DELETE_DATABASE, MainFrame::OnDeleteCurrentDatabase)
    EVT_MENU(MENU_Quit, MainFrame::Quit)
    EVT_TOOL(ID_TOOL_ADD, MainFrame::OnbAddItem)
    EVT_TOOL(ID_TOOL_EDIT, MainFrame::OnbEditItem)
    EVT_TOOL(ID_TOOL_DELETE, MainFrame::OnbDeleteItem)
    EVT_TOOL(ID_TOOL_VIEW, MainFrame::OnbViewItem)
    EVT_TOOL(ID_TOOL_FILTER, MainFrame::OnbFilter)
    EVT_TOOL(ID_HELP, MainFrame::OnbHelp)
    EVT_TOOL(ID_AUTO_CHECK_DEFINITION, MainFrame::OnAutoCheckDefinitions)
    EVT_TOOL(ID_OPEN_TABLE_DIAGRAM, MainFrame::OnOpenTableDiagram)
    EVT_MYEVENT(MainFrame::OnMyEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);
///////////////////////////////////////////////////////////////////////////
//=============
// MyApp Class Methods
//

bool MyApp::OnInit()
{
    bProgress_loop_on = false;
    m_bImportingDatabase = false;
    m_bCheckTableDefinitions= false;
    m_iIdleStep=0;
    m_saTableIndex=wxNOT_FOUND;
    m_ProgressStep=0;
    m_ProgessCount=0;
    m_dataRecordIndex=0;
    m_sTableID="";
    m_saTables = nullptr;
    //m_Res = nullptr;

    bool bSettingLoaded= LoadAppSettings();
    
    m_MainFrame = new MainFrame((wxFrame*) nullptr,
                                   -1,
                                  wxT(MAIN_WIN_TITLE),
                                  wxPoint(MAIN_WIN_POSX,MAIN_WIN_POSY),
                                  wxSize((int)Settings.lMainWindowWidth,(int)Settings.lMainWindowHeight),
                                (unsigned)wxDEFAULT_FRAME_STYLE
                                  );
    
    m_MainFrame->Show(true);


    m_MainFrame->SetSettingsLoaded(bSettingLoaded);

    return true;
}

void MyApp::activateRenderLoop(bool on)
{
    if(on && !bProgress_loop_on)
    {
        Connect( wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle) );
        bProgress_loop_on = true;
    }
    else if(!on && bProgress_loop_on)
    {
        Disconnect( wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle) );
        bProgress_loop_on = false;
    }
}

bool MyApp::LoadAppSettings()
{
    wxString        str;
    wxString strExe = wxStandardPaths::Get().GetExecutablePath(); // Get the path to the images
    //wxLogMessage(strExe);
    strExe.Replace("DBWorks", "settings.ini"); //For mac and linux
    strExe.Replace("dbworks", "settings.ini"); //For mac and linux
    strExe.Replace("dbworks.exe", "settings.ini"); // For windows.



    //wxString strExe = "/Applications/DatabaseWorks/settings.ini";
    //wxString strExe = "settings.ini";
    // open the file
    wxTextFile tfile;
    if(tfile.Open(strExe)){
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
    else{
        //
        //There is nothing else to do here because the setting already are loaded with default values;
        return true;
    }

    return false;
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
        Settings.sDatabaseUser=value;
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
    else if (setting=="IMPORT_KEYS_HAVING_PRIMARY_FIELDS"){
        if(value=="NO")
            Settings.bImportPrimaryKey=false;
        else if (value=="YES")
            Settings.bImportPrimaryKey=true;
    }
    else if (setting=="IMPORT_CREATE_TABLES"){
        if(value=="NO")
            Settings.bImportCreateTables=false;
        else if (value=="YES")
            Settings.bImportCreateTables=true;
    }
    else if (setting=="IMPORT_DATA"){
        if(value=="NO")
            Settings.bImportData=false;
        else if (value=="YES")
            Settings.bImportData=true;
    }
    else if (setting=="AUTO_CHECK_DEFINITIONS"){
        if(value=="NO")
            Settings.bAutoCheckDefinitions=false;
        else if (value=="YES")
            Settings.bAutoCheckDefinitions=true;
    }
    else if (setting=="GRID_COLUMN_HEADING"){
        if(value=="TITLE")
            Settings.bShowGridColumnFields=false;
        else if (value=="FIELD")
            Settings.bShowGridColumnFields=true;
    }
    else if (setting=="ENFORCE_SELECTION_LINKED_TO_PRIMARY_KEY"){
        if(value=="YES")
            Settings.ENFORCE_SELECTION_LINKED_TO_PRIMARY_KEY=true;
        else if (value=="NO")
            Settings.ENFORCE_SELECTION_LINKED_TO_PRIMARY_KEY=false;
    }


}

void MyApp::StartImportDatabase(wxString sDatabase, wxString sNewDatabaseName) {
    m_DatabaseToImport=sDatabase;
    m_NewDatabaseNameToImportInto=sNewDatabaseName;
    m_saTableIndex=wxNOT_FOUND;


    m_iIdleStep=1;
    m_bImportingDatabase=true;
    activateRenderLoop(true);
}


void MyApp::onIdle(wxIdleEvent& evt)
{
    if(bProgress_loop_on) {

        //IMPORT DATABASE
        if (m_bImportingDatabase) {


            if (m_iIdleStep == 1) {


                //Turn off other processes running in this loop
                SetStateOfAutoCheckDefinitions(false);
                m_MainFrame->SetStateOfAutoCheckDefinitions(false);


                //Step 1: We need to read all the tables in the database and place them in a string array.
                // SHOW TABLES;
                // Example
                // +-----------------------+
                // | Tables_in_spreadsheet |
                // +-----------------------+
                // | books                 |
                //
                m_MainFrame->SetProgressLabel("Importing database: " + m_DatabaseToImport);

                auto *dlg = new wxMessageDialog(nullptr, wxT("Are you sure you want to import this database?"),
                                                wxT("Import Database"),
                                                (unsigned) wxYES_NO | (unsigned) wxICON_EXCLAMATION);

                if (dlg->ShowModal() == wxID_YES) {
                    m_saTables = new wxArrayString;
                    m_iIdleStep++;
                } else {
                    m_bImportingDatabase = false;
                    activateRenderLoop(false);//StopAllidle process.
                }

                dlg->Close(true);

            }
            else if (m_iIdleStep == 2)
            {

                //If we have a database name supplied, it will create a new database
                if (!m_NewDatabaseNameToImportInto.IsEmpty()) {

                    if(m_NewDatabaseNameToImportInto=="information" || m_NewDatabaseNameToImportInto=="dbworks" ){

                        wxLogMessage("You can't overwrite a system database, please choose another database name.");
                        m_iIdleStep=8;
                        return;
                    }

                    //If the database exists, drop it then create it again.
                    if (Utility::DoesDatabaseExist(m_NewDatabaseNameToImportInto))
                        Utility::ExecuteQuery("DROP DATABASE " + m_NewDatabaseNameToImportInto);

                    Utility::CreateDatabase(m_NewDatabaseNameToImportInto);


                    //Save the database name to the dbworks database to remember our imported and loaded databases
                    //The dbworks database only holds names at the moment.
                    Utility::SaveDatabaseToDBWorks(m_NewDatabaseNameToImportInto);
                }
                else
                    // If the user didn't supply a database name, then we are going to import all the tables into this database.
                    m_NewDatabaseNameToImportInto = Settings.sDatabase;

                if(m_NewDatabaseNameToImportInto=="information" || m_NewDatabaseNameToImportInto=="dbworks" ){

                    wxLogMessage("You can't overwrite a system database, please choose another database name.");
                    m_iIdleStep=8;
                    return;
                }

                if (!Utility::CreateSystemTables(m_NewDatabaseNameToImportInto)) {
                    wxLogMessage("Failed to create the system tables, see database administrator for help.");
                    m_iIdleStep=8;
                    return;
                }



                m_iIdleStep++;
            }
            else if (m_iIdleStep == 3)
            {

                //We only read all the table names here with the SHOW TABLES mysql command.
                Utility::LoadStringArrayWithMySQLTableNames(m_DatabaseToImport, *m_saTables);
                if(m_saTables->GetCount()==0){
                    wxLogMessage("There are no tables in this database, exiting.");
                    m_iIdleStep=8;
                    return;

                } else
                    m_saTableIndex = 0;




                m_iIdleStep++;

            } else if (m_iIdleStep == 4)
            {


                //We need to calculate the number of steps for the progess bar
                if (m_saTableIndex == 0){
                    m_ProgessCount=0;
                    m_ProgressStep=Utility::CalculateProgressStepsforImport(m_saTables->GetCount());
                }


                //Loop through all the tables. We need to read the tables from the database and fill th
                //for(int index=0; index < saTables->GetCount(); index++){

                //if(m_progressCount<100){
                //  m_progressCount+=progressStep;

                //m_MainFrame->UpdateProgressBar(m_progressCount);
                //}
                //Step 2: For each table, we need to read all the fields and,
                // DESCRIBE tablename;
                /*Example
                +----------+--------------+------+-----+---------+-------+
                | Field    | Type         | Null | Key | Default | Extra |
                +----------+--------------+------+-----+---------+-------+
                | title    | varchar(50)  | NO   |     | NULL    |       |
                | price    | int          | NO   |     | NULL    |       |
                | language | varchar(100) | YES  |     | ENGLISH |       |
                | author   | varchar(200) | NO   |     | NULL    |       |
                | comments | varchar(900) | YES  |     | NULL    |       |
                +----------+--------------+------+-----+---------+-------+
                */

                //This will call the SQL Describe method and load the tableFieldItemArray with table fields.
                ArrayTableFields tableFieldItemArray;
                Utility::LoadFieldArrayWithTableFields(m_DatabaseToImport, m_saTables->Item(m_saTableIndex),
                                                       tableFieldItemArray);


                // Step 3: Add table to sys_tables and save the table ID.
                //Step 4: Similar to PropertyTable::PrepareCreateQuery() but instead of reading from the grid and creating the table, we are reading from the table and creating table definition
                // in sys_fields
                //THIS IS THE WRONG FUNCTION TO CALL.
                //NOTE, We want to create table definition in the new database or existing database if new database string is empty.
                m_MainFrame->SetProgressLabel("Creating Table Def: " + m_saTables->Item(m_saTableIndex));
                m_MainFrame->CreateTableDefinitions(m_NewDatabaseNameToImportInto, m_saTables->Item(m_saTableIndex),
                                                    tableFieldItemArray);

                if (Settings.bImportCreateTables) {

                    m_MainFrame->SetProgressLabel("Creating Table: " + m_saTables->Item(m_saTableIndex));
                    //This is where we create the actual tables in the database
                    Utility::CreateTable(m_NewDatabaseNameToImportInto, m_saTables->Item(m_saTableIndex),
                                         tableFieldItemArray);
                }
                //if(Settings.bImportCreateTables && Settings.bImportData){
                //Now the table exist in the database, we can import all the data.


                UpdateProgressBar();
                m_saTableIndex++;


                if (m_saTableIndex == m_saTables->GetCount()) {
                    m_iIdleStep++;
                    m_saTableIndex = 0;// reset it, but it will be reset when we start
                }
            }else if (m_iIdleStep == 5)
            {
                //Load all the data
                // We need to run through all the tables again and fill them with data
                if (Settings.bImportCreateTables && Settings.bImportData) {


                            //If you are importing a dbworks created database, don't load data into the system tables, or you will get insane results and corrupt the data.
                            if (m_saTables->Item(m_saTableIndex) == SYS_FIELDS || m_saTables->Item(m_saTableIndex) ==SYS_TABLES) {
                                m_iIdleStep = 8;
                                return;
                            }

                           if(m_dataRecordIndex==0){

                               m_ProgessCount=0;
                               m_ProgressStep=Utility::CalculateProgressStepsforImport(m_saTables->GetCount());

                               m_tableFieldItemArray.Clear();
                               Utility::LoadFieldArrayWithTableFields(m_DatabaseToImport, m_saTables->Item(m_saTableIndex),m_tableFieldItemArray);

                                wxString database(m_DatabaseToImport);
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
                                    wxString QueryString = "SELECT * FROM " + m_saTables->Item(m_saTableIndex) + ";";
                                    Query query = conn.query(QueryString);
                                    m_Res = query.store();

                                    if(!m_Res){

                                        //If we going to increment the table index, we need to check that it hasn't reached the end.
                                        m_saTableIndex++;
                                        if (m_saTableIndex == m_saTables->GetCount()) {
                                            m_iIdleStep = 8;
                                            return;
                                        } else
                                            return;
                                    }
                                    //If there are not rows of data, move onto the next
                                    if(m_Res.num_rows()<1){
                                        m_saTableIndex++;
                                        if (m_saTableIndex == m_saTables->GetCount()) {
                                            m_iIdleStep = 8;
                                            return;
                                        } else
                                            return;
                                    }
                                    if(m_Res){
                                        m_ProgressStep = Utility::CalculateProgressStepsforImport(m_Res.num_rows());
                                        m_MainFrame->GetProgressControlPointer()->SetValue(0);
                                        m_MainFrame->SetProgressLabel("Import data for: " + m_saTables->Item(m_saTableIndex));


                                    }

                                  //  m_dataRecordIndex

                                }
                                else{
                                    //We have a bigger problem, get out of here
                                    m_iIdleStep = 8;
                                    return;
                                }
                           }//END OF INIT EACH TABLE before we get all the records


                            //Get Each data Records and write it to the database table
                            // Utility::LoadNextTableData(m_DatabaseToImport,m_NewDatabaseNameToImportInto, m_saTables->Item(m_saTableIndex),m_dataRecordIndex,tableFieldItemArray);

                            wxString queryString;
                            int count = m_tableFieldItemArray.GetCount();

                            if (count > 0 && m_Res) {

                                queryString = "INSERT INTO " + m_saTables->Item(m_saTableIndex) + " (";

                                for (int i = 0; i < count; i++) {
                                    if (i == count - 1)
                                        queryString = queryString + m_tableFieldItemArray[i].fieldName;
                                    else
                                        queryString = queryString + m_tableFieldItemArray[i].fieldName + ",";
                                }

                                queryString = queryString + ") VALUES (";

                                for (int i = 0; i < count; i++) {

                                    int indexRecord=m_dataRecordIndex;

                                    if(indexRecord<m_Res.num_rows()){
                                        wxString sValue(m_Res[indexRecord][m_tableFieldItemArray[i].fieldName], wxConvUTF8);

                                        sValue = Utility::Escape(sValue);

                                        if (m_tableFieldItemArray[i].fieldType == "int") {
                                            if (i == count - 1)
                                                queryString = queryString + sValue + ")";
                                            else
                                                queryString = queryString + sValue + ",";

                                        } else {
                                            if (i == count - 1)
                                                queryString = queryString + "'" + sValue + "')";
                                            else
                                                queryString = queryString + "'" + sValue + "',";
                                        }
                                    }
                                }
                            }

                            Utility::ExecuteQuery(m_NewDatabaseNameToImportInto, queryString);

                            m_dataRecordIndex++;
                            m_ProgessCount+=m_ProgressStep;
                            m_MainFrame->GetProgressControlPointer()->SetValue(m_ProgessCount);

                            //Don't Update here, will do so inside funciton
                            //UpdateProgressBar();
                            int Rowcount=m_Res.num_rows();
                            if(m_dataRecordIndex == Rowcount){


                                m_saTableIndex++;
                                m_tableFieldItemArray.Clear();

                                if (m_saTableIndex < m_saTables->GetCount())
                                    Utility::LoadFieldArrayWithTableFields(m_DatabaseToImport, m_saTables->Item(m_saTableIndex),m_tableFieldItemArray);



                                m_dataRecordIndex=0;
                            }


                            if (m_saTableIndex == m_saTables->GetCount()) {
                                m_iIdleStep++;
                                m_saTableIndex = 0;// reset it, but it will be reset when we start
                            }

                }
                else
                    m_iIdleStep++;

            }
            else if (m_iIdleStep == 6)
            {

                //Step 5: Place the database in the dropdown selection list and load the database tables.
                //Note: The user might import into an existing name, we don't want duplicates here.
                if (!Utility::DoesSelectionExistInCombobox(m_MainFrame->GetDatabaseComboBox(),
                                                           m_NewDatabaseNameToImportInto))
                    m_MainFrame->GetDatabaseComboBox()->Append(m_NewDatabaseNameToImportInto); // Append the imported database to the combo box NOTE We still need a place to save our database.

                m_iIdleStep++;
            } else if (m_iIdleStep == 7) {

                //Set the selected database on the toolbar combo and then refresh the grid.
                m_MainFrame->GetDatabaseComboBox()->SetStringSelection(m_NewDatabaseNameToImportInto);
                Settings.sDatabase = m_NewDatabaseNameToImportInto;
                m_MainFrame->Refresh(true);


                m_iIdleStep++;
            } else if (m_iIdleStep == 8) {


                if (m_saTables == nullptr) {
                    delete m_saTables;
                    m_saTables = nullptr;
                }
                m_iIdleStep = 0;
                m_dataRecordIndex=0;
                m_bImportingDatabase = false;
                activateRenderLoop(false);//StopAllidle process.
                m_MainFrame->UpdateProgressBar(100);
                m_MainFrame->SetProgressLabel("Import Ended");
                //wxLogMessage("Import Ended");
            }

        }
        // CHECK DEFINITION
        else if (m_bCheckTableDefinitions) {


            if (m_iIdleStep == 1) {
                m_MainFrame->SetProgressLabel("Checking Definitions");
                m_ProgessCount=0;
                m_ProgressStep=Utility::CalculateProgressStepsforImport(m_MainFrame->GetMainGrid()->GetRows());

                m_iIdleStep++;
                UpdateProgressBar();
            } else if (m_iIdleStep == 2) {

                if (m_MainFrame->GetMainGrid()->GetFirstRowCellValue(m_sCellValue, 2)) {
                    m_sTableID = m_MainFrame->GetMainGrid()->GetCurrentRowValue(0);

                    CheckCellTableDefinitionsMatchDatabaseTable(m_sCellValue, m_sTableID,
                                                                m_MainFrame->GetMainGrid()->GetCurrentRowIndex());
                    //   while(m_MainFrame->GetMainGrid()->GetNextRowCellValue(m_sCellValue)){
                    //      TableID = m_MainFrame->GetMainGrid()->GetCurrentRowValue(0);
                    //      CheckCellTableDefinitionsMatchDatabaseTable(m_sCellValue,TableID,m_MainFrame->GetMainGrid()->GetCurrentRowIndex());
                    //  }
                    //        if(m_MainFrame->GetMainGrid()->GetNextRowCellValue(m_sCellValue)){
                    //       TableID = m_MainFrame->GetMainGrid()->GetCurrentRowValue(0);
                    //   CheckCellTableDefinitionsMatchDatabaseTable(m_sCellValue,TableID,m_MainFrame->GetMainGrid()->GetCurrentRowIndex());
                    //}
                    //else
                    m_iIdleStep++;

                } else {
                    m_iIdleStep = 0;
                    m_sTableID = "";
                    m_bCheckTableDefinitions = false;
                    activateRenderLoop(false);//StopAllidle process.
                    m_MainFrame->UpdateProgressBar(100);
                    //m_MainFrame->SetProgressLabel("Checking Table Definitions Ended.");
                }

                UpdateProgressBar();
            } else if (m_iIdleStep == 3) {

                if (m_MainFrame->GetMainGrid()->GetNextRowCellValue(m_sCellValue)) {
                    m_sTableID = m_MainFrame->GetMainGrid()->GetCurrentRowValue(0);
                    CheckCellTableDefinitionsMatchDatabaseTable(m_sCellValue, m_sTableID,
                                                                m_MainFrame->GetMainGrid()->GetCurrentRowIndex());
                } else
                    m_iIdleStep++;

                UpdateProgressBar();
            } else if (m_iIdleStep == 4) {

                m_iIdleStep++;
                UpdateProgressBar();
            } else if (m_iIdleStep == 5) {

                m_iIdleStep++;
                UpdateProgressBar();
            } else if (m_iIdleStep == 6) {
                m_iIdleStep = 0;
                m_sTableID = "";
                m_bCheckTableDefinitions = false;
                activateRenderLoop(false);//StopAllidle process.
                m_MainFrame->UpdateProgressBar(100);
                m_MainFrame->SetProgressLabel("Checking Table Definitions Ended.");
                m_MainFrame->GetMainGrid()->ResizeSpreadSheet();
            }


        }


        evt.RequestMore(); // render continuously, not only once on idle
    }
}

void MyApp::StartCheckIfTableDefinitionsMatchDatabaseTable()
{
    m_sTableID="";
    m_iIdleStep=1;
    m_bCheckTableDefinitions=true;
    activateRenderLoop(true);
    m_sCellValue="";
}

void MyApp::StopCheckIfTableDefinitionsMatchDatabaseTable()
{
    m_sTableID="";
    m_iIdleStep=1;
    m_bCheckTableDefinitions=false;
    activateRenderLoop(false);
    m_sCellValue="";
    m_MainFrame->UpdateProgressBar(0);
}

//What we need to do now it check the database to see if table and fields exist.
//If they do, then mark the grid cell text as red.
void MyApp::CheckCellTableDefinitionsMatchDatabaseTable(const wxString& sTable, const wxString& iTableId, int iRowIndex)
{
    if(!Utility::DoesTableExist(Settings.sDatabase,sTable)) {
        m_MainFrame->GetMainGrid()->HighlightCell(iRowIndex, 2);
    }
    else{

        m_MainFrame->GetMainGrid()->UnHighlightCell(iRowIndex, 2);
        ArrayTableFields fieldList;// We need to get the field array.
        if(Utility::GetFieldList(fieldList,iTableId)){ // This is all the fields for that table from sys_fields

            //Now we have a list of all the table field names, we can check the database for the list
            //for(int index=0;index<fieldList.GetCount();index++){

                //This will match the field name, type null, key, default and extra values to see if they match the mySQL database
                if(!Utility::DoesFieldExitInTable(sTable,fieldList))
                    m_MainFrame->GetMainGrid()->HighlightCell(iRowIndex, 2);
            //}
        }
    }
}


void MyApp::UpdateProgressBar()
{
    m_ProgessCount+=m_ProgressStep;
    m_MainFrame->UpdateProgressBar(m_ProgessCount);
}

//=============
// MyFrame Class Methods
//
void MainFrame::OnButtonAction( wxCommandEvent& event )
{

    SetStatusText(_("Status Text"));
    
    
}

wxComboBox * MainFrame::GetDatabaseComboBox()
{
    return m_DatabaseCombo;
}
DBGrid* MainFrame::GetMainGrid()
{
    return m_MainGrid;

}
void MainFrame::SetSettingsLoaded(bool bSettingsLoadedFlag)
{
    m_bSettingsLoaded=bSettingsLoadedFlag;

}




MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_TableForm = nullptr;
    m_HtmlWin = nullptr;
    m_pTableDiagaram = nullptr;
    m_pFilters = nullptr;
    m_MainGrid = nullptr;
    m_StatusBar = nullptr;
    m_Menubar= nullptr;
    m_FileMenu= nullptr;
    m_DatabaseCombo= nullptr;
    m_UserGroupCombo= nullptr;
    m_txtCltUserGroup = nullptr;
    //m_ImportMySQLForm = nullptr;
    m_txtCltCheckTableTxt = nullptr;
    m_AutoCheckDefinitionsCheckCtl = nullptr;
    m_ProgressGauge = nullptr;
    m_txtCltProgressBar = nullptr;
    //m_TableDiagramPanel = nullptr;


    bool b_DatabaseDeveloper=false;
    m_sCurrentStoredWhereCondition="";


    iOldComboIndex=0;
    m_sDefaultUsergroupFilterCondition="";

    //The dbworks database is where we store all our database names.
    //This database is a place to store system information that you don't want installed inside user databases.
    //Most system information is stored inside the user databases in sys_tables sys_fields and sys_docs.
    if(!Utility::DoesDatabaseExist("dbworks")){

        Utility::CreateDatabase("dbworks");
        //Check to see if we have a system database, if not create it.

        if(!Utility::DoesTableExist("dbworks","sys_databases")){

            //We can load from a file or write in code here. I think it's better to write it code or have it in the sys_docs, much better I think.
            wxString query="";

            //Option 3 DIRECTLY IN CODE. I think this is the best
            query = "CREATE TABLE `sys_databases` ("
                    "`sys_databasesId` int NOT NULL AUTO_INCREMENT,"
                    "`databasename` varchar(200) NOT NULL,"
                    "`comments` longtext,"
                    "PRIMARY KEY (`sys_databasesId`)"
                    " ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;";

            if(!query.IsEmpty())
                Utility::ExecuteQuery("dbworks",query);

        }

    }

    /*
    wxMenuBar * m_FrameMenuBar = new wxMenuBar;
    wxMenu* m_FrameMenu = new wxMenu();
    m_FrameMenu->Append(1, ("E&xit"));
    m_FrameMenuBar->Append(m_FrameMenu,"&Edit");
    SetMenuBar(m_FrameMenuBar);*/

    //Let's check if the information database exists, if not, create it.
    if(!CheckCreateDatabase(Settings.sDatabase)){
        wxLogMessage(MSG_FAILED_TO_CREATE_DATABASE);
        this->Close();
        return;
    }

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    //Create the main menubar
      //Create the main menu
      m_Menubar = new wxMenuBar( 0 );

      m_FileMenu = new wxMenu();


    if (Utility::IsSystemDatabaseDeveloper() )
        m_FileMenu->Append(MENU_IMPORT,
                       _("&Import a MySQL Database"), _("Import an existing file"));
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

    m_FileMenu->Append(MENU_DELETE_DATABASE,
                       _("&Delete Current Database"), _("Delete the current selected database."));
        m_FileMenu->AppendSeparator();

      m_FileMenu->Append(MENU_Quit,
          _("&Quit"), _("Quit the editor"));

      m_Menubar->Append(m_FileMenu, _("&File"));

    
    //this->Centre( wxBOTH );
    m_StatusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

    //======
    // Setup the toolbar
    m_Toolbar1 = this->CreateToolBar();




    wxInitAllImageHandlers(); //You need to call this or the images will not load.

    wxBitmap BitMap;


    Utility::LoadBitmap(BitMap,"help.png");
    m_Toolbar1->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));



    if (Utility::IsSystemDatabaseDeveloper() ) {

        b_DatabaseDeveloper = true;

        Utility::LoadBitmap(BitMap,"add.png");
        m_Toolbar1->AddTool(ID_TOOL_ADD, wxT("Add a new table to the database."), BitMap,
                           wxT("Add a new table to the database."));

        Utility::LoadBitmap(BitMap,"edit.png");
        m_Toolbar1->AddTool(ID_TOOL_EDIT, wxT("Edit the selected table."), BitMap, wxT("Edit the selected table."));


        Utility::LoadBitmap(BitMap,"delete.png");
        m_Toolbar1->AddTool(ID_TOOL_DELETE, wxT("Delete the selected table, will also delete all data."), BitMap,
                           wxT("Delete the selected table, will also delete all data."));


        Utility::LoadBitmap(BitMap,"view.png");
        m_Toolbar1->AddTool(ID_TOOL_VIEW, wxT("View the selected table."), BitMap, wxT("View the selected table."));

    }
    else if (Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest()){
        b_DatabaseDeveloper=false;

        Utility::LoadBitmap(BitMap,"view.png");
        m_Toolbar1->AddTool(ID_TOOL_VIEW, wxT("View the selected table."), BitMap, wxT("View the selected table."));
    }

    wxStaticText * txtCltDatabase = new wxStaticText( m_Toolbar1, wxID_ANY, "Database ", wxDefaultPosition, wxDefaultSize, 0 );


    m_DatabaseCombo = new wxComboBox( m_Toolbar1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,wxCB_READONLY);
    m_DatabaseCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( MainFrame::OnDatabaseComboChange ), nullptr, this );
    m_DatabaseCombo->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( MainFrame::OnDatabaseComboDropDown ), nullptr, this );


    m_Toolbar1->AddControl(txtCltDatabase);
    m_Toolbar1->AddControl(m_DatabaseCombo);




    LoadDatabaseCombo();




    m_DatabaseCombo->SetStringSelection(Settings.sDatabase);

    m_txtCltUserGroup = new wxStaticText( m_Toolbar1, wxID_ANY, Settings.sUsergroup, wxDefaultPosition, wxDefaultSize, 0 );

    if (Utility::IsSystemDatabaseDeveloper() || Utility::IsSystemDatabaseAdministrator()  ) {
        m_txtCltUserGroup->SetLabel("Usergroup");
        m_UserGroupCombo = new wxComboBox( m_Toolbar1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,wxCB_READONLY);
        m_UserGroupCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( MainFrame::OnUserGroupComboChange ), nullptr, this );
        m_UserGroupCombo->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( MainFrame::OnUserGroupComboDropDown ), nullptr, this );
        m_UserGroupCombo->SetSize(260,-1);

        wxArrayString sUserGroupSelectionItemArray;
        Utility::ExtractSelectionItems(sUserGroupSelectionItemArray,"SELECTION{SYSTEM_DATABASE_DEVELOPER;SYSTEM_DATABASE_ADMINISTRATOR;ADVANCED_USER;STANDARD_USER;GUEST;}");

        //Fill the list box with the selection items.
        for ( int index=0; index<sUserGroupSelectionItemArray.GetCount(); index++ )
            m_UserGroupCombo->Append(sUserGroupSelectionItemArray[index]);

        m_Toolbar1->AddControl(m_txtCltUserGroup);
        m_Toolbar1->AddControl(m_UserGroupCombo);

        m_UserGroupCombo->SetStringSelection(Settings.sUsergroup);

        Utility::LoadBitmap(BitMap,"filter.png");
        m_Toolbar1->AddTool(ID_TOOL_FILTER, wxT("User Filters."), BitMap, wxT("Define filters for your tables."));

        //Create the checkbox for auto check definitions to tables
        if(Utility::IsSystemDatabaseDeveloper()){

            Utility::LoadBitmap(BitMap,"tableDiagram.png");
            m_Toolbar1->AddTool(ID_OPEN_TABLE_DIAGRAM, wxT("Open table definitions diagram."), BitMap, wxT("Open table definitions diagram."));

            m_txtCltCheckTableTxt = new wxStaticText( m_Toolbar1, wxID_ANY, Settings.sUsergroup, wxDefaultPosition, wxDefaultSize, 0 );
            m_txtCltCheckTableTxt->SetLabel("Auto check definitions");
            m_Toolbar1->AddControl(m_txtCltCheckTableTxt);

            m_AutoCheckDefinitionsCheckCtl = new wxCheckBox( m_Toolbar1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            m_Toolbar1->AddControl(m_AutoCheckDefinitionsCheckCtl);
            m_AutoCheckDefinitionsCheckCtl->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( MainFrame::OnAutoCheckDefinitions ), nullptr, this );

            m_ProgressGauge = new myProgressControl(m_Toolbar1,wxID_ANY,100);
            m_ProgressGauge->DoGetBestSize();

            if(Settings.bAutoCheckDefinitions)
                m_AutoCheckDefinitionsCheckCtl->SetValue(true);

            m_Toolbar1->AddControl(m_ProgressGauge);

        }
        //Add a checkbox to enable Refresh
    }





    m_Toolbar1->Realize();

    mainFormSizerForGrid = new wxBoxSizer( wxVERTICAL );
    this->SetToolBar(m_Toolbar1);

    this->SetBackgroundColour(wxColour(70,70,130));




    //


    // Connect Events
    //m_buttonCalculate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnButtonAction ), NULL, this );
  SetMenuBar( m_Menubar );

  //  wxMenuBar* pMenuBar = GetMenuBar();
  //  pMenuBar->SetParent(this);
  //  pMenuBar->SetFocus();

    //Load the grid
    LoadGrid();

    //Loads all the DrawTableObjects and draws them to the screen.
    LoadTableObjects();

    if(!b_DatabaseDeveloper){

        // For standard users, we don't want to see ID and Table name
        if(Utility::IsStandardUser() || Utility::IsGuest())
        {
            m_MainGrid->HideColumn(0);
            m_MainGrid->HideColumn(2);
            m_MainGrid->HideColumn(3);

        } else
            m_MainGrid->HideColumn(2); // For advanced user, we don't want to see the tablename, just show the ID

    }


    m_Menubar->Show();
}


//Loads all the DrawTable Objects and draws them to the screen.
void MainFrame::LoadTableObjects()
{
    if(m_pTableDiagaram!= nullptr)
        m_pTableDiagaram->LoadTableObjects(Settings.sDatabase);
}

void MainFrame::LoadDatabaseCombo(){

    //Remove all the items
    if(m_DatabaseCombo->GetCount()>0){
        m_DatabaseCombo->Clear();
    }

    wxArrayString sDatabaseSelectionItemArray;
    wxString databasesToSelect = "SELECTION{" + Settings.sDatabase +";" + Settings.sDatbaseSelectionList + "}";
    Utility::ExtractSelectionItems(sDatabaseSelectionItemArray,databasesToSelect);

    //Now we want to append database from the dbworks - sys_databases table
    Utility::AppendDBWorksDatabases(sDatabaseSelectionItemArray);
    //Fill the list box with the selection items.
    for ( int index=0; index<sDatabaseSelectionItemArray.GetCount(); index++ ){
        if(!Utility::DoesSelectionExistInCombobox(m_DatabaseCombo,sDatabaseSelectionItemArray[index]))
            m_DatabaseCombo->Append(sDatabaseSelectionItemArray[index]);
    }
}





void MainFrame::SetProgressLabel(wxString sLabel)
{
    if(m_ProgressGauge!=nullptr)
        m_ProgressGauge->SetLabel(sLabel);
}
//You only need to do this on the main window, show and hide different windows.
//NOTE, this function needs to be modified to reflect what visibility we give in the mainframe constructor.
void MainFrame::SetUsergroupWindowVisibility() {

    if (Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest()) {

        if(Utility::IsStandardUser() || Utility::IsGuest())
        {
            m_MainGrid->HideColumn(0);
            m_MainGrid->HideColumn(2);
            m_MainGrid->HideColumn(3);


          //  m_Toolbar->RemoveTool(ID_TOOL_ADD);
          //  m_Toolbar->RemoveTool(ID_TOOL_EDIT);
          //  m_Toolbar->RemoveTool(ID_TOOL_DELETE);


        } else
            m_MainGrid->HideColumn(2); // For advanced user, we don't want to see the tablename, just show the ID

    }

    if (Utility::IsSystemDatabaseDeveloper() || Utility::IsSystemDatabaseAdministrator()  ) {



        m_MainGrid->SetColumnWidth(0,100); // Set the width to 1 so it's not hidden then the refresh function will correct the width.
        m_MainGrid->SetColumnWidth(2,100); // Set the width to 1 so it's not hidden then the refresh function will correct the width.
        m_MainGrid->SetColumnWidth(3,100); // Set the width to 1 so it's not hidden then the refresh function will correct the width.


        //m_Toolbar1->Realize();
    }

    Refresh();
}

void MainFrame::PopulateToolbar()
{
   // m_Toolbar->RemoveTool(ID_TOOL_ADD,bt);
   // m_Toolbar->RemoveTool(ID_TOOL_EDIT,bt);
   // m_Toolbar->RemoveTool(ID_TOOL_DELETE,bt);


}

MainFrame::~MainFrame() = default;

myProgressControl * MainFrame::GetProgressControlPointer()
{
    return m_ProgressGauge;
}


void MainFrame::LoadGrid()
{
    bool bDeleteOldSizer = false;

//    if(m_TableDiagramPanel!= nullptr){

   //     m_TableDiagramPanel->Destroy();
  //      m_TableDiagramPanel= nullptr;
 //   }

    if(m_MainGrid != nullptr){

        //mainFormSizerForGrid->Remove(3);

        bDeleteOldSizer = true;
        m_MainGrid->Destroy();
        m_MainGrid = nullptr;
    }


    //Create the spread sheet grid
    m_MainGrid = new DBGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, (unsigned)wxVSCROLL | (unsigned)wxFULL_REPAINT_ON_RESIZE);
    m_MainGrid->AddItem("Title","title","","","","","","");
    m_MainGrid->AddItem("Tablename *","tablename","","","","","","");
    m_MainGrid->AddItem("Type of Table","tabletype","","","","","","");
    m_MainGrid->AddItem("Comments","comments","","","","","","");
    m_MainGrid->CreateFields();

    m_MainGrid->SetEventType(myEVT_MYEVENT);
    //Remove the horizonal scroll bar
    m_MainGrid->SetWindowStyleFlag( (unsigned)m_MainGrid->GetWindowStyle() &~ (unsigned)wxHSCROLL );

    //Add the spread sheet directly to the main form box grid..
    mainFormSizerForGrid->Add( m_MainGrid, 0, wxGROW, 0);

    //Create a drawing pane under the grid.
//    m_TableDiagramPanel = new ObTableDiagram(this); //This are our drawing functions.
//    mainFormSizerForGrid->Add( m_TableDiagramPanel, 1, wxGROW, 0);


    //Set the sizer to be attached to the main form
    this->SetSizer( mainFormSizerForGrid , bDeleteOldSizer);

    m_MainGrid->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,SYS_TABLES, "sys_tablesId","");


    SetGridWhereCondition();
    Refresh();
}

wxString MainFrame::GetUserWhereCondition()
{
    // The system Developer and Administrator don't have an initial where condition, they can see all tables.
    if(!(Utility::IsSystemDatabaseDeveloper() || Utility::IsSystemDatabaseAdministrator())){

       return "tabletype <> 'system'";
    }
    return "";
}

void MainFrame::OnUserGroupComboChange( wxCommandEvent& event ) {
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    Settings.sUsergroup = combo->GetStringSelection();
    SetGridWhereCondition();
    SetUsergroupWindowVisibility();
}

void MainFrame::OnUserGroupComboDropDown( wxCommandEvent& event )
{

}

void MainFrame::OnDatabaseComboDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    iOldComboIndex = combo->GetSelection();
    UpdateProgressBar(0);
    SetProgressLabel("");

}

void MainFrame::OnDatabaseComboChange( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString sDatabase = combo->GetStringSelection();
    if(!CheckCreateDatabase(sDatabase)){
        combo->SetSelection(iOldComboIndex); // Restore the old selection because the database doesn't exit.
        wxLogMessage(MSG_DATABASE_DOES_NOT_EXIST);
    }
    else{
        Settings.sDatabase = sDatabase;
        LoadGrid();
        //Loads all the DrawTableObjects and draws them to the screen.
        LoadTableObjects();
        SetUsergroupWindowVisibility();//Hide columns for none database developers
    }
}

bool MainFrame::CheckCreateDatabase(wxString sDatabase) {

    //This the database doesn't exist, then we need to create it.
    if(!Utility::DoesDatabaseExist(sDatabase))
        Utility::CreateDatabase(sDatabase);

    return Utility::CreateSystemTables(sDatabase);

}

void MainFrame::CreateToolBars()
{


}
void MainFrame::NewFile(wxCommandEvent& WXUNUSED(event))
{

}
void MyApp::SetStateOfAutoCheckDefinitions(bool bOnIsTrue){
    Settings.bAutoCheckDefinitions=bOnIsTrue;
    m_bCheckTableDefinitions=bOnIsTrue;
}
void MainFrame::SetStateOfAutoCheckDefinitions(bool bOnIsTrue){

    m_AutoCheckDefinitionsCheckCtl->SetValue(bOnIsTrue);
    Settings.bAutoCheckDefinitions=bOnIsTrue;
}
void MainFrame::OnAutoCheckDefinitions(wxCommandEvent& event)
{
    if(m_AutoCheckDefinitionsCheckCtl->GetValue()){
        Settings.bAutoCheckDefinitions=true;
        wxGetApp().StartCheckIfTableDefinitionsMatchDatabaseTable();
        //CheckIfTableDefinitionsMatchDatabaseTable();
        //Refresh();
    }
    else{
        Settings.bAutoCheckDefinitions=false;
        wxGetApp().StopCheckIfTableDefinitionsMatchDatabaseTable();
        //We are going to keep the tables red.
    }
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

void MainFrame::OnbAddItem( wxCommandEvent& event ) {

    if (!Utility::IsSystemDatabaseDeveloper()) {
        wxLogMessage(MSG_ONLY_DATABASE_DEVELOPERS);
    } else {

        m_FormItem = new GenericItemForm((wxFrame *) this, -1, "Add Table", wxDefaultPosition, wxDefaultSize,(unsigned) wxCAPTION | (unsigned) wxSTAY_ON_TOP);

        //m_FormItem->SetDatabaseSettings(Settings.Database, Settings.Server, Settings.User, Settings.Password);
        //Define the database
        //Define the database
        m_FormItem->AddItem("Title", "title", "", "", "","","","");
        m_FormItem->AddItem("Tablename", "tablename", "", "", "","","","");
        m_FormItem->AddItem("Type of Table", "tabletype", "SELECTION{system;user;development;}", "VARCHAR(255)", "user","","","");
        m_FormItem->AddItem("Comments", "comments", "MULTILINE", "", "","","","");
        m_FormItem->SetUse("ADD");
        m_FormItem->CreateFields();

        m_FormItem->SetSettings(Settings.sDatabase, Settings.sServer, Settings.sDatabaseUser, Settings.sPassword, SYS_TABLES, "sys_tablesId");

        m_FormItem->Show(true);
        SetStatusText("Add Table.");
    }


}
void MainFrame::OpenEditForm(wxString sTableId) {

    // wxPoint(100,100),
    // wxSize(500,410),

    m_FormItem = new GenericItemForm((wxFrame*) this, -1,"Edit Table",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP);

    //Define the database
    m_FormItem->AddItem("Title","title","","","","","","");
    m_FormItem->AddItem("Tablename *","tablename","","","","","","");
    m_FormItem->AddItem("Type of Table","tabletype","SELECTION{system;user;development;}","VARCHAR(255)","user","","","");
    m_FormItem->AddItem("Comments","comments","MULTILINE","","","","","");

    m_FormItem->SetUse("UPDATE");
    m_FormItem->CreateFields();
    m_FormItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,SYS_TABLES,"sys_tablesId");


    SetStatusText(sTableId);

    m_FormItem->SetID(sTableId);
    m_FormItem->Show(true);
    m_FormItem->LoadFields();
    SetStatusText("Edit Item");
}






void MainFrame::OnbEditItem( wxCommandEvent& event )
{
    if (!Utility::IsSystemDatabaseDeveloper()) {
        wxLogMessage(MSG_ONLY_DATABASE_DEVELOPERS);
    } else {
        wxArrayInt rowsSelected = m_MainGrid->GetSelectedRows();
        int size = rowsSelected.size();

        if (size == 1) {
            int row = rowsSelected[0];
            wxString sTableId = m_MainGrid->GetCellValue(row,0);
            OpenEditForm(sTableId);
        }
    }
}



wxString MainFrame::GetSelectedRowTable()
{
    wxString tableName ="";
    
    wxArrayInt rowsSelected = m_MainGrid->GetSelectedRows();
    
    int size = rowsSelected.size();
    
    if(size==1){
        int row = rowsSelected[0];
        tableName = m_MainGrid->GetCellValue(row,2);// Column two hold the table name
    }

    return tableName;
}

void MainFrame::OnDeleteCurrentDatabase( wxCommandEvent& event )
{
    auto *dlg = new wxMessageDialog(nullptr, wxT("Are you sure you want to delete the selected database?"), wxT("Delete Database"),  wxICON_EXCLAMATION|wxYES_NO);

    if ( dlg->ShowModal() == wxID_YES ){
        if(m_DatabaseCombo->GetCount()>0){
            Utility::ExecuteQuery("DROP DATABASE "+Settings.sDatabase);
            Utility::ExecuteQuery("dbworks","delete from sys_databases where databasename='"+Settings.sDatabase+"';");
            //Get the first selection, this will be the default
            m_DatabaseCombo->Select(0);
            Settings.sDatabase = m_DatabaseCombo->GetStringSelection();
            LoadDatabaseCombo();
            m_DatabaseCombo->Select(0);
            if(!Utility::DoesDatabaseExist(Settings.sDatabase))
                Utility::CreateDatabase(Settings.sDatabase);
            Refresh(true);
        }
    }
    dlg->Destroy();
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
    wxString user(Settings.sDatabaseUser);
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
        << SYS_FIELDS << ".valnull, "
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
                    wxString valNull(res[i]["valnull"], wxConvUTF8);
                    wxString valExtra(res[i]["valextra"], wxConvUTF8);

                    wxString fn(res[i]["FormName"], wxConvUTF8);
                    FormName = fn;
                    m_TableForm->AddField(title,valfield,valtype,flags,valDefault,valKey,valExtra,valNull);
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

    if (!Utility::IsSystemDatabaseDeveloper()) {
        wxLogMessage(MSG_ONLY_DATABASE_DEVELOPERS);
    } else {
        wxString sTable = GetSelectedRowTable();
        if(sTable.IsEmpty()){

            wxLogMessage(MSG_NEED_TO_SELECT_ROW);

        } else{


            auto *dlg = new wxMessageDialog(nullptr, wxT("Are you sure you want to delete this table?"), wxT("Delete Table"), (unsigned)wxYES_NO | (unsigned)wxICON_EXCLAMATION);

            if ( dlg->ShowModal() == wxID_YES ){

                wxString queryString = PrepareDeleteQuery(sTable);
                ExecuteQuery(queryString);
                m_MainGrid->DeleteSelectedRow();
            }


            dlg->Destroy();
            m_MainGrid->ResizeSpreadSheet();
            this->Layout();

        }




    }

}
wxString MainFrame::PrepareDeleteQuery(wxString sTable)
{

    wxString queryString;

    queryString = "DROP TABLE IF EXISTS `"+Settings.sDatabase+"`.`"+sTable+"`;";

    return queryString;
}


void MainFrame::ExecuteQuery(const wxString& queryString)
{
    //wxFrame *f = static_cast<wxFrame*>(this->GetParent());

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



void MainFrame::AddTableObject()
{
    wxArrayInt rowsSelected = m_MainGrid->GetSelectedRows();
    int size = rowsSelected.size();

    if (size == 1) {
       // int row = rowsSelected[0];

        wxString tableName ="";
        wxString tableId = "";
        int row = rowsSelected[0];

        tableId = m_MainGrid->GetCellValue(row,0);
        tableName = m_MainGrid->GetCellValue(row,2);

       // m_TableDiagramPanel->AddDrawObject( tableId, tableName);
        m_pTableDiagaram->AddDrawObject( tableId, tableName);

    }

}


//Create the filter table.
void MainFrame::OnbFilter( wxCommandEvent& event ) {


    m_pFilters = new GenericTable((wxFrame*) this, -1,"Table Filter Definitions",wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE);
    m_pFilters->SetTableDefinition("usr_filters", "User Filters", "","");// We will grab this from our form.

    if (m_pFilters != nullptr){

        //m_pFilters->SetGridTableName("usr_filters");

       // wxString linkID;
       // linkID << event.m_lTableID;

        m_pFilters->SetTableDefinition("usr_filters", "User Filters", "Select a filter","");
        //Add the field items
        m_pFilters->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword);

        m_pFilters->AddField("ID","usr_filtersId","int","HIDE-READONLY","","","",""); // This is the linking ID
        m_pFilters->AddField("Filter Name *","filterName","varchar(255)","","","","","");
        m_pFilters->AddField("Query Definition","queryDefinition","text","MULTILINE","","","","");
        m_pFilters->AddField("Associated TableId","associatedTableId","int","SELECTION_LINKED_NAME{sys_tables;Title;} - READONLY","","","","");
        m_pFilters->AddField("Description","description","text","","MULTILINE","","","");

        m_pFilters->Create();// Create the table.
        //m_TableForm->SetIDTitleName(event.m_sTableName+"Id"); Don't do this here
        m_pFilters->HideIDColumn();
        m_pFilters->Show(true);


    }
}


wxString MainFrame::GetCurrentStoredWhereCondition()
{
    return m_sCurrentStoredWhereCondition;
}
void MainFrame::SetCurrentStoredWhereCondition(wxString sWhereCondition)
{
    m_sCurrentStoredWhereCondition = sWhereCondition;
}

// This is where we blend the user where condition with the filter where condition
void MainFrame::SetGridWhereCondition(wxString whereToBlend)
{
    wxString userWhere = GetUserWhereCondition(); //Gets the administraion where condidtion
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
    //Safely destroy any window that are open so your program doesn't crash on close.
    // If you have a frames window open and you close the main app window, the program will crash on close because
    // you need to destroy all the open windows before you destroy the main frame.

    //This is where we need to shut down any windows that are open.
    if (m_TableForm != nullptr)
        m_TableForm->Destroy();

    if (m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    if(m_pFilters != nullptr)
        m_pFilters->Destroy();

    if(m_pTableDiagaram != nullptr)
        m_pTableDiagaram->Destroy();

    //Always do this last, only destroy the main frame when all open windows are destroyed. 
    bool bResult = wxFrame::Destroy();
    return bResult;
}

void MainFrame::Refresh(bool bReloadTableDiagram)
{
    if (Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser() || Utility::IsStandardUser() || Utility::IsGuest())
        m_MainGrid->LoadGridFromDatabase(true);//Check if the table exists before you load it
    else
        m_MainGrid->LoadGridFromDatabase(); //Load all tables.

    //OK, this is a very show function and needs a checkbox to turn it off.
    if(Settings.bAutoCheckDefinitions)
        wxGetApp().StartCheckIfTableDefinitionsMatchDatabaseTable();

    if(bReloadTableDiagram)
        LoadTableObjects();

    this->Layout();
}

void MainFrame::UpdateProgressBar(int val)
{
    if(m_ProgressGauge!=nullptr)
        m_ProgressGauge->SetValue(val);
}
//This is where we create a new database and fill
void MainFrame::CreateTableDefinitions(wxString sDatabase, wxString sTableName, ArrayTableFields tableFieldItemArray){

        //Create the table definition is sys_tables and return the table ID
        wxString tableId = Utility::InsertTableInSYS_TABLES(sDatabase,sTableName);

        for (int index=0;index < tableFieldItemArray.GetCount();index++){
            if(!tableId.IsEmpty())
                Utility::InsertFieldInSYS_FIELDS(sDatabase,tableId,tableFieldItemArray[index]);
        }
}

void MainFrame::OnOpenTableDiagram(wxCommandEvent& event)
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().

    if(m_pTableDiagaram != nullptr)
        m_pTableDiagaram->Destroy();


    m_pTableDiagaram = new TableDiagramFrame((TableDiagramFrame*) this, -1, "Table Diagram Definitions", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
    LoadTableObjects();
    m_pTableDiagaram->Show(true);


}

void MainFrame::OpenFile(wxCommandEvent& WXUNUSED(event)) {

}

void MainFrame::OnImportDatabase(wxCommandEvent& WXUNUSED(event))
{
    ImportMySQLDatabase * pImportMySQLForm = new ImportMySQLDatabase(this, -1, "Import MySQL Database", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);

    if(pImportMySQLForm->ShowModal()==wxOK){
        wxString sDatabase = pImportMySQLForm->GetDatabaseToCopy();
        if(sDatabase.IsEmpty())
            wxLogMessage("You need to select a database, please try again");
        else
            wxGetApp().StartImportDatabase(pImportMySQLForm->GetDatabaseToCopy(),pImportMySQLForm->GetNewDatabaseToCopyInto() );

    }


    pImportMySQLForm->Destroy();

}
void MainFrame::OpenTableDefinitions(wxString sTableName)
{
    if (m_TableForm!= nullptr){
        m_TableForm->Destroy();
        m_TableForm = nullptr;
    }

    wxString sTabldId= Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,sTableName);
    // Instead of opening contacts, we are going to open the generic form table.
    m_TableForm = new PropertyTable((wxFrame*) this, -1, " Table Field Definitions: "+sTableName,wxDefaultPosition,wxDefaultSize,(unsigned)wxDEFAULT_FRAME_STYLE);

    if (m_TableForm != nullptr){

        m_TableForm->SetGridTableName(sTableName);

        m_TableForm->SetTableDefinition(SYS_FIELDS, SYS_FIELDS, "All the fields of this table"," WHERE sys_tablesId="+sTabldId);// We will grab this from our form.
        //Add the field items
        m_TableForm->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword);

        m_TableForm->AddField("Linking ID *","sys_tablesId","int","HIDE-READONLY",sTabldId,"","",""); // This is the linking ID
        m_TableForm->AddField("Field Name *","valfield","varchar(100)","","","","","");
        m_TableForm->AddField("Type *","valtype","varchar(100)",MYSQL_TYPE_OPTIONS,"VARCHAR(255)","","","");
        m_TableForm->AddField("Can be Null *","valnull","varchar(3)","READONLY - SELECTION{YES;NO;}","YES","","","");
        m_TableForm->AddField("Key *","valkey","varchar(10)","","","","","");
        m_TableForm->AddField("Default *","valdefault","varchar(255)","","","","","");
        m_TableForm->AddField("Extra *","valextra","varchar(255)","","","","","");
        m_TableForm->AddField("Title","title","varchar(100)","","","","","");
        m_TableForm->AddField("Flags","flags","varchar(200)",FLAG_OPTIONS,"","","",""); // FLAG_OPTIONS are in global.h
        m_TableForm->Create();// Create the table.
        //m_TableForm->SetIDTitleName(event.m_sTableName+"Id"); Don't do this here
        m_TableForm->HideIDColumn();
        m_TableForm->Show(true);


    }


}

//CONTEXT MENU EVENT and REFRESH EVENTS
//We created an event to refresh the grid so we can call it from any frame class.
void MainFrame::OnMyEvent(MyEvent& event)
{
    if(event.m_bTableFieldDefinitions){
        //We are going to show the field types for this table
        m_MainGrid->SetGridCursor(event.m_iRow,event.m_iCol);
        OpenTableDefinitions(event.m_sTableName);
    }
    else if(event.m_bOpen){
        OpenForm(event.m_sTableName,event.m_sTableId);
    }
    else if(event.m_bEdit){
        OpenEditForm(event.m_sTableId);
    }
    else if(event.m_bHelpFrameWasDestroyed){
        m_pTableDiagaram = nullptr;
    }
    else if(event.m_bDestroyed){


        // If we have the diagram open, reload it incase we change an entry in the field definition.
        // At some stage, we will simply want to update the effected table, not reload the entire diagram.
        if(m_pTableDiagaram!= nullptr && m_TableForm!= nullptr)
            LoadTableObjects();

        //This needs to be changed because you might have two or more windows open, a help, table definitions, a table diagram and you don't want to null all of them.
        //What you need is extra flags in the event to identify which pointers you need to null.



        m_TableForm = nullptr;


        //m_ImportMySQLForm = nullptr; // This if fine here, you don't want to keep this window open after you import it.
        m_pFilters = nullptr;
        
        if(Settings.bAutoCheckDefinitions)
            wxGetApp().StartCheckIfTableDefinitionsMatchDatabaseTable(); //When we close the definition grid view, we need to reflect changes made to the definitions



    }
    else if(event.m_bTableDiagramFrameWasDestroyed){
        m_pTableDiagaram = nullptr; //NOT SURE YET, you might want to keep it open
    }
    else if(event.m_bHelpFrameWasDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }
    else if(event.m_bStatusMessage){
        SetStatusText(event.m_sData);
    }
    else{
        // We are showing everything
        //This is where we need to remember the current where conditon

        //If this is not empty, then we have a new where condition so we can override the stored where condition in the mainframe.
        if(!event.m_sWhereCondition.IsEmpty()){
            SetCurrentStoredWhereCondition(event.m_sWhereCondition);//Store the where condition from the grid in the mainframe.
            SetGridWhereCondition(event.m_sWhereCondition);
            Refresh(true);

        }else{
            //If the events where condition is empty, then see if we have a stored where condition.
            if(event.m_bShowAll){
                SetCurrentStoredWhereCondition("");//Remove the stored where condition because we want to show all records.
            }
            SetGridWhereCondition(GetCurrentStoredWhereCondition());
            Refresh(true);
        }

    }
}