
#include <wx/wxprec.h>
#include<wx/wx.h>

//Add my own classes here


#include "../Shared/global.h"
#include "../MyEvent.h"
#include "../Shared/Utility.h"

#include "RecordScrollDisplay.h"

wxBEGIN_EVENT_TABLE(RecordScrollDisplay, wxWindow)

wxEND_EVENT_TABLE()


RecordScrollDisplay::RecordScrollDisplay(wxWindow* _parent,wxWindowID _ID,wxPoint _pos,wxSize _size,long _style) : wxWindow( _parent, _ID, _pos, _size, _style )
{
    m_iRecordIndex=0;
    //Connect(wxEVT_COMMAND_SCROLLBAR_UPDATED,wxScrollEventHandler( RecordScrollDisplay::ScrollChanged ), nullptr, this );
    Connect(wxEVT_SCROLLWIN_THUMBTRACK,wxScrollEventHandler( RecordScrollDisplay::ScrollChanged ), nullptr, this );
}
RecordScrollDisplay::~RecordScrollDisplay()
{

}
void RecordScrollDisplay::ScrollChanged(wxScrollEvent& event) {
    int newPos = GetScrollPos(wxVERTICAL);

    if (newPos != m_iRecordIndex) {

        m_iRecordIndex = newPos;

        MyEvent my_event(this);
        my_event.m_bButtonClicked = true;
        my_event.m_sTableId = m_asRecordID[m_iRecordIndex]; // Pass the current tableId to the run engine.
        GetParent()->ProcessWindowEvent(my_event);
    }
}

void RecordScrollDisplay::LoadAllRecordID(wxString sTableName)
{
    Utility::LoadStringArrayWithAllIDSFromTable(sTableName, m_asRecordID);
}

wxString RecordScrollDisplay::GetCurrentRecordID()
{
    if(m_asRecordID.GetCount()>0)
        return m_asRecordID[m_iRecordIndex];

    return "";
}