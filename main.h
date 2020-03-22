
#include "HtmlHelp.h"

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
    private:
    wxDECLARE_EVENT_TABLE();

        wxString GetSelectedRowTable(); //Get the current selected grid row from the database
        void OnButtonAction( wxCommandEvent& event );
        DBGrid* m_MainGrid;
        wxToolBar * m_Toolbar1;
        HtmlHelp *m_HtmlWin;

        bool m_bSettingsLoaded=false;


        wxBoxSizer* mainFormSizerForGrid;

        wxString m_sDefaultUsergroupFilterCondition; // Depending on the type of user, you may not have access to certain table types. Example a guest can't see system table types.

        GenericItemForm *m_FormItem{};
        void OpenForm(wxString sTableName, wxString sTableID); //Open the user data form
        void OpenEditForm(int sRow); //Open the user data form

        static void ExecuteQuery(const wxString& queryString); //Execute a query string
        wxString PrepareDeleteQuery(); //Prepare the delete query

        int iOldComboIndex;
        void OnUserGroupComboChange(wxCommandEvent& event);
        void OnUserGroupComboDropDown( wxCommandEvent& event );
        void OnDatabaseComboChange(wxCommandEvent& event);
        void OnDatabaseComboDropDown( wxCommandEvent& event );
        void LoadGrid(); //Load or reload the grid from the database.

        void CheckIfTableDefinitionsMatchDatabaseTable();
        void CheckCellTableDefinitionsMatchDatabaseTable(wxString sTable, wxString iTableId, int iRowIndex);

        // The default filter for different usertypes.
        wxString GetUserWhereCondition();
        void SetGridWhereCondition(wxString whereToBlend="");

        void SetUsergroupWindowVisibility();
        void Refresh();
        void PopulateToolbar();

    protected:

        wxStatusBar* m_StatusBar;
        wxMenuBar* m_Menubar;
        wxMenu *m_FileMenu;
        GenericTable * m_TableForm;
        wxComboBox * m_DatabaseCombo;
        wxComboBox * m_UserGroupCombo;
        wxStaticText * m_txtCltUserGroup;
    public:

        explicit MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

        //Event functions
        void Quit( wxCommandEvent& event );
        void NewFile( wxCommandEvent& event );
        void OpenFile( wxCommandEvent& event );
        void SaveFile( wxCommandEvent& event );
        void SaveFileAs( wxCommandEvent& event);
        void CloseFile( wxCommandEvent& event );
        void OnbAddItem( wxCommandEvent& event );
        void OnbEditItem( wxCommandEvent& event );
        void OnbDeleteItem( wxCommandEvent& event );
        void OnbViewItem( wxCommandEvent& event );
        void OnbHelp( wxCommandEvent& event );
        static void CreateToolBars();

        void SetSettingsLoaded(bool bSettingsLoadedFlag);

        bool Destroy() override;

    
        void OnMyEvent(MyEvent& event);
        ~MainFrame() override;

};

// The main app class that launcheds DBWorks
class MyApp: public wxApp
{
private:
    MainFrame *m_MainFrame;
    static bool LoadAppSettings();
    static void ProcessLine(wxString line);
public:
    bool OnInit() override;
};

