//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_DesignForm_H
#define DBWORKS_DesignForm_H
#include<wx/wxhtml.h>
class HtmlHelp;
class DiagaramFrame;
class DesignFormPanel;
class RunFormFrame;
class GenericQueryGrid;

#include "../TableDefDiagram/DiagramFrame.h"

class DesignForm : public DiagramFrame
{
    wxDECLARE_EVENT_TABLE();
private:

    wxComboBox * m_pComboSelectCtl;
    RunFormFrame * m_pRunFormFrame;
    HtmlHelp *m_pHtmlWin;
    GenericQueryGrid * m_pQueryGrid;
    wxSize m_FormSize;

    DesignFormPanel * m_pDesignFormDiagramPanel; //Used for drawing

    void AddDrawObject( const wxString& sTableID);

    int iOldComboIndex;
    wxString m_sFormId;
    wxString m_sBuildQuery;
    wxString m_usr_queriesId;

    void OnMyEvent(MyEvent& event );
    void OnbHelp( wxCommandEvent& event );
    void OnSelectCtlChange( wxCommandEvent& event );
    void OnSelectCtlDropDown( wxCommandEvent& event );
    void OnbAddItem( wxCommandEvent& event );
    void OnEditQuery( wxCommandEvent& event );
    void OnRunForm( wxCommandEvent& event );
    void OnSizeForm(wxSizeEvent& event);

public:
    explicit DesignForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);
    ~DesignForm() override;
    void LoadControlObjects(); //Reload all the drawing objects
    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;
    void SetFormID(wxString sFormId);
    void RunQuery();
};


#endif //DBWORKS_MainRunForm_H
