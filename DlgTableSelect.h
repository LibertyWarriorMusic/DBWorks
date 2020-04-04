//
// Created by Nicholas Zounis on 23/3/20.
//
#ifndef DBWORKS_DlgTableSelect_H
#define DBWORKS_DlgTableSelect_H
#include "DialogBaseClass.h"

class DlgTableSelect : public DialogBaseClass{
public:
    explicit DlgTableSelect( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);
};
#endif //DBWORKS_DlgTableSelect_H
