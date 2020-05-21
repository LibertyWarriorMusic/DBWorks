//
// Created by Nicholas Zounis on 26/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/control.h>

#include "myButton.h"


BEGIN_EVENT_TABLE(myButton, wxPanel)

END_EVENT_TABLE()


myButton::myButton(wxFrame* parent, wxString title) :
        wxButton(parent, wxID_ANY, title)
{

}

void myButton::SetAction(wxString sAction)
{
    m_sAction = sAction;
}

wxString myButton::GetAction()
{
  return m_sAction;
}
