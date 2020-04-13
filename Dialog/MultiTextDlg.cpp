//
// Created by Nicholas Zounis on 23/3/20.
//

#define BORDERWIDTH 3
#define TEXTWIDTH 140
#define ALLOW_TO_GROW 1

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Utility.h"
#include "../global.h"


#include "MultiTextDlg.h"

MultiTextDlg::MultiTextDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ), wxDEFAULT_FRAME_STYLE | wxID_OK )
{

    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_MULTI_TEXT, sFieldTitle, "", "ID_TEXT");

    //Render all the controls to the mainframe sizer.
    RenderAllControls();

}
