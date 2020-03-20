
//I am not sure why I have to do this, but when you include this in the main, it doesn't know what these classes are.
class wxImagePanel;
class DBGrid;
class GenericItemForm;
class MyEvent;
class HtmlHelp;

class TableField
{
public:
    wxString fieldName;
    wxString title;
    wxString type;
    wxString flag;
    wxString key;
    wxString extra;
    wxString defaultValue;
};


WX_DECLARE_OBJARRAY(TableField, ArrayTableField);

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

        void  EditItem(long rowID);
        void ViewItem(long rowID);

        void OnbViewItem( wxCommandEvent& event );
        void OnbExitApp(wxCommandEvent& event);
        void OnbHelp( wxCommandEvent& event );

    public:

        virtual void OnbDeleteItem( wxCommandEvent& event );
        void SetTableDefinition(const wxString& tableName, const wxString& title, const wxString& comments, const wxString& whereCondition);
        GenericItemForm *formItem{};
    
        void AddField(const wxString& title, const wxString& field, const wxString& type, const wxString& flag,const wxString& defaultVal, const wxString& KeyVal, const wxString& ExtraVal);
        explicit GenericTable( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    
        virtual bool Create();
        virtual void SetSettings(wxString& database, wxString& server, wxString& user, wxString& password){};
        virtual void SetGridTableName(wxString& name){};

        bool Destroy() override;

        void OnMyEvent(MyEvent& event);
        wxDECLARE_EVENT_TABLE();
    
        ~GenericTable() override;

};
