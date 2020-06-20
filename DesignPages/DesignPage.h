//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_DesignPage_H
#define DBWORKS_DesignPage_H
#include<wx/wxhtml.h>
class HtmlHelp;
class DiagaramFrame;
class DesignPagePanel;

#include "../TableDefDiagram/DiagramFrame.h"

class DesignPage : public DiagramFrame
{
    wxDECLARE_EVENT_TABLE();
private:
    wxComboBox * m_pComboSelectCtl;
    MainRunPage *m_pMainRunPage;
    wxSize m_PageSize;
    DesignPagePanel * m_pDesignPageDiagramPanel; //Used for drawing
    void AddDrawObject( const wxString& sFormID);
    HtmlHelp *m_HtmlWin;
    void OnMyEvent(MyEvent& event );
    void OnbHelp( wxCommandEvent& event );
    void OnbAddItem( wxCommandEvent& event );
    void OnRunPage( wxCommandEvent& event );
    void OnSelectCtlChange( wxCommandEvent& event );
    void OnSelectCtlDropDown( wxCommandEvent& event );
    void OnSizePage(wxSizeEvent& event);
public:
    explicit DesignPage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);
    ~DesignPage() override;

    void LoadFormObjects(); //Reload all the drawing objects
    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;
    void SetPageID(wxString sPageId);
};


#endif //DBWORKS_DesignPage_H
