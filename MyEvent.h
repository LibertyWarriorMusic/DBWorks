#pragma once
#include <wx/wx.h>

class MyEvent : public wxEvent
{
    public:
    
    //We can create our own variables to pass onto parent windows
    int m_iRow;
    int m_iCol;

    wxString m_sTableName;
    wxString m_sDatabaseName;
    wxString m_sNewDatabaseName;
    long m_lTableID;

    //These flags indicate which context menu we came from
    bool m_bTableFieldDefinitions;
    bool m_bOpen;
    bool m_bEdit;
    bool m_bRefreshDatabase; //Refresh the entire grid reloading every record.
    bool m_bDestroyed;
    bool m_bParseDocument;
    bool m_bShowAll;
    bool m_bImportDatabase;

    wxString m_cellValue; // The value of the current clicked cell.
    wxString m_cellFieldName; // The field name for that cell
    wxString m_sWhereCondition;

    explicit MyEvent( wxWindow* win = (wxWindow*) nullptr );
    wxEvent* Clone() const override {return new MyEvent(*this);}

    DECLARE_DYNAMIC_CLASS( MyEvent )
};

typedef void (wxEvtHandler::*myEventFunction)(MyEvent&);


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE( myEVT_MYEVENT, 1 )
END_DECLARE_EVENT_TYPES()



#define EVT_MYEVENT(func)                              \
DECLARE_EVENT_TABLE_ENTRY( myEVT_MYEVENT,       \
    -1,                                    \
    -1,                                    \
    (wxObjectEventFunction)                \
    (myEventFunction) & func,              \
    (wxObject *) NULL ),    \


// End of MyEvent.h
