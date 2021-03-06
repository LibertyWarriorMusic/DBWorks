
#include "HtmlHelp.h"


///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{

public:
    myProgressControl *GetProgressControlPointer();
    void Refresh(bool bReloadTableDiagram=false);
    void UpdateProgressBar(int val);
    explicit MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    void SetProgressLabel(wxString sLabel);
    void OpenTableDefinitions(wxString sTableName);
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
    void OnbFilter( wxCommandEvent& event );
    void OnAutoCheckDefinitions(wxCommandEvent& event);
    void OnOpenTableDiagram(wxCommandEvent& event);
    void OnbHelp( wxCommandEvent& event );
    void OnbFormQuery( wxCommandEvent& event );
    void OnDeleteCurrentDatabase( wxCommandEvent& event );
    static void CreateToolBars();
    void CreateTableDefinitions(wxString sDatabase, wxString sTableName, ArrayTableFields tableFieldItemArray);
    wxComboBox * GetDatabaseComboBox();
    void SetStateOfAutoCheckDefinitions(bool bOnIsTrue = true);
    DBGrid* GetMainGrid();
    void SetSettingsLoaded(bool bSettingsLoadedFlag);
    void OnImportDatabase( wxCommandEvent& event );


    bool Destroy() override;

    void OnMyEvent(MyEvent& event);
    ~MainFrame() override;


    private:
    wxDECLARE_EVENT_TABLE();

        wxString GetSelectedRowTable(); //Get the current selected grid row from the database
        void OnButtonAction( wxCommandEvent& event );
        DBGrid* m_MainGrid;
        wxToolBar * m_Toolbar1;
        HtmlHelp *m_HtmlWin;
        MainRunPage *m_pMainRunPage;
        DesignForm * m_pDesignForm;
        DesignPage * m_pDesignPage;
        RunFormFrame * m_pRunFormFrame;
        TableDiagramFrame *m_pTableDiagaram;
        GenericTable * m_pFilters;

        void DestroyOpenWindows();

        void AddTableObject(); //Will add a new table object to the DrawPanelTable

        bool m_bSettingsLoaded=false;

        wxString GetCurrentStoredWhereCondition();
        void SetCurrentStoredWhereCondition(wxString sWhereCondition);
        wxString m_sCurrentStoredWhereCondition;

        wxBoxSizer* mainFormSizerForGrid;

        wxString m_sDefaultUsergroupFilterCondition; // Depending on the type of user, you may not have access to certain table types. Example a guest can't see system table types.

        GenericItemForm *m_pFormItem;
        void OpenForm(wxString sTableName, wxString sTableID); //Open the user data form
        void OpenEditForm(wxString sTableId); //Open the user data form
        bool CheckCreateDatabase(wxString sDatabase);
        static void ExecuteQuery(const wxString& queryString); //Execute a query string
        wxString PrepareDeleteQuery(wxString sTable); //Prepare the delete query

        int iOldComboIndex;
        void OnUserGroupComboChange(wxCommandEvent& event);
        void OnUserGroupComboDropDown( wxCommandEvent& event );
        void OnDatabaseComboChange(wxCommandEvent& event);
        void OnDatabaseComboDropDown( wxCommandEvent& event );

        void OnOpenPages( wxCommandEvent& event );
        void OnOpenForms( wxCommandEvent& event );
        void OnRunDatabase( wxCommandEvent& event );

        void LoadGrid(); //Load or reload the grid from the database.
        void LoadTableObjects(); //Reload all the drawing objects


        // The default filter for different usertypes.
        wxString GetUserWhereCondition();
        void SetGridWhereCondition(wxString whereToBlend="");

        void SetUsergroupWindowVisibility();
        void LoadDatabaseCombo();

        void OpenDesignForm(wxString sTableId, wxString sTableName);
        void OpenRunForm(wxString sTableId, wxString sTableName);
        void OpenDesignPage(wxString sTableId, wxString sTableName);

    protected:

        wxStatusBar* m_StatusBar;
        wxMenuBar* m_Menubar;
        wxMenu *m_FileMenu;
        GenericTable * m_TableForm;
        GenericTable * m_pPages;
        GenericTable * m_pForms;
        wxComboBox * m_DatabaseCombo;
        wxComboBox * m_UserGroupCombo;
        wxStaticText * m_txtCltUserGroup;
        wxStaticText * m_txtCltCheckTableTxt;
        wxStaticText * m_txtCltProgressBar;
        wxCheckBox * m_AutoCheckDefinitionsCheckCtl;
        myProgressControl * m_ProgressGauge;


};

// The main app class that launcheds DBWorks
class MyApp: public wxApp
{
private:
    MainFrame *m_MainFrame;
    MainRunPage *m_MainRunPageFrame;
    static bool LoadAppSettings();
    static void ProcessLine(wxString line);

    ArrayTableFields m_tableFieldItemArray;
    StoreQueryResult m_Res;

    //ProgressBar
    double m_ProgressStep;
    double m_ProgessCount;

    void UpdateProgressBar();

    int m_iIdleStep;
    //ArrayTableFields m_tableFieldItemArray;// Store all our tables field

    //IMPORTING DATABASE
    bool m_bImportingDatabase;
    wxString m_DatabaseToImport;
    wxString m_NewDatabaseNameToImportInto;
    int m_saTableIndex;
    int m_dataRecordIndex;

    wxArrayString *m_saTables;//This holds all the databases we need to import

    //Checking databse definition
    bool m_bCheckTableDefinitions;
    wxString m_sCellValue;
    wxString m_sTableID;

public:
    bool OnInit() override;
    void onIdle(wxIdleEvent& evt);
    bool bProgress_loop_on;
    void activateRenderLoop(bool on);
    void StartImportDatabase(wxString sDatabase, wxString sNewDatabaseName);
    void SetStateOfAutoCheckDefinitions(bool bOnIsTrue);

    void StartCheckIfTableDefinitionsMatchDatabaseTable();
    void StopCheckIfTableDefinitionsMatchDatabaseTable();
    void CheckCellTableDefinitionsMatchDatabaseTable(const wxString& sTable, const wxString& iTableId, int iRowIndex);

};

