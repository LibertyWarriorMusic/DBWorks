
//I am not sure why I have to do this, but when you include this in the main, it doesn't know what these classes are.
class wxImagePanel;
class DBGrid;
class GenericItemForm;
class MyEvent;
class HtmlHelp;



//WX_DECLARE_OBJARRAY(TableField, ArrayTableField);

//This is a generic table form that will allow you to add - edit - delete entries in a user table

class GenericTable : public wxFrame
{
    private:
    
        //We are going to create two panels and then
        // attach them to a sizer.
        wxString m_sTitle;
        wxString m_sComments;
        wxString m_sWhereCondition;
        wxString m_sTableName;
        //Pointers
        HtmlHelp *m_HtmlWin;
        wxStaticText *m_txtFilter;
        wxComboBox *m_ComboFilter;

        wxString GetCurrentStoredWhereCondition();
        void SetCurrentStoredWhereCondition(wxString sWhereCondition);
        wxString m_sCurrentStoredWhereCondition;

        int m_iTempRowIndex; // This is the row index that needs to be updatad when add or edit a field.
        wxString m_sTempQuery; // Used to store the query to pass onto the grid.

        void Refresh();

        void OnParseDocument(wxString sDocument);

    void OnButtonAction( wxCommandEvent& event );
      
    protected:

        wxStatusBar* m_StatusBar;
        wxButton* m_Close;
        DBGrid* m_Grid;
        wxImagePanel * m_DrawPane;
        wxToolBar * m_Toolbar;
        wxBoxSizer* m_MainFormSizer;
    
        ArrayTableField m_FieldArray;

        void SetGridWhereCondition(wxString whereToBlend="");

        //Button Handle functions
        void OnbAddItem( wxCommandEvent& event );
        void OnbEditItem( wxCommandEvent& event );

        void AddItem(long rowID);
        void  EditItem(long rowID);
        void ViewItem(long rowID);

        void OnUserGroupComboChange(wxCommandEvent& event);
        void OnbViewItem( wxCommandEvent& event );
        void OnbExitApp(wxCommandEvent& event);
        virtual void OnbHelp( wxCommandEvent& event );



    public:

        virtual void OnbDeleteItem( wxCommandEvent& event );
        void SetTableDefinition(const wxString tableName, const wxString title, const wxString comments, const wxString whereCondition);
        GenericItemForm *formItem{};
    
        void AddField(const wxString& title, const wxString& field, const wxString& type, const wxString& flag,const wxString& defaultVal, const wxString& KeyVal, const wxString& ExtraVal, const wxString &nullVal);
        explicit GenericTable( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    
        virtual bool Create();
        virtual void SetSettings(wxString& database, wxString& server, wxString& user, wxString& password){};
        virtual void SetGridTableName(wxString& name){};

        bool Destroy() override;
        void HideIDColumn();
        void SetIDTitleName(wxString sTitle);

        void OnMyEvent(MyEvent& event);
        wxDECLARE_EVENT_TABLE();
    
        ~GenericTable() override;

};
