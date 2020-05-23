//
// Created by Nicholas Zounis on 3/4/20.
//


#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Shared/global.h"
#include "../Dialog/DlgTableSelect.h"
#include "../Shared/Utility.h"

#include "../HtmlHelp.h"
#include "ObTablePanel.h"// This is the panel attached to this object

#include "TableDiagramFrame.h"

enum {
    ID_HELP = wxID_HIGHEST + 1100,
    ID_TOOL_ADD
};

wxBEGIN_EVENT_TABLE(TableDiagramFrame, wxFrame)
                EVT_TOOL(ID_HELP, TableDiagramFrame::OnbHelp)
                EVT_TOOL(ID_TOOL_ADD, TableDiagramFrame::OnbPasteTable)
                EVT_MYEVENT(TableDiagramFrame::OnMyEvent)
wxEND_EVENT_TABLE()

TableDiagramFrame::TableDiagramFrame( DiagramFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DiagramFrame( parent, id, title, pos, size, style )
{

    m_HtmlWin = nullptr;
    m_TableDiagramPanel = nullptr;


    m_TableDiagramPanel = new ObTablePanel(this); //This are our drawing functions.
    AttachPanel(m_TableDiagramPanel);
    //======
    // Setup the toolbar
    wxInitAllImageHandlers(); //You need to call this or the images will not load.

    wxBitmap BitMap;

    m_pToolbar = this->CreateToolBar();
    Utility::LoadBitmap(BitMap,"help.png");
    m_pToolbar->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));

    Utility::LoadBitmap(BitMap,"add.png");
    m_pToolbar->AddTool(ID_TOOL_ADD, wxT("Paste existing table to diagram."), BitMap, wxT("Paste existing table to diagram."));

    m_pToolbar->Realize();
    this->SetToolBar(m_pToolbar);
}

TableDiagramFrame::~TableDiagramFrame()
{
    if(m_TableDiagramPanel!= nullptr)
        m_TableDiagramPanel->Destroy();
}

bool TableDiagramFrame::Destroy()
{

    if (m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    bool bResult = DiagramFrame::Destroy();


    MyEvent my_event( this );
    my_event.m_bTableDiagramFrameWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}
void TableDiagramFrame::OnbPasteTable( wxCommandEvent& event ) {

    DlgTableSelect * pDlg = new DlgTableSelect(this, -1, "Import MySQL Database", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);

    if(pDlg->ShowModal()==wxOK){

        wxString sTableName = pDlg->GetDataValue("ID_SELECT_TABLE");
        if(!sTableName.IsEmpty())
        {
            wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,sTableName);
            //wxMessageBox("The you selected was: " + sTable);
            AddDrawObject(sTableId,sTableName);

        }
    }

    pDlg->Destroy();
}

void TableDiagramFrame::OnbHelp( wxCommandEvent& event )
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(2); //
    m_HtmlWin->Show(true);
}

void TableDiagramFrame::LoadTableObjects(const wxString& sDatabase)
{
   m_TableDiagramPanel->LoadTableObjects(sDatabase);
   this->SetSize(m_TableDiagramPanel->GetSizeDiagramExtend());
   this->Show();
}

void TableDiagramFrame::AddDrawObject( const wxString& sTableID, const wxString& sTableName)
{
    m_TableDiagramPanel->AddTableObject( sTableID, sTableName);
}

void TableDiagramFrame::OnMyEvent(MyEvent& event) {

    if(event.m_bStatusMessage){
        SetStatusText(event.m_sData);
    }
    else if(event.m_bAddTableObject){
        m_TableDiagramPanel->AddTableObject( event.m_sTableId, event.m_sTableName);
    }
    else if(event.m_bHelpFrameWasDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.

    } // Send all of the following events to the main frame.
    else if(event.m_bTableFieldDefinitions || event.m_bOpen || event.m_bEdit){
        GetParent()->ProcessWindowEvent( event );
    }
}