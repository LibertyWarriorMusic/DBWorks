// Begin of MyEvent.cpp
#include "MyEvent.h"


IMPLEMENT_DYNAMIC_CLASS( MyEvent, wxEvent )
DEFINE_EVENT_TYPE( myEVT_MYEVENT )

MyEvent::MyEvent( wxWindow* win )
{
    m_iRow=0;//The current row value , used in DBGrid when right clicked.
    m_iCol=0;//The current column value , used in DBGrid when right clicked.
    m_bTableFieldDefinitions=false;//Indicates if the event came from a right click on properties context menu
    m_bOpen=false; // Right click open context menu.
    m_bEdit=false; // Right click edit context menu.
    m_bRefreshDatabase=false;
    m_bDestroyed=false;
    m_bParseDocument=false;
    m_bShowAll=false;
    m_bImportDatabase=false;
    m_sTableName="";
    m_sDatabaseName="";
    m_sNewDatabaseName="";
    m_lTableID=0;
    m_cellValue=""; // The value of the current clicked cell.
    m_cellFieldName=""; // The field name for that cell
    m_sWhereCondition="";
    SetEventType( myEVT_MYEVENT );
    SetEventObject( win );
}
