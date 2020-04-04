//
// Created by Nicholas Zounis on 3/4/20.
//

#ifndef DBWORKS_DIAGRAMFRAME_H
#define DBWORKS_DIAGRAMFRAME_H
#include<wx/wxhtml.h>

class DiagramFrame : public wxFrame{

private:
wxDECLARE_EVENT_TABLE();
    wxToolBar * m_Toolbar1;
    wxBoxSizer* m_MainFormSizer;
    wxPanel *m_attachedPanel;
    wxStatusBar* m_StatusBar;

public:

    wxBoxSizer* GetFrameSizer();
    explicit DiagramFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);

    ~DiagramFrame() override;

    void AttachPanel(wxPanel *panel);

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;
};


#endif //DBWORKS_DIAGRAMFRAME_H
