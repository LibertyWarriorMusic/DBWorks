//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include <wx/hyperlink.h>
#include <wx/datectrl.h>

#include "../CustomControls/myButton.h"
#include "../CustomControls/RecordSelector.h"

#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "../Shared/global.h"

#include "RunForm.h"
#include "RunFormFrame.h"

wxBEGIN_EVENT_TABLE(RunFormFrame, wxFrame)
                EVT_MYEVENT(RunFormFrame::OnMyEvent)
wxEND_EVENT_TABLE()


RunFormFrame::RunFormFrame( wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_pRunForm = new RunForm((wxFrame *) this, -1, "Run Form", wxDefaultPosition, wxDefaultSize,wxDEFAULT_FRAME_STYLE);
}

RunFormFrame::~RunFormFrame(){

}

void RunFormFrame::SetFormID(wxString Id)
{
    m_pRunForm->SetFormID(Id);
}

wxString RunFormFrame::GetQuery(wxString sFormId)
{
    return m_pRunForm->GetQuery(sFormId);
}

void RunFormFrame::Create(wxString sQuery) {
    m_pRunForm->Create(sQuery);
    SetSize(m_pRunForm->GetFormSize());
}

//We can send a message to the parent that this window is destroyed.
bool RunFormFrame::Destroy()
{

    m_pRunForm->Close();


    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bRunFormWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void RunFormFrame::OnMyEvent(MyEvent& event) {

    if (event.m_bDestroyed)
        Close(true);
    else if(event.m_bRunForm){

        RunFormFrame* pRunFormFrame = new RunFormFrame((wxFrame *) this, -1, "Run Form", wxDefaultPosition, wxDefaultSize,wxDEFAULT_FRAME_STYLE);
        pRunFormFrame->SetFormID(event.m_sFormId); // The formId is used to load the form definition from the database.
        pRunFormFrame->Create(event.m_sBuildQuery);
        pRunFormFrame->Show(true);
    }
}

