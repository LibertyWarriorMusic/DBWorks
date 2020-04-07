//
// Created by Nicholas Zounis on 4/4/2020.
//

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Utility.h"
#include "../global.h"

#include "DlgTableSelect.h"

DlgTableSelect::DlgTableSelect( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DialogBaseClass( parent, id, title, pos, size, style)
{
    this->SetLabel("Select a Databases.");
    SetOkLabel("Select Table");
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_COMBO_LOOKUP_NAME, "Available Tables", "Select a table to add to the diagram.", "ID_SELECT_TABLE");
    SetFlags("ID_SELECT_TABLE","SYS_HIDDEN_TABLES");

    //Render all the controls to the mainframe sizer.
    RenderAllControls();

}