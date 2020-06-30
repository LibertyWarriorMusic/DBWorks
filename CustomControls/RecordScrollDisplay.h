
#ifndef DBWORKS_RecordScrollDisplay_H
#define DBWORKS_RecordScrollDisplay_H



class RecordScrollDisplay : public wxWindow
{
    wxDECLARE_EVENT_TABLE();
private:
    wxArrayString m_asRecordID; // Holds all the record ID's for the query.
    int m_iRecordIndex;
public:
    RecordScrollDisplay(wxWindow* _parent,wxWindowID _ID,wxPoint _pos,wxSize _size,long _style);
    ~RecordScrollDisplay() override;
    void LoadAllRecordID(wxString sTableName);
    wxString GetCurrentRecordID();
    void ScrollChanged(wxScrollEvent& event);


};
#endif //DBWORKS_RecordScrollDisplay_H