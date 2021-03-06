
#include <string.h>
// THIS MUST COME FIRST BEFORE YOU INCLUDE MAIN.H
#include <wx/wxprec.h>
#include<wx/wx.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/stdpaths.h>
#include <wx/toolbar.h>


#include "Shared/global.h"
#include "Shared/Utility.h"
#include "Generic/GenericTable.h"
#include "Generic/DBGrid.h"
#include "MyEvent.h"
#include "HtmlHelp.h"




#include "PropertyTable.h"

//#include <mysql++.h>
//using namespace mysqlpp;

// This must be included last because it might use some above dependencies.


//=============
// MyFrame Class Methods
//

enum {
    ID_CREATE_TABLE= wxID_HIGHEST + 100,
    ID_DELETE_TABLE,
    ID_HELP
};

wxBEGIN_EVENT_TABLE(PropertyTable, GenericTable)
    EVT_TOOL(ID_CREATE_TABLE, PropertyTable::OnbCreateTable)
    EVT_TOOL(ID_DELETE_TABLE, PropertyTable::OnbDeleteTable)
    EVT_TOOL(ID_HELP, PropertyTable::OnbHelp)
    EVT_MYEVENT(PropertyTable::OnMyEvent)
wxEND_EVENT_TABLE()


PropertyTable::PropertyTable( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ):GenericTable( parent, id, title, pos, size, style )
{
    m_HtmlWin = nullptr;
}

bool PropertyTable::Create()
{
    GenericTable::Create();
    wxBitmap BitMap;
    wxInitAllImageHandlers(); //You need to call this or the images will not load.
    //Add a new toolbar item.
    Utility::LoadBitmap(BitMap,"newTable.png");
    m_Toolbar->AddTool(ID_CREATE_TABLE, wxT("Create Table"), BitMap, wxT("Create this table if it doesn't exit."));
    Utility::LoadBitmap(BitMap,"deleteTable.png");
    m_Toolbar->AddTool(ID_DELETE_TABLE, wxT("Delete Table"), BitMap, wxT("Delele table from the database."));
    m_Toolbar->Realize();
    // Window Properties
    this->SetBackgroundColour(wxColour(140,37,37));
    return true;
}


PropertyTable::~PropertyTable()
= default;

void PropertyTable::SetSettings(wxString& database, wxString& server, wxString& user, wxString& password)
{
    m_sDatabase=database;
    m_sServer=server;
    m_sUser=user;
    m_sPassword=password;
}

void PropertyTable::SetGridTableName(wxString& name)
{
    m_sGridTableName = name;
}

 void PropertyTable::OnbDeleteTable( wxCommandEvent& event )
{

    auto *dlg = new wxMessageDialog(nullptr, wxT(MSG_DELETE_TABLE_PROMPT), wxT("Delete Table"), wxYES_NO | wxICON_EXCLAMATION);

    if ( dlg->ShowModal() == wxID_YES ){
    }
        wxString queryString = PrepareDeleteQuery();
        Utility::ExecuteQuery(Settings.sDatabase,queryString);
    
      dlg->Destroy();
}

 void PropertyTable::OnbCreateTable( wxCommandEvent& event )
{
    //First we want to see if the table exists, if it does, then we need to alter the table. If the table doesn't exist, then create it.

    if (Utility::DoesTableExist(Settings.sDatabase,m_sGridTableName))
    {
        if(!SynFieldDefinitionToTable())
            wxLogMessage("The table exists and the definition are up to date");
        return;
    }

    wxString queryString = PrepareCreateQuery();

    auto *dlg = new wxMessageDialog(nullptr, "You are about the execute the following command, continue? \n\n"+queryString, wxT("Create Table"), wxYES_NO | wxICON_EXCLAMATION);

    if ( dlg->ShowModal() == wxID_YES )
        Utility::ExecuteQuery(Settings.sDatabase,queryString);

    dlg->Destroy();

}
//This function needs more work.
//We have to iterate through our grid and see if each field exits in the database.
bool PropertyTable::SynFieldDefinitionToTable()
{
    wxString fieldName;
    wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,m_sGridTableName);
    //for (int row = 0 ; row < num_rows; row++){

        ArrayTableFields fieldList;// We need to get the field array.
        if(Utility::GetFieldListFromSysFieldsByTableId(fieldList,sTableId)){ // This is all the fields for that table from sys_fields
           if(!Utility::SynFieldDefinitionToTable(m_sGridTableName,fieldList))
               wxLogMessage("There was an error, failed to syn fields, check definitions and try again.");
        }





 /*       if(Utility::DoesFieldExitInTable(m_sGridTableName,fieldName)){
            //return false;WHY ARE WE RETURN FALSE AlterTable will not run

        } else{
            // The field does not exist, we need to create it.
            // To do this, we need all row information, we just need to pass the row index then get it from the grid.
            //
            AlterTable(row);

        }*/
   // }
    return true;
}

//This will alter the table by adding new records. NOTE: table records are not deleted.
void PropertyTable::AlterTable(int row) {

    int num_cols=m_Grid->GetNumberCols();
    wxString msg = MSG_MYSQL_RESERVED_WORD;
    if( num_cols>0) {
        wxString fieldName = "";
        wxString valtype = "";
        wxString valnull = "";
        wxString valkey = "";
        wxString valdefault = "";
        wxString extra = "";

        for (int col = 0; col < num_cols; col++) {
            wxString cellValue = m_Grid->GetCellValue(row, col);

            switch (col) {
                case 2:
                    fieldName = cellValue;
                    if(Utility::IsReservedMySQLWord(fieldName)){

                        msg << fieldName;
                        wxLogMessage(msg);

                        return;
                    }
                    break;
                case 3:
                    valtype = cellValue;
                    break;
                case 4:
                    if (cellValue.Lower() == "no")
                        valnull = " NOT NULL ";
                    else
                        valnull = " ";
                    break;
                case 5:
                    valkey = cellValue;
                    break;
                case 6:
                    if (cellValue.Lower() == "null")
                        valdefault = " ";
                    else
                        valdefault=cellValue;
                    break;
                case 7:
                    extra = cellValue;
                    break;
                default:
                    break;

            }
        }



        wxString previousFieldName="";

        if(row>0)
            previousFieldName=m_Grid->GetCellValue(row-1, 2);
        else if(row==0)
            previousFieldName=m_sGridTableName + "Id";


        wxString QueryString = "";

        wxString DEFVAL = "";
        if(!valdefault.IsEmpty() && valdefault!= " "){
            DEFVAL = " DEFAULT '" + valdefault + "'";
        }



        QueryString = "ALTER TABLE " + m_sGridTableName + " ADD COLUMN " + fieldName + " " + valtype + " " + valnull + " " + valkey + DEFVAL + extra + " after "+previousFieldName+ ";";




        wxString msg;

        msg << MSG_ALTER_TABLE << "\n"  << QueryString;

        auto *dlg = new wxMessageDialog(nullptr, msg, wxT("Alter Table"), wxYES_NO | wxICON_EXCLAMATION);

        if ( dlg->ShowModal() == wxID_YES ){

            Utility::ExecuteQuery(Settings.sDatabase,QueryString);
        }

        dlg->Destroy();


    }
}

// If the table doesn't exist, this will prepare a create string.
wxString PropertyTable::PrepareCreateQuery()
{
    wxString queryString="";
    
    int num_cols=m_Grid->GetNumberCols();
    int num_rows=m_Grid->GetNumberRows();
    
    wxString fieldName;
    wxString valtype;
    wxString valnull;
    wxString valkey;
    wxString valdefault;
    wxString extra;

    wxString msg = MSG_MYSQL_RESERVED_WORD;

    queryString = "CREATE TABLE IF NOT EXISTS `"+m_sDatabase+"`.`"+m_sGridTableName+"` (";
    queryString += "`"+m_sGridTableName+"Id` INT NOT NULL AUTO_INCREMENT, ";
   
    for (int row = 0 ; row < num_rows; row++){
        
        //Reset
        //buildString="";
        fieldName="";
        valtype="";
        valnull="";
        valkey="";
        valdefault="";
        extra="";
        
        queryString += "`";
       
        for (int col = 0 ; col < num_cols; col++){
            wxString cellValue = m_Grid->GetCellValue(row,col);
            
                switch (col)
                {
                    case 2:
                        fieldName=cellValue;
                        if(Utility::IsReservedMySQLWord(fieldName)){

                            msg << fieldName;
                            wxLogMessage(msg);

                            return "";
                        }
                        break;
                    case 3:
                        valtype=cellValue;
                        break;
                    case 4:
                        if(cellValue.Lower()=="no")
                            valnull=" NOT NULL ";
                        else
                           valnull=" ";
                        break;
                    case 5:
                        valkey=cellValue;
                        break;
                    case 6:
                        if(cellValue.Lower()=="null")
                            valdefault=" ";
                        else if (Utility::IsEmpty(cellValue))
                            valdefault="";
                        else
                            valdefault= " DEFAULT '" + cellValue + "'";
                        break;
                    case 7:
                        extra=cellValue;
                        break;
                    default:
                        break;
                    
                }


        }
            queryString += fieldName + "` " + valtype + valnull + " " + valkey + " " + valdefault + " " + extra + ",";
            //Update the string
    }
    
    queryString += "PRIMARY KEY (`"+m_sGridTableName+"Id`));";
    
    return queryString;

}


wxString PropertyTable::PrepareDeleteQuery()
{
    
    wxString queryString;
  
    queryString = "DROP TABLE IF EXISTS `"+m_sDatabase+"`.`"+m_sGridTableName+"`;";
    
    return queryString;
}



void PropertyTable::OnbHelp( wxCommandEvent& event )
{

    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(1); //
    m_HtmlWin->Show(true);

}


//We can send a message to the parent that this window is destroyed.
bool PropertyTable::Destroy()
{

    MyEvent my_event( this );
    my_event.m_bDestroyedPropertyTable=true;
    GetParent()->ProcessWindowEvent( my_event );

    bool bResult = wxFrame::Destroy();
    return bResult;
}


void PropertyTable::OnMyEvent(MyEvent& event) {

    if(event.m_bTableFieldDefinitions){
        wxMessageBox("Event Function Properties : PropertyTable.Cpp 480");
    }
    else if(event.m_bOpen){
        ViewItem(event.m_iRow);
    }
    else if(event.m_bEdit){
        EditItem(event.m_iRow);
    }
    else if(event.m_bDestroyed){
        m_HtmlWin= nullptr;
        m_pFormItem=nullptr;

    }
    else if(event.m_bHelpFrameWasDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }

    if(event.m_bRefreshDatabase){
        m_Grid->LoadGridFromDatabase();

        this->Layout(); // This is the key, you need to relayout the form.
    }

}
