//
// Created by Nicholas Zounis on 23/3/20.
//

#define BORDERWIDTH 3
#define TEXTWIDTH 140
#define ALLOW_TO_GROW 1

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "../Shared/global.h"


#include "ManageActionsDlg.h"

ManageActionsDlg::ManageActionsDlg(wxWindow* parent, wxString id, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ),  wxSTAY_ON_TOP | wxID_OK )
{
    m_sControlID=id;

    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_SELECTION_LOOKUP_NAME, sFieldTitle, "", "ID_ACTION");
    wxComboBox *pCom = (wxComboBox*)GetControl("ID_ACTION");
    pCom->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( ManageActionsDlg::OnActionComboChange ), nullptr, this );

    wxString sQuery="SELECT actionName FROM usr_control_actions";
    wxArrayString ArrayFieldResults;

    if(Utility::GetSingleFieldRecords(sQuery, "actionName",ArrayFieldResults)){

        Utility::LoadComboFromStringArray((wxComboBox*)GetControl("ID_ACTION"),ArrayFieldResults);

        wxString sAction="";
        Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Action",sAction);
        SetDataValue("ID_ACTION",sAction);

        CheckActionChange(sAction);
    }

    RenderAllControls();
}

void ManageActionsDlg::OnActionComboChange( wxCommandEvent& event ) {

    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    m_sSelectedAction = combo->GetStringSelection();
    CheckActionChange(m_sSelectedAction);

}

void ManageActionsDlg::CheckActionChange(wxString sAction)
{
    if(sAction=="ACTION_OPEN_FORM"){

        if(!HasCtlItem("ID_FORM")){
            AddCtlItem(CTL_SELECTION_LOOKUP_NAME, "Form", "", "ID_FORM");
            RenderNewControl();

            //Load the new control with values
            wxString sQuery="SELECT formName FROM usr_forms";
            wxArrayString ArrayFieldResults;

            if(Utility::GetSingleFieldRecords(sQuery, "formName",ArrayFieldResults)) {
                //Fill the combo with all the form names
                Utility::LoadComboFromStringArray((wxComboBox*)GetControl("ID_FORM"),ArrayFieldResults);

                //See if we have a form saved, then select that form.
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_sControlID,"ActionFormToOpen",sAction);
                SetDataValue("ID_FORM",sAction);
            }
            this->Layout();
        }
    }
    else {
        //Removed the controls for form
        RemoveCtlItem("ID_FORM");
    }
}

void ManageActionsDlg::Save()
{
    wxString sAction = GetDataValue("ID_ACTION");
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Action",sAction);
    if(sAction=="ACTION_OPEN_FORM"){
        sAction = GetDataValue("ID_FORM");
        Utility::SaveTableData(Settings.sDatabase,"usr_controls",m_sControlID,"ActionFormToOpen",sAction);
    }
}

wxString ManageActionsDlg::GetAction()
{
    return GetDataValue("ID_ACTION");
}