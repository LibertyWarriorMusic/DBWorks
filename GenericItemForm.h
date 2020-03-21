///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Feb 26 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef GENERICITEMFORM
#define GENERICITEMFORM

// We must forward declare the array because it is used
// inside the class declaration.
class wxHyperlinkCtrl;
class wxDatePickerCtrl;


class FieldItem
{
public:
    wxString field;
    wxString title;
    wxString flag;
    wxString type;
    wxString defaultValue;
    wxStaticText *TitleCtl;
    wxTextCtrl *textCtl;
    wxComboBox *comCtl;
    wxHyperlinkCtrl *linkCtl;
    wxDatePickerCtrl *datePickerCtl;

    FieldItem();

    ~FieldItem();
};
WX_DECLARE_OBJARRAY(FieldItem, ArrayFieldItem);

///////////////////////////////////////////////////////////////////////////
//wxDECLARE_EVENT(CUSTOM_EVENT_REFRESHGRID, wxCommandEvent);

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame2
///////////////////////////////////////////////////////////////////////////////
class GenericItemForm : public wxFrame
{
	private:
        wxString m_sUse;
        wxString m_sId;
        
        wxString m_sDatabase;
        wxString m_sUser;
        wxString m_sServer;
        wxString m_sPassword;
    
        wxString m_sTableName;
        wxString m_sKeyName;
        wxBoxSizer* m_mainFormSizer;

        wxString m_sOldSelectionText; //Remember the selectiontext
        ArrayFieldItem itemArray;

        void ParseSpecialCharacters(); //Only removes " and ' characters
        void ResizeTitleText();

        wxButton* m_OK;
        wxButton* m_Cancel;
	protected:
        wxBoxSizer* m_ButtonSizer;
    
        void OnbOK( wxCommandEvent& event );
        void OnbCancel( wxCommandEvent& event );
		void OnComboCloseUp( wxCommandEvent& event );
        void OnComboDropDown( wxCommandEvent& event );
        void OnComboChange( wxCommandEvent& event );

	void InsertItem();
        void UpdateItem();

	public:
        void CreateFields();
        void AddItem(const wxString& fieldTitle, const wxString& field, const wxString& flag,const wxString& type, const wxString& defaultVal);
        void SetUse(const wxString& sUse);
        void SetID(const wxString& pID);
        void LoadFields();
        void SetSettings(const wxString& database, const wxString& server, const wxString& user, const wxString& password, const wxString& tableName, const wxString& KeyName);

		explicit GenericItemForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~GenericItemForm() override;
        bool Destroy() override;


};

#endif
