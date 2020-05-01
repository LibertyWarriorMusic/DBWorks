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

#include "SetFlagsDlg.h"

SetFlagsDlg::SetFlagsDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ), wxDEFAULT_FRAME_STYLE | wxID_OK | wxSTAY_ON_TOP)
{

    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_SELECTION, sFieldTitle, "", "ID_FLAGS");
    wxArrayString* arraySelectionString = GetSelectionArrayString();
    Utility::ExtractSelectionItems(*arraySelectionString,FLAG_OPTIONS);
    RenderAllControls();
}
