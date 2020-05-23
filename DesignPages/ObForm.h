//
// Created by Nicholas Zounis on 20/4/20.
//
#ifndef DBWORKS_ObForm_H
#define DBWORKS_ObForm_H

class ObForm
{
public:
    ObForm();
    wxRect GetFormRect();
    void DrawFormObject(wxDC&  dc);

    void SetFormName(wxString sFormName);
    void SetFormID(wxString sFormId);
    wxString GetFormID();
    void SetPageID(wxString sPageId);
    void SetFormPosition(wxPoint pt); //Save the table position of this object.
    bool HitTest(wxPoint pt);
    void TurnSnapOff();
    void RestorePreviousSnapCondition();
    wxString SaveDB(wxString sFormId); //Save this table to the database;
private:
    wxRect m_FormRect;
    wxString m_sFormId;
    wxString m_sPageId;
    wxString m_sFormName;


    bool m_bSnapToGrid; // Will snap each object to a grid value; DEFAULT true
    bool m_SnapToGridRememberState; // Used if we want.
    wxSize m_gridDistance; //The distance between grid lines.  DEFAULT 1




};

#endif //DBWORKS_ObForm_H
