//
// Created by Nicholas Zounis on 3/4/20.
//

#ifndef DBWORKS_TABLEDIAGRAMFRAME_H
#define DBWORKS_TABLEDIAGRAMFRAME_H

#include "DiagramFrame.h"

class ObTablePanel;

class TableDiagramFrame : public DiagramFrame
{
public:
    TableDiagramFrame( DiagramFrame* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);
    ~TableDiagramFrame();

    void LoadTableObjects(const wxString& sDatabase); //Reload all the drawing objects
    void AddDrawObject( const wxString& sTableID, const wxString& sTableName);
    bool Destroy() override;
   // void OpenTableDefinitions(wxString sTableName);
private:


    //PropertyTable * m_TableForm;


    wxDECLARE_EVENT_TABLE();
    ObTablePanel * m_TableDiagramPanel; //Used for drawing
    HtmlHelp *m_HtmlWin;
    void OnbHelp( wxCommandEvent& event );

    void OnMyEvent(MyEvent& event);
    void OnbPasteTable( wxCommandEvent& event );

};


#endif //DBWORKS_TABLEDIAGRAMFRAME_H
