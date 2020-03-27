///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Feb 26 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#define TITLE_WIDTH 100
#define BUTTON_WIDTH 100
#define TEXTCTL_WIDTH 500
#define CTRL_HEIGHT 20
#define TEXTCTLMULTI_HEIGHT 60
#define BORDER_WIDTH 2
#define ALLOW_TO_GROW 1 // 1, the control will grow with the sizer
#define DONT_ALLOW_TO_GROW 0
#define WINDOW_WIDTH 600



///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Feb 26 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/textctrl.h>
#include <wx/hyperlink.h>
#include <wx/combobox.h>
#include <wx/datectrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/dynarray.h>

#include "GenericItemForm.h"
#include "MyEvent.h"

#include <mysql.h>
#include <mysql++.h>
#include "global.h"
#include "Utility.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayFieldItem);
//using namespace mysqlpp;


FieldItem::~FieldItem()
= default;

FieldItem::FieldItem(){
    field="";
    title="";
    flag="";
    type="";
    defaultValue="";
    textCtl=nullptr;
    comCtl=nullptr;
    datePickerCtl=nullptr;
    linkCtl=nullptr;
}

//EVT_COMBOBOX_CLOSEUP

///////////////////////////////////////////////////////////////////////////
using namespace mysqlpp;

GenericItemForm::GenericItemForm( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    //Initialise
    m_OK = nullptr;
    m_Cancel = nullptr;
    m_ButtonSizer = nullptr;
    m_mainFormSizer = nullptr;
    m_sUse = "";
    m_sOldSelectionText="";
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

}

//We can send a message to the parent that this window is destroyed.
bool GenericItemForm::Destroy()
{

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    bool bResult = wxFrame::Destroy();
    return bResult;
}

void GenericItemForm::CreateFields()
{

    // Run through the array and create the fields
    int count = itemArray.GetCount();
    int calculatedHeightWindow=0;
    wxString sFlags="";
       
       if (count>0){
           
                   //Create the main form sizer to fit all the controls
                   m_mainFormSizer = new wxBoxSizer( wxVERTICAL );

                   
                   for(int i=0;i<count;i++){

                               wxBoxSizer* gSizer1;
                               
                               gSizer1 = new wxBoxSizer( wxHORIZONTAL );
                               sFlags =  itemArray[i].flag;

                               itemArray[i].TitleCtl = new wxStaticText( this, wxID_ANY, itemArray[i].title, wxDefaultPosition, wxDefaultSize, 0 );
                               itemArray[i].TitleCtl->Wrap( -1 );
                                gSizer1->Add( itemArray[i].TitleCtl, 0, wxALL, BORDER_WIDTH);

                               if(Utility::HasFlag(sFlags,"MULTILINE")){

                                    //Don't do rich text right now because you copy and past into the field, some special characters might be present that you can't see.
                                    //I still need to implement parsing for special characters, then we might be able to use this style again.
                                   //unsigned long style =  wxTE_WORDWRAP | wxTE_RIGHT| wxTE_DONTWRAP | wxTE_MULTILINE | wxTE_RICH | wxHSCROLL;

                                    unsigned long style =  wxTE_WORDWRAP | wxTE_RIGHT | wxTE_MULTILINE  | wxHSCROLL;

                                    if (m_sUse=="VIEW")
                                        style |= wxTE_READONLY;

                                    itemArray[i].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style);
                                    itemArray[i].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,TEXTCTLMULTI_HEIGHT ) );
                                    gSizer1->Add( itemArray[i].textCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

                                   if(!itemArray[i].defaultValue.IsEmpty())
                                       itemArray[i].textCtl->SetValue(itemArray[i].defaultValue);

                                   calculatedHeightWindow += TEXTCTLMULTI_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

                                   m_mainFormSizer->Add( gSizer1, ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


                               }else if(Utility::HasFlag(sFlags,"LOOKUP_SELECTION") && m_sTableName!=SYS_FIELDS){ //SELECTION

                                   // NOTE: We don't want to come in here if we are in the property table, flag section, so we need to indicate this with a flag.
                                   //Create a wxComboCtrl. We need to fill it with the values extracted from

                                   //Extract all the selection items.
                                   wxArrayString sSelectionItemArray;
                                   Utility::ExtractSelectionLookupItems(sSelectionItemArray,sFlags);

                                   long style = 0;

                                   // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
                                   // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
                                   //if (m_sUse=="VIEW" || find != wxNOT_FOUND)
                                   if (Utility::HasFlag(itemArray[i].flag,"READONLY"))
                                       style |= wxCB_READONLY;

                                   itemArray[i].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);

                                   if(m_sUse=="VIEW" )
                                       itemArray[i].comCtl->Disable();

                                   gSizer1->Add( itemArray[i].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);
                                  // gSizer1->Add( itemArray[i].comCtl, 0 , wxEXPAND, BORDER_WIDTH);


                                   //Fill the list box with the selection items.
                                   for ( int index=0; index<sSelectionItemArray.GetCount(); index++ )
                                       itemArray[i].comCtl->Append(sSelectionItemArray[index]);

                                   if(!itemArray[i].defaultValue.IsEmpty())
                                       itemArray[i].comCtl->SetValue(itemArray[i].defaultValue);

                                  // wxSize size(-1,30); //The reason we have -1 is because we want the width to grow when sizing the window.
                                   //itemArray[i].comCtl->SetMaxSize(size);


                                   calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

                                   m_mainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );

                               }else if(Utility::HasFlag(sFlags,"SELECTION") || Utility::HasFlag(sFlags,"SELECTION_ADDITIVE")){ //SELECTION

                                   //Create a wxComboCtrl. We need to fill it with the values extracted from

                                   //Extract all the selection items.
                                   wxArrayString sSelectionItemArray;
                                   Utility::ExtractSelectionItems(sSelectionItemArray,sFlags);

                                   long style = 0;

                                   // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
                                   // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
                                   //if (m_sUse=="VIEW" || find != wxNOT_FOUND)
                                   if (Utility::HasFlag(itemArray[i].flag,"READONLY"))
                                       style |= wxCB_READONLY;

                                  itemArray[i].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);

                                   if(m_sUse=="VIEW" )
                                        itemArray[i].comCtl->Disable();

                                   gSizer1->Add( itemArray[i].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);


                                    //Fill the list box with the selection items.
                                   for ( int index=0; index<sSelectionItemArray.GetCount(); index++ )
                                       itemArray[i].comCtl->Append(sSelectionItemArray[index]);

                                   //Make sure you set the default value after you append selection items.
                                   if(!itemArray[i].defaultValue.IsEmpty())
                                       itemArray[i].comCtl->SetValue(itemArray[i].defaultValue);


                                   calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

                                   //Attache handlers to buttons

                                    // Connect a closeup event so I can remove the comments.
                                   itemArray[i].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( GenericItemForm::OnComboCloseUp ), nullptr, this );


                                   m_mainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );

                                    //If we have additive selection, these events will prevent the replacement of text and add them together.
                                    if(Utility::HasFlag(sFlags,"SELECTION_ADDITIVE")){
                                        itemArray[i].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( GenericItemForm::OnComboChange ), nullptr, this );
                                        itemArray[i].comCtl->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( GenericItemForm::OnComboDropDown ), nullptr, this );
                                    }


                               }
                               else if(itemArray[i].type == "DATE"){ // You can also create different controls for different types.
                                   int style = 0;

                                   style = wxDP_DEFAULT | wxDP_DROPDOWN | wxDP_SHOWCENTURY;

                                   itemArray[i].datePickerCtl = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, style );
                                   itemArray[i].datePickerCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                                   gSizer1->Add( itemArray[i].datePickerCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
                                   //gSizer1->Add( itemArray[i].textCtl, 0, 0, BORDER_WIDTH );

                                   if(!itemArray[i].defaultValue.IsEmpty())
                                       itemArray[i].datePickerCtl->SetValue(Utility::StringToDate(itemArray[i].defaultValue));
                                   else if(itemArray[i].type == "DATE") { //Check to see if the type is date, then set with current date.
                                       itemArray[i].datePickerCtl->SetValue(wxDateTime::Now());
                                   }

                                   if (m_sUse=="VIEW" || Utility::HasFlag(itemArray[i].flag,"READONLY"))
                                       itemArray[i].datePickerCtl->Disable();


                                   calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


                                   m_mainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
                               }
                               else if (Utility::HasFlag(sFlags,"WEBLINK"))
                                   {

                                   bool bWebLink = false;
                                   int style = 0;

                                   if (m_sUse=="VIEW" || Utility::HasFlag(itemArray[i].flag,"READONLY")){
                                       style = wxTE_READONLY;
                                       bWebLink=true;
                                   }

                                   //The only place we actually want to see the password is when we view the table.
                                   if(Utility::HasFlag(itemArray[i].flag,"PASSWORD") && m_sUse!="VIEW" && !bWebLink)
                                       style = wxTE_PASSWORD;

                                    if(bWebLink){
                                        //This Hyperlink control needs to have a URL and label, so I just added google, but it gets writen over
                                        itemArray[i].linkCtl = new wxHyperlinkCtrl( this, wxID_ANY,"weblink","https://www.google.com", wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU | wxHL_DEFAULT_STYLE ,wxHyperlinkCtrlNameStr);
                                        itemArray[i].linkCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                                        gSizer1->Add( itemArray[i].linkCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
                                        itemArray[i].linkCtl->SetURL("");
                                        itemArray[i].linkCtl->SetLabel("");

                                        if(!itemArray[i].defaultValue.IsEmpty()){
                                            itemArray[i].linkCtl->SetURL(itemArray[i].defaultValue);
                                            itemArray[i].linkCtl->SetLabel(itemArray[i].defaultValue);
                                        }
                                    }else{

                                        itemArray[i].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
                                        itemArray[i].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                                        gSizer1->Add( itemArray[i].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


                                        if(!itemArray[i].defaultValue.IsEmpty())
                                            itemArray[i].textCtl->SetValue(itemArray[i].defaultValue);

                                    }





                                   calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


                                   m_mainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
                               }
                               else{


                                   int style = 0;

                                   if (m_sUse=="VIEW" || Utility::HasFlag(itemArray[i].flag,"READONLY"))
                                       style = wxTE_READONLY;

                                   //The only place we actually want to see the password is when we view the table.
                                   if(Utility::HasFlag(itemArray[i].flag,"PASSWORD") && m_sUse!="VIEW")
                                       style = wxTE_PASSWORD;

                                   itemArray[i].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
                                   itemArray[i].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                                   gSizer1->Add( itemArray[i].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


                                   if(!itemArray[i].defaultValue.IsEmpty())
                                       itemArray[i].textCtl->SetValue(itemArray[i].defaultValue);


                                   calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


                                   m_mainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
                               }
                   }
       
                   wxString sOKLabel = "";
                   if (m_sUse=="ADD")
                       sOKLabel = "Add Item";
                   else if (m_sUse=="UPDATE")
                       sOKLabel = "Update Item";
                   else if (m_sUse=="VIEW")
                       sOKLabel = "Close";
           
                            m_ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
           
                            m_OK = new wxButton( this, wxID_ANY, sOKLabel, wxDefaultPosition, wxDefaultSize, 0 );
                            m_OK->SetMaxSize( wxSize( BUTTON_WIDTH,CTRL_HEIGHT ) );
                            m_ButtonSizer->Add( m_OK, DONT_ALLOW_TO_GROW, wxALL, BORDER_WIDTH );

                            calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
                            
                            if(m_sUse!="VIEW"){
               
                                m_Cancel = new wxButton( this, wxID_ANY , wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
                                m_Cancel->SetMaxSize( wxSize( BUTTON_WIDTH,CTRL_HEIGHT ) );
                                m_ButtonSizer->Add( m_Cancel, DONT_ALLOW_TO_GROW, wxALL, BORDER_WIDTH );


                            }
                            calculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

           //Add the spread sheet directly to the main form box grid..
           m_mainFormSizer->Add( m_ButtonSizer, DONT_ALLOW_TO_GROW, wxALIGN_CENTER, BORDER_WIDTH);

           ResizeTitleText(); //Find the maximum width of the title fields and set all controls to that width.

           this->SetSizer( m_mainFormSizer );
            
           
           this->Layout();
           this->SetSize(WINDOW_WIDTH,calculatedHeightWindow);

           this->Centre( wxBOTH );
           
           if (m_OK != nullptr)
               m_OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GenericItemForm::OnbOK ), nullptr, this );
           
           if(m_Cancel != nullptr)
               m_Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GenericItemForm::OnbCancel ), nullptr, this );
    
       }
}

//Search each text label and find the largest lable width.
//Once found, set all labels to that maximum value.
void GenericItemForm::ResizeTitleText()
{

    int num_cols=itemArray.GetCount();
    int MaxWidth=0;

    // First find the maximum label width
    for(int col=0;col<num_cols;col++)
    {

        int WidthOfChar =itemArray[col].TitleCtl->GetCharWidth();
        wxString str = itemArray[col].TitleCtl->GetLabel();
        int numChar = str.Length();
        int widthOfText = numChar*WidthOfChar;


        if(widthOfText>MaxWidth)
            MaxWidth = widthOfText;
    }

    for(int col=0;col<num_cols;col++) {

        itemArray[col].TitleCtl->SetMinSize(wxSize(MaxWidth+5, CTRL_HEIGHT));
        // Set each label width
    }
}


void GenericItemForm::OnComboCloseUp( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();


    //Remove the comments from the combo.
    wxString value = combo->GetStringSelection();
    int i = value.find('[',0);

    if(i!= wxNOT_FOUND){
        value = value.Left(i);
        value = value.Trim(true);
        combo->SetValue(value);
    }
}

GenericItemForm::~GenericItemForm()
{
   
    if (m_OK != nullptr){
        m_OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GenericItemForm::OnbOK ), nullptr, this );
        //m_OK->Destroy();
    }
    if(m_Cancel != nullptr){
        m_Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GenericItemForm::OnbCancel ), nullptr, this );
        //m_Cancel->Destroy();
    }

    itemArray.Empty();
    
}

void GenericItemForm::AddItem(const wxString& fieldTitle, const wxString& field, const wxString& flag,const wxString& type, const wxString& defaultVal)
{
    auto *item = new FieldItem();
    item->title = fieldTitle;
    item->field = field;
    item->flag = flag;
    item->type = type;
    item->defaultValue = defaultVal;
    itemArray.Add(item);

}

void GenericItemForm::SetSettings( const wxString& database, const wxString& server, const wxString& user, const wxString& password, const wxString& tableName, const wxString& KeyName)
{
    m_sTableName = tableName;
    m_sKeyName = KeyName;
    m_sDatabase=database;
    m_sServer=server;
    m_sUser=user;
    m_sPassword=password;
}

void GenericItemForm::OnbOK( wxCommandEvent& event )
{
    if(m_sUse=="ADD")
        InsertItem();
    else if(m_sUse=="UPDATE")
        UpdateItem();
    else{
        //If we are only viewing, there is no need to refresh the grid.
        Close(TRUE);
        return;

    }

    //Send a message to the main frame to update the spreadsheet
    //Refresh the control by generating an event to the main frame.
    MyEvent my_event( this );
    my_event.m_bRefreshDatabase = true;
    GetParent()->ProcessWindowEvent( my_event );

     Close(TRUE);
}
void GenericItemForm::OnbCancel( wxCommandEvent& event )
{
    //The is no need to refresh the parent if we are canceling.
    //Refresh the control by generating an event to the main frame.
 //   MyEvent my_event( this );
 //   GetParent()->ProcessWindowEvent( my_event ) ;

    Close(TRUE);
}


void GenericItemForm::SetUse(const wxString& sUse){

    m_sUse = sUse;

    if(m_sUse=="UPDATE" && m_OK != nullptr)
        m_OK->SetLabel("Update Item");
    else if (m_sUse=="ADD" && m_OK!= nullptr)
        m_OK->SetLabel("Add Item");
    else if (m_sUse=="VIEW" && m_OK!= nullptr)
        m_OK->SetLabel("View Item");
}

void GenericItemForm::SetID(const wxString& pID)
{
    m_sId=pID;
}

void GenericItemForm::InsertItem(){
  
    
    //auto *f = dynamic_cast<wxFrame*>(this->GetParent());
    
    wxString database(m_sDatabase);
    wxString server(m_sServer);
    wxString user(m_sUser);
    wxString pass(m_sPassword);
       
    try{
        // Connect to the sample database.
        Connection conn(false);
       
        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {

            ParseSpecialCharacters();
            
            wxString queryString;
            int count = itemArray.GetCount();
            
            if (count>0){
                
                queryString = "INSERT INTO " +m_sTableName + " (";
                
                for(int i=0;i<count;i++){
                    if (i == count-1)
                        queryString = queryString + itemArray[i].field;
                    else
                        queryString = queryString + itemArray[i].field + ",";
                }
                    
                queryString = queryString + ") VALUES (";
                
                for(int i=0;i<count;i++){

                    wxString sValue;
                    if(itemArray[i].textCtl != nullptr)
                        sValue = itemArray[i].textCtl->GetValue();
                    else if(itemArray[i].comCtl != nullptr)
                        sValue = itemArray[i].comCtl->GetValue();
                    else if(itemArray[i].datePickerCtl != nullptr)
                        sValue = Utility::DateToString(itemArray[i].datePickerCtl->GetValue());
                    else if(itemArray[i].linkCtl != nullptr)
                        sValue = itemArray[i].linkCtl->GetURL();

                    if(itemArray[i].type=="int"){
                        if (i == count-1)
                            queryString = queryString +  sValue+ ")";
                        else
                            queryString = queryString + sValue + ",";

                    }
                    else{
                        if (i == count-1)
                            queryString = queryString + "'" +  sValue+ "')";
                        else
                            queryString = queryString + "'" + sValue + "',";
                    }
                    

                }

            }

            
            Query query = conn.query(queryString);
            query.execute();

            //wxMessageBox(queryString);

            StoreQueryResult res = query.store();
        }
        
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

void GenericItemForm::UpdateItem(){

        wxString database(m_sDatabase);
        wxString server(m_sServer);
        wxString user(m_sUser);
        wxString pass(m_sPassword);
       
    try{
        // Connect to the sample database.
        Connection conn(false);

                                                       
        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {


            ParseSpecialCharacters();

            wxString queryString = "UPDATE " + m_sTableName + " SET ";
            
            // Run through the array and create the fields
            int count = itemArray.GetCount();

            if (count>0){
                for(int i=0;i<count;i++){

                    wxString sValue;
                    if(itemArray[i].textCtl != nullptr)
                        sValue = itemArray[i].textCtl->GetValue();
                    else if(itemArray[i].comCtl != nullptr)
                        sValue = itemArray[i].comCtl->GetValue();
                    else if(itemArray[i].datePickerCtl != nullptr)
                        sValue = Utility::DateToString(itemArray[i].datePickerCtl->GetValue());
                    else if(itemArray[i].linkCtl != nullptr)
                        sValue = itemArray[i].linkCtl->GetURL();

         
                    if(itemArray[i].type=="int"){
                        if (i == count-1)
                            queryString = queryString + itemArray[i].field + " = " + sValue;
                        else
                            queryString = queryString + itemArray[i].field + " = " + sValue + " ,";
                        
                    }
                    else{
                        if (i == count-1)
                            queryString = queryString + itemArray[i].field + " = '" + sValue + "' ";
                        else
                            queryString = queryString + itemArray[i].field + " = '" + sValue + "' ,";
                    }
                }
                    
            }
            
            queryString = queryString + "WHERE " + m_sKeyName + " = " + m_sId;

            Query query = conn.query(queryString);
            query.execute();

            StoreQueryResult res = query.store();
    
        }

        
    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up

       
    } catch (const BadConversion& er) {
        // Handle bad conversions


        
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions

        
    }
    
}
/*
UPDATE sys_docs SET Title = 'Main Window' ,TypeOfDoc = 'Help Document' ,Document = '<HTML>\n\t<HEAD>\n\t</HEAD>\n\n\t<BODY><P><b><h2>Help Documentaion</h2></b></P>\n\n<i>Overview</i><br>\n\n<p>This program allows you to quicky and effectivly create a database that is powerful and simple to use.</p>\n<p>\n#User Types\n# SYSTEM_DATABASE_DEVELOPER - Complete permissions\n# SYSTEM_DATABASE_ADMINISTRATOR - Can add, edit, view and delete user records as well as assign user permissions and other administration functions.\n# ADVANCED_USER - Can add, edit, view and delete user records.\n# STANDARD_USER - Can add, edit, view user records, but can't delete any records.\n# GUEST - Can only view records\n\n</p>\n\n\n\t</BODY>\n</HTML>\n'


*/



//Load the form fields directly from the database rather than passing them in from the form given a passwordId
void GenericItemForm::LoadFields()
{
        wxString database(m_sDatabase);
        wxString server(m_sServer);
        wxString user(m_sUser);
        wxString pass(m_sPassword);

        // Connect to the sample database.
        Connection conn(false);
    try{
         
            if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                             (const_cast<char*>((const char*)server.mb_str())),
                             (const_cast<char*>((const char*)user.mb_str())),
                             (const_cast<char*>((const char*)pass.mb_str())))) {

                Query query = conn.query("select * from " + m_sTableName + " WHERE "+m_sKeyName+" = " + m_sId);
                           
                StoreQueryResult res = query.store();

                // Display results
                if (res) {
                    
                    //There will only be one row
                    // Get each row in result set, and print its contents
                    for (size_t i = 0; i < res.num_rows(); ++i) {
                        
                            //Add a new row to the grid control.
                            int count = itemArray.GetCount();


                            if (count>0){
                                
                                for(int index=0;index<count;index++){
                                    wxString strData1(res[i][itemArray[index].field], wxConvUTF8);

                                    wxString defaultValue = itemArray[index].defaultValue;

                                    //Make sure we don't have an empty field because of a bug that cause field left to overwright the right field
                                    if(strData1.IsEmpty()){
                                        //This is where we can place a default value only if there is no value in the database.
                                        if (!defaultValue.IsEmpty())
                                            strData1 = defaultValue;
                                        else if(itemArray[index].type == "DATE") { //Check to see if the type is date, then set with current date.
                                            wxDateTime now = wxDateTime::Now();
                                            wxString dt = now.FormatDate();
                                            strData1 = dt;
                                        }
                                        else if(itemArray[index].type == "TIME") { //Check to see if the type is date, then set with current date.
                                            wxDateTime now = wxDateTime::Now();
                                            wxString dt = now.FormatTime();
                                            strData1 = dt;
                                        }
                                        else if(itemArray[index].type == "DATETIME") { //Check to see if the type is date, then set with current date.

                                        }
                                        else if(itemArray[index].type == "TIMESTAMP") { //Check to see if the type is date, then set with current date.

                                        }
                                        else if(itemArray[index].type == "YEAR") { //Check to see if the type is date, then set with current date.

                                        }
                                    }

                                    if(itemArray[index].textCtl != nullptr)
                                        itemArray[index].textCtl->SetValue(strData1);
                                    else if(itemArray[index].comCtl != nullptr)
                                        itemArray[index].comCtl->SetValue(strData1);
                                    else if(itemArray[index].datePickerCtl != nullptr)
                                        itemArray[index].datePickerCtl->SetValue(Utility::StringToDate(strData1));
                                    else if(itemArray[index].linkCtl != nullptr){
                                        itemArray[index].linkCtl->SetURL(strData1);
                                        itemArray[index].linkCtl->SetLabel(strData1);
                                    }


                                }
                            }
                    }
                }
            }
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






//This only removed " and ' from the field, this is not allowed in an entry.
void GenericItemForm::ParseSpecialCharacters()
{


    // Run through the array and create the fields
    int count = itemArray.GetCount();

    if (count>0){

        for(int i=0;i<count;i++){

            wxString st;

            if(itemArray[i].textCtl != nullptr)
                st = itemArray[i].textCtl->GetValue();
            else if(itemArray[i].comCtl != nullptr)
                st = itemArray[i].comCtl->GetValue();


            st=Utility::Escape(st);

            if(itemArray[i].textCtl != nullptr)
                itemArray[i].textCtl->SetValue(st);
            else if(itemArray[i].comCtl != nullptr)
                itemArray[i].comCtl->SetValue(st);

        }
    }

}


//Before you dropdown, save the combo contents.
void GenericItemForm::OnComboDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    m_sOldSelectionText = combo->GetValue();

}
void GenericItemForm::OnComboChange( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString value = combo->GetStringSelection();

    if(m_sOldSelectionText.IsEmpty())
        combo->SetValue(value);
    else{
        m_sOldSelectionText << " - " << value;
        combo->SetValue(m_sOldSelectionText);
    }




}