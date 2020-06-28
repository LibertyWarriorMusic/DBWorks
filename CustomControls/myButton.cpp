//
// Created by Nicholas Zounis on 26/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/control.h>

#include "myButton.h"


BEGIN_EVENT_TABLE(myButton, wxPanel)

END_EVENT_TABLE()


myButton::myButton(wxWindow* parent, wxString title) :
        wxButton(parent, wxID_ANY, title)
{
    m_sFormId="";
    m_sBuildQuery="";
}

void myButton::SetAction(wxString sAction)
{
    m_sAction = sAction;
}

wxString myButton::GetAction()
{
  return m_sAction;
}

void myButton::SetFormId(wxString sId)
{
    m_sFormId = sId;
}
void myButton::SetBuildQuery(wxString sQuery)
{
    m_sBuildQuery = sQuery;
}
wxString myButton::GetBuildQuery()
{
    return m_sBuildQuery;
}

wxString myButton::GetFormId()
{
    return m_sFormId;
}
