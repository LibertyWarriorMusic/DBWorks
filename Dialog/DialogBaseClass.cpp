//
// Created by Nicholas Zounis on 23/3/20.
//

#define TEXTWIDTH 140
#define ALLOW_TO_GROW 1

#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/listbox.h>

#include <wx/hyperlink.h>
#include <wx/datectrl.h>
#include "../Shared/Utility.h"
#include "../Shared/global.h"

#include "../CustomControls/ListBoxManager.h"
#include "../CustomControls/MyTextCtrl.h"


#include "DialogBaseClass.h"
enum {
    ID_OK = wxID_HIGHEST + 1400,
    ID_CANCEL
};


wxBEGIN_EVENT_TABLE(DialogBaseClass, wxDialog)

wxEND_EVENT_TABLE()

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayFieldDataCtls);



DialogBaseClass::DialogBaseClass( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    m_CalculatedHeightWindow=0;
    m_OkLabel = "OK";
    //The is the sizer we are going to attach control to.
    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );
    m_CtrlItemSizer = new wxBoxSizer(wxVERTICAL);
    m_MainFormSizer->Add( m_CtrlItemSizer, 0, wxALIGN_LEFT, 0);
    this->SetLabel(title);
    this->SetSizer( m_MainFormSizer );

    m_Winpos=pos;
}
wxBoxSizer* DialogBaseClass::GetMainSizer()
{
    return m_MainFormSizer;
}

wxArrayString* DialogBaseClass::GetSelectionArrayString()
{
    return &m_asSelectionItemArray;
}

void DialogBaseClass::SetOkLabel(wxString label)
{
    m_OkLabel = label;
}

void DialogBaseClass::Create()
{
    //------------------------------------------------
    // BUTTONS IMPORT AND CANCEL

    wxBoxSizer *btSizer = new wxBoxSizer( wxHORIZONTAL );

    m_bOk = new wxButton( this, wxID_ANY , wxT("Import Database"), wxDefaultPosition, wxSize(120,20), 0 );
    m_bOk->SetLabel(m_OkLabel);
    m_bOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogBaseClass::OnbOk ), nullptr, this );
    btSizer->Add( m_bOk, 0, wxALL, BORDER_WIDTH );


    m_Cancel = new wxButton( this, wxID_ANY , wxT("Cancel"), wxDefaultPosition, wxSize(120,20), 0 );
    m_Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogBaseClass::OnbCancel ), nullptr, this );
    btSizer->Add( m_Cancel, 0, wxALL, BORDER_WIDTH );

    m_MainFormSizer->Add( btSizer, 0, wxALIGN_CENTER, BORDER_WIDTH);

    m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
    m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH + 10;

    //this->SetPosition(m_Winpos);
    this->SetSize(WINDOW_WIDTH,m_CalculatedHeightWindow);


    this->Layout();



}

DialogBaseClass::~DialogBaseClass(){

}

//Search each text label and find the largest lable width.
//Once found, set all labels to that maximum value.
void DialogBaseClass::ResizeTitleText()
{

    int num_cols=m_CtrlDataItem.GetCount();
    int MaxWidth=0;

    // First find the maximum label width
    for(int col=0;col<num_cols;col++)
    {

        int WidthOfChar =m_CtrlDataItem[col].TitleCtl->GetCharWidth();
        wxString str = m_CtrlDataItem[col].TitleCtl->GetLabel();
        int numChar = str.Length();
        int widthOfText = numChar*WidthOfChar;

        if(widthOfText>MaxWidth)
            MaxWidth = widthOfText;
    }

    for(int col=0;col<num_cols;col++)
        m_CtrlDataItem[col].TitleCtl->SetMinSize(wxSize(MaxWidth+5, CTRL_HEIGHT));

}
//We can send a message to the parent that this window is destroyed.
bool DialogBaseClass::Destroy()
{
    bool bResult = wxDialog::Destroy();
    return bResult;
}

void DialogBaseClass::OnbCancel( wxCommandEvent& event )
{
    EndModal(wxCANCEL);
}

void DialogBaseClass::OnbOk( wxCommandEvent& event )
{
    EndModal(wxOK);
}

int DialogBaseClass::GetCtlIndex(wxString sIdentifier){
    for (int index=0; index < m_CtrlDataItem.GetCount();index++) {
        if (sIdentifier == m_CtrlDataItem[index].m_sIdentifier)
            return index;
    }
    return wxNOT_FOUND;
}

//Set addition flags for the data: PASSWORD - READONLY
void DialogBaseClass::SetFlags(wxString sIdentifier, wxString sFlags)
{
    for (int index=0; index < m_CtrlDataItem.GetCount();index++) {
        if (sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            m_CtrlDataItem[index].Flags=sFlags;
            return;
        }
    }
}

FieldCtlItem* DialogBaseClass::NewFieldDataCtrl()
{
    FieldCtlItem *item = new FieldCtlItem();
    return item;
}


// This is where we generate our control
void DialogBaseClass::AddCtlItem(int iTypeOfControl, wxString TitleName, wxString Description, wxString sIdentifier, int ID)
{
    FieldCtlItem* ctlItem = NewFieldDataCtrl();
    ctlItem->Title= TitleName;
    ctlItem->iTypeOfControl = iTypeOfControl;
    ctlItem->m_sDescription=Description;
    ctlItem->m_sIdentifier=sIdentifier;
    m_CtrlDataItem.Add(ctlItem);
    int index = m_CtrlDataItem.GetCount();

    index--;


    switch(m_CtrlDataItem[index].iTypeOfControl) {
        case CTL_STATIC :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, ID, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            break;
        case CTL_TEXT :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
            break;
        case CTL_MULTI_TEXT :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            break;
        case CTL_SELECTION_LOOKUP_NAME :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].comCtl = new wxComboBox( this, ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,0 );
            break;

        case CTL_SELECTION_ADDITIVE :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].comCtl = new wxComboBox( this, ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,0);
            break;

        case CTL_SELECTION :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].comCtl = new wxComboBox( this, ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,0);

            break;
        case CTL_CHECKBOX :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].pCheckBox = new wxCheckBox( this, ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            break;
        case CTL_DATE :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].datePickerCtl = new wxDatePickerCtrl( this, ID, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, 0 );

            break;

        case CTL_LISTBOX :

            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].pListBox = new wxListBox(this, ID,wxDefaultPosition, wxDefaultSize);
            break;

        case CTL_BUTTON :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, Description, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].pButton = new wxButton(this, ID,TitleName, wxDefaultPosition, wxDefaultSize);
            break;
        case CTL_MYTEXTCTRL :
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].pMyTextCtrl = new MyTextCtrl(this,"Test");
            break;
        default:
            break;
    }
}

void DialogBaseClass::RemoveCtlItem(wxString sIdentifier)
{
    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(m_CtrlDataItem[index].m_sIdentifier==sIdentifier){

            m_CtrlItemSizer->Remove(m_CtrlDataItem[index].pBoxSizer);

            // m_CtrlDataItem[index].DestroyCtrl();// This is causing a crash
            m_CtrlDataItem[index].HideCtrl();
            
            m_CtrlDataItem.RemoveAt(index); //Remove it from the list

            this->Layout();
            break;
        }
    }
}

bool DialogBaseClass::HasCtlItem(wxString sIdentifier)
{
    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(m_CtrlDataItem[index].m_sIdentifier==sIdentifier){
            return true;
        }
    }
    return false;
}

//We added a new ctrl item after we created the OK buttons, so we need to add this before the buttons.
void DialogBaseClass::RenderNewControl()
{
    int index = m_CtrlDataItem.GetCount();
    index--;
    if(index > -1){
        //m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
        RenderCtrl(index);
        this->Layout();
    }
}


//After we add all our control, we call this function to render our constrols to the main sizer so they display
void DialogBaseClass::RenderAllControls()
{

    for (int index=0; index < m_CtrlDataItem.GetCount();index++)
        RenderCtrl(index);

    ResizeTitleText(); //Find the maximum width of the title fields and set all controls to that width.
    this->Layout();
    this->Centre( wxBOTH );
    //Create the buttons to the dialog
    Create();
}

void DialogBaseClass::RenderCtrl(int index)
{
    wxString sFlags="";
    unsigned long style=0;

    sFlags =  m_CtrlDataItem[index].Flags; // Get additional flags for the control
    switch(m_CtrlDataItem[index].iTypeOfControl) {
        case CTL_STATIC :

            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );



            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            break;
        case CTL_TEXT :
            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY"))
                style = wxTE_READONLY;
                //The only place we actually want to see the password is when we view the table.
            else if(Utility::HasFlag(m_CtrlDataItem[index].Flags,"PASSWORD") )
                style = wxTE_PASSWORD;

            //m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
            m_CtrlDataItem[index].textCtl->SetWindowStyle(style);


            m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_MULTI_TEXT :

            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;



            //Don't do rich text right now because you copy and past into the field, some special characters might be present that you can't see.
            //I still need to implement parsing for special characters, then we might be able to use this style again.
            //unsigned long style =  wxTE_WORDWRAP | wxTE_RIGHT| wxTE_DONTWRAP | wxTE_MULTILINE | wxTE_RICH | wxHSCROLL;

            style =  wxTE_WORDWRAP | wxTE_RIGHT | wxTE_MULTILINE  | wxHSCROLL;

            if (Utility::HasFlag( m_CtrlDataItem[index].Flags,"READONLY"))
                style |= wxTE_READONLY;

            //m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style);
            m_CtrlDataItem[index].textCtl->SetWindowStyle(style);


            m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,TEXTCTLMULTI_HEIGHT ) );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].textCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //This will probably not be used for combos, but kept it hear because it might be used to remember a default value for the control
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += TEXTCTLMULTI_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer, ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_SELECTION_LOOKUP_NAME :
            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=m_CtrlDataItem[index].comCtl->GetWindowStyle();

            //We can add other type of flags here
            if(Utility::HasFlag(m_CtrlDataItem[index].Flags,"SQL_DATABASES"))
                Utility::LoadStringArrayWidthMySQLDatabaseNames(m_asSelectionItemArray);
            else if(Utility::HasFlag(m_CtrlDataItem[index].Flags,"SYS_TABLES"))
                Utility::LoadStringArrayWithTableNamesFromSysTables(Settings.sDatabase, m_asSelectionItemArray);
            else if(Utility::HasFlag(m_CtrlDataItem[index].Flags,"SYS_HIDDEN_TABLES"))
                Utility::LoadStringArrayWithTableNamesFromSysTables(Settings.sDatabase, m_asSelectionItemArray, true);
            else
                Utility::ExtractSelectionLookupItemsName(m_asSelectionItemArray,sFlags);


            // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
            // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
            //if (m_sUse=="VIEW" || find != wxNOT_FOUND)
            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY")){
                style |= wxCB_READONLY;
                m_CtrlDataItem[index].comCtl->Disable();
            }


            //m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);
            m_CtrlDataItem[index].comCtl->SetWindowStyle(style);

            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);


            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);
            // for ( int iIdx=0; iIdx<m_asSelectionItemArray.GetCount(); iIdx++ )
            //m_CtrlDataItem[index].comCtl->Append(m_asSelectionItemArray[iIdx]);


            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer, 0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION_ADDITIVE :
            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=m_CtrlDataItem[index].comCtl->GetWindowStyle();
            //Create a wxComboCtrl. We need to fill it with the values extracted from

            //Extract all the selection items.
            Utility::ExtractSelectionItems(m_asSelectionItemArray,sFlags);


            // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
            // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
            //if (m_sUse=="VIEW" || find != wxNOT_FOUND)
            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY")){
                style |= wxCB_READONLY;
            }

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"DISABLED")){
                m_CtrlDataItem[index].comCtl->Disable();
            }

            //m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);
            m_CtrlDataItem[index].comCtl->SetWindowStyle(style);

            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);


            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);
            //for ( int iIdx=0; iIdx<sSelectionItemArray.GetCount(); iIdx++ )
            //m_CtrlDataItem[index].comCtl->Append(sSelectionItemArray[iIdx]);

            //Make sure you set the default value after you append selection items.
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);


            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            //Attache handlers to buttons

            // Connect a closeup event so I can remove the comments.
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( DialogBaseClass::OnComboCloseUp ), nullptr, this );

            //These events will prevent the replacement of text and add them together.

            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( DialogBaseClass::OnComboChange ), nullptr, this );
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( DialogBaseClass::OnComboDropDown ), nullptr, this );


            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer, 0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION :

            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=m_CtrlDataItem[index].comCtl->GetWindowStyle() ;
            //Create a wxComboCtrl. We need to fill it with the values extracted from

            //Extract all the selection items.
            Utility::ExtractSelectionItems(m_asSelectionItemArray,sFlags);


            // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
            // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
            //if (m_sUse=="VIEW" || find != wxNOT_FOUND)
            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY")){
                style |= wxCB_READONLY;
            }

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"DISABLED")){
                m_CtrlDataItem[index].comCtl->Disable();
            }

            //m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);
            m_CtrlDataItem[index].comCtl->SetWindowStyle(style);

            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);


            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);
            // for ( int iIdx=0; iIdx<sSelectionItemArray.GetCount(); iIdx++ )
            //m_CtrlDataItem[index].comCtl->Append(sSelectionItemArray[iIdx]);

            //Make sure you set the default value after you append selection items.
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            //Attache handlers to buttons
            // Connect a closeup event so I can remove the comments.
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( DialogBaseClass::OnComboCloseUp ), nullptr, this );



            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_CHECKBOX :
            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            //m_CtrlDataItem[index].pCheckBox = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].pCheckBox, 0, wxALL, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_DATE :
            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;


            style = wxDP_DEFAULT | wxDP_DROPDOWN | wxDP_SHOWCENTURY;

            //m_CtrlDataItem[index].datePickerCtl = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, style );
            m_CtrlDataItem[index].datePickerCtl->SetWindowStyle(style);


            m_CtrlDataItem[index].datePickerCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].datePickerCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
            //gSizer1->Add( itemArray[i].textCtl, 0, 0, BORDER_WIDTH );

            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].datePickerCtl->SetValue(Utility::StringToDate(m_CtrlDataItem[index].fieldDefault));
            else if(m_CtrlDataItem[index].fieldType == "DATE") { //Check to see if the type is date, then set with current date.
                m_CtrlDataItem[index].datePickerCtl->SetValue(wxDateTime::Now());
            }

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY"))
                m_CtrlDataItem[index].datePickerCtl->Disable();


            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_LISTBOX :

            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].pListBox,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );

            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_BUTTON :

            m_CtrlDataItem[index].pBoxSizer = new wxBoxSizer( wxHORIZONTAL );
            //m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            m_CtrlDataItem[index].pBoxSizer->Add( m_CtrlDataItem[index].pButton,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );

            m_CtrlItemSizer->Add( m_CtrlDataItem[index].pBoxSizer,0, wxEXPAND, BORDER_WIDTH );
            break;


        default:
            break;
    }

}

void DialogBaseClass::SetDataValue(wxString sIdentifier,wxString sData)
{

    bool bState=false;
    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {
                case CTL_STATIC :
                    m_CtrlDataItem[index].m_sData = sData;
                    break;
                case CTL_TEXT :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].textCtl->SetValue(sData);
                    break;
                case CTL_MULTI_TEXT :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].textCtl->SetValue(sData);
                    break;
                case CTL_SELECTION_LOOKUP_NAME :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_SELECTION_ADDITIVE :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_SELECTION :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_CHECKBOX :
                    m_CtrlDataItem[index].m_sData = sData;
                    sData.Lower();

                    if (sData == "true")
                        bState = true;
                    else if (sData == "false")
                        bState = false;

                    m_CtrlDataItem[index].pCheckBox->SetValue(bState);
                    break;
                case CTL_DATE :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].datePickerCtl->SetValue(Utility::StringToDate(sData));
                    break;
                case CTL_WEBLINK :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].linkCtl->SetURL(sData);
                    m_CtrlDataItem[index].linkCtl->SetLabel(sData);
                    break;
                default:
                    break;
            }

            return;

        }
    }
}

wxString DialogBaseClass::GetDataValue(wxString sIdentifier)
{

    bool bState=false;
    wxString sTempData="";

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {
                case CTL_STATIC :
                    return m_CtrlDataItem[index].m_sData;

                case CTL_TEXT :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].textCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_MULTI_TEXT :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].textCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_SELECTION_LOOKUP_NAME :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_SELECTION_ADDITIVE :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_SELECTION :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_CHECKBOX :

                    bState = m_CtrlDataItem[index].pCheckBox->GetValue();
                    sTempData="false";

                    if (bState)
                        sTempData = "true";
                    else if (!bState)
                        sTempData = "false";
                    m_CtrlDataItem[index].m_sData = sTempData;
                    return sTempData;

                case CTL_DATE :
                    m_CtrlDataItem[index].m_sData = Utility::DateToString(m_CtrlDataItem[index].datePickerCtl->GetValue());
                    return m_CtrlDataItem[index].m_sData;

                case CTL_WEBLINK :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].linkCtl->GetURL();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_BUTTON :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].pButton->GetLabel();
                    return m_CtrlDataItem[index].m_sData;
                case CTL_LISTBOX :
                    return m_CtrlDataItem[index].m_sData;
                default:

                    break;
            }
        }
    }
    return "";
}


void* DialogBaseClass::GetControl(wxString sIdentifier)
{

    bool bState=false;
    wxString sTempData="";

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {
                case CTL_STATIC :
                    return (wxStaticText*)m_CtrlDataItem[index].TitleCtl;
                case CTL_TEXT :
                    return (wxTextCtrl*)m_CtrlDataItem[index].textCtl;
                case CTL_MULTI_TEXT :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_SELECTION_LOOKUP_NAME :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_SELECTION_ADDITIVE :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_SELECTION :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_CHECKBOX :
                    return (wxCheckBox*)m_CtrlDataItem[index].pCheckBox;
                case CTL_DATE :
                    return (wxDatePickerCtrl*)m_CtrlDataItem[index].datePickerCtl;
                case CTL_WEBLINK :
                    return (wxHyperlinkCtrl*)m_CtrlDataItem[index].linkCtl;
                case CTL_BUTTON :
                    return (wxButton*) m_CtrlDataItem[index].pButton;
                case CTL_LISTBOX :
                    return (wxListBox*) m_CtrlDataItem[index].pListBox;
                default:
                    break;
            }
        }
    }

    return nullptr;
}

bool DialogBaseClass::GetCheckState(wxString sIdentifier)
{
    bool bState=false;

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {

                case CTL_CHECKBOX :
                    return m_CtrlDataItem[index].pCheckBox->GetValue();;

                default:

                    break;
            }
        }
    }
    return false;
}
void DialogBaseClass::SetCheckState(wxString sIdentifier, bool bCheckState){

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control
            switch(m_CtrlDataItem[index].iTypeOfControl) {

                case CTL_CHECKBOX :
                    m_CtrlDataItem[index].pCheckBox->SetValue(bCheckState);
                default:

                    break;
            }
        }
    }
}

//Before you dropdown, save the combo contents.
void DialogBaseClass::OnComboDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    m_sOldSelectionText = combo->GetValue();

}
void DialogBaseClass::OnComboChange( wxCommandEvent& event )
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


void DialogBaseClass::OnComboCloseUp( wxCommandEvent& event )
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