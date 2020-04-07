


class DBGrid :
    public wxGrid
{
   wxDECLARE_EVENT_TABLE();
private:
    
    long m_eventType; // Used to initiate a refresh of the grid
    
    int m_iRow; //Used when we right click on a cell to store the cell position
    int m_iCol;
    int m_iDocumentColumn; //If we found a DOCUMENT flag, this store the column where the document is located, else it's -1;

    int m_iGetCellRowIndex; //Used with the GetfirstCellValue and GetNextCellValue functions to keep track of the cells.
    int m_iGetCellColumnNumber; //Used with the GetfirstCellValue and GetNextCellValue functions to remember the cell column number.

    wxString m_sTableName; //The name of the table this grid represents
    wxString m_sPrimaryKeyName;  //The table file name that stores the key ID of the table, used for deleting etc.
    wxString m_sWhereCondition;// A where condition to filter the grid if required

    ArrayGridItem m_GridArray; //Stores the information that builds the grid, field, title flags extra.
    int HasRowDocumentFlag(int iRow);

    void DeleteGridRows();
    
    void OnSizeGridSpreadSheet( wxGridEvent& event );
    void OnGridEvents(wxGridEvent& event);

    //Context Menu Event Functions
    void OnClickProperties(wxCommandEvent& event);
    void OnClickOpen(wxCommandEvent& event);
    void OnClickOpenDocument(wxCommandEvent& event);
    void OnClickEdit(wxCommandEvent& event);
    void OnClickMenuFilter(wxCommandEvent& event);
    void OnClickMenuFilterShowAll(wxCommandEvent& event);
public:
    DBGrid(wxWindow* _parent,wxWindowID _ID,wxPoint _pos,wxSize _size,long _style);
    ~DBGrid() override;

    void AddItem(const wxString& fieldTitle, const wxString& field, const wxString& flags,const wxString& defaultVal, const wxString& fieldType, const wxString& fieldNull, const wxString& fieldKey,const wxString& fieldExtra );

    bool GetGridItemArray(ArrayGridItem &GridItemList); //Loads an Array of GridItem class. Does a copy can creates all new row items.
    bool GetFirstRowCellValue(wxString& sCellValue,int  ColumnNumber);
    bool GetNextRowCellValue(wxString& sCellValue);
    int GetCurrentRowIndex();
    wxString GetCurrentRowValue(int iColumnNumber);

    void HighlightCell(int iRow, int iCol);
    void UnHighlightCell(int iRow, int iCol);
    bool IsCellHighlighted(int iRow, int iCol);

    void SetGridWhereCondition(wxString sWhere);
    bool LoadGridFromDatabase(bool bCheckTableExists=false, wxString queryToApply=""); //Loads the grid with data from the m_sTableName table;
    bool LoadGridRowFromDatabase(int m_gridRow, bool bCheckTableExists=false); //Loads the grid with data from the m_sTableName table;
    void ResizeSpreadSheet(); //Resises the fields to the text with respect to max and min size limits.



    void HideColumn(int colNumber);
    
    void SetSettings(const wxString& database, const wxString& server, const wxString& user, const wxString& password, const wxString& tableName, const wxString& KeyName, const wxString& whereCondition);
 
    void DeleteSelectedRow();
    void DeleteEntryFromDatabase(const wxString& contactId);
    void CreateFields(); // Creates the fields of the grid, used in initalising.
    

    wxString getSelectedFieldValue(const wxString& fieldname);
    void SetEventType(long type);
    // We are going to do this with every frame class so we can send messages to parents that this window may of been destroyed internally.
    bool Destroy() override;
    
protected:
    void OnGridRClick(wxGridEvent& event );
    void OnGridDLClick(wxGridEvent& event );
    void OnGridClick(wxGridEvent& event );



};
