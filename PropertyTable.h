
//This is a system table that that allows you to define a user table.
#ifndef DBWORKS_PROPERTYTABLE_H
#define DBWORKS_PROPERTYTABLE_H
class HtmlHelp;

class PropertyTable : public GenericTable
{
    private:

        //Variables that have set functions
        //--group--
        wxString m_sDatabase; //Database connection variables
        wxString m_sUser;
        wxString m_sServer;
        wxString m_sPassword;
        //----------------
        wxString m_sGridTableName; //The name of the table that this property grid is working on.

        //Pointers
        HtmlHelp *m_HtmlWin;

        bool SynFieldDefinitionToTable();
        void AlterTable(int row);

        wxString PrepareCreateQuery(); //Prepare the creation query
        wxString PrepareDeleteQuery(); //Prepare the delete query

    protected:
        void OnbCreateTable( wxCommandEvent& event );
        void OnMyEvent(MyEvent& event);
        virtual void OnbHelp( wxCommandEvent& event ) override;

    public:
        void OnbDeleteTable( wxCommandEvent& event );
        //Constructor
        explicit PropertyTable( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

       // inline virtual bool CheckDatabaseTableDefinitionsToMatchGrid();
        bool Create() override; //Create this window.
    
        //Set function variables
        void SetSettings(wxString& database, wxString& server, wxString& user, wxString& password) override; // Set Database settings.
        void SetGridTableName(wxString& name) override; //Set the table that this property grid is working on

        virtual bool Destroy() override;
        ~PropertyTable() override;
    
    wxDECLARE_EVENT_TABLE();
};

#endif //DBWORKS_PROPERTYTABLE_H