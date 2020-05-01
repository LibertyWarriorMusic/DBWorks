//
// Created by Nicholas Zounis on 20/4/20.
//
#ifndef DBWORKS_ObControl_H
#define DBWORKS_ObControl_H

class ObControl
{
public:

    void SetTypeName(wxString sType);
    void SetControlID(wxString sId);
    void SetTypeID(wxString sId);
    void SetFormID(wxString sId);
    void SetLabel(wxString sLabel);
    void SetField(wxString sField);

    wxString GetTypeName();
    wxString GetTypeID();
    wxString GetControlID();
    wxString GetFormID();
    wxString GetLabel();
    wxString GetField();

    ObControl();
    wxRect GetControlRect();
    void TurnSnapOff();
    void RestorePreviousSnapCondition();
    void DrawControlObject(wxDC&  dc);
    bool HitTest(wxPoint pt); //Returns true if point is in this objects rect.
    void SetControlPosition(wxPoint pt); //Save the table position of this object.

    wxString SaveDB(wxString sControlId); //Save this table to the database;
private:
    wxString m_sTypeName;
    wxString m_sFormId;
    wxString m_sTypeId;
    wxString m_sControlId;
    wxString m_sLabel;
    wxString m_sField;

    wxRect m_ControlRect;

    bool m_bSnapToGrid; // Will snap each object to a grid value; DEFAULT true
    bool m_SnapToGridRememberState; // Used if we want.
    wxSize m_gridDistance; //The distance between grid lines.  DEFAULT 1

};

#endif //DBWORKS_ObControl_H
