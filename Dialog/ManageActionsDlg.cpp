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

ManageActionsDlg::ManageActionsDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ),  wxSTAY_ON_TOP | wxID_OK )
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_SELECTION_LOOKUP_NAME, sFieldTitle, "", "ID_ACTION");

    RenderAllControls();

}

void ManageActionsDlg::SetControlID(wxString id)
{
    m_sControlID=id;
}

void ManageActionsDlg::Load()
{
    wxString sQuery="SELECT actionName FROM usr_control_actions";
    wxArrayString ArrayFieldResults;

    if(Utility::GetSingleFieldRecords(sQuery, "actionName",ArrayFieldResults)){

        Utility::LoadComboFromStringArray((wxComboBox*)GetControl("ID_ACTION"),ArrayFieldResults);

        wxString sAction="";
        Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Action",sAction);
        SetDataValue("ID_ACTION",sAction);
    }
}

void ManageActionsDlg::Save()
{
    wxString sField = GetDataValue("ID_ACTION");
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Action",sField);
}

wxString ManageActionsDlg::GetAction()
{
    return GetDataValue("ID_ACTION");
}