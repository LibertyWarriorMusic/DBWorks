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


#include "SelectionFieldQueryDlg.h"

SelectionFieldQueryDlg::SelectionFieldQueryDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ), wxDEFAULT_FRAME_STYLE | wxID_OK | wxSTAY_ON_TOP)
{

    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_SELECTION_LOOKUP_NAME, sFieldTitle, "", "ID_FIELD");

    RenderAllControls();

}

void SelectionFieldQueryDlg::SetControlID(wxString id)
{
    m_sControlID=id;
}

void SelectionFieldQueryDlg::SetQuery(wxString sQuery)
{
    m_sQuery=sQuery;
}
void SelectionFieldQueryDlg::Load()
{
    wxArrayString saFields;
    Utility::LoadStringArrayWithFieldNamesFromQuery(saFields, m_sQuery);
    Utility::LoadComboFromStringArray((wxComboBox*)GetControl("ID_FIELD"),saFields);

    wxString sField="";
    Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Field",sField);
    SetDataValue("ID_FIELD",sField);

}

void SelectionFieldQueryDlg::Save()
{
    wxString sField = GetDataValue("ID_FIELD");
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Field",sField);

}

wxString SelectionFieldQueryDlg::GetField()
{
    return GetDataValue("ID_FIELD");
}