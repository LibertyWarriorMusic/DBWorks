//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_RunForm_H
#define DBWORKS_RunForm_H
#include<wx/wxhtml.h>

class RecordScrollDisplay;

//Query Mode
enum {
    mode_select = wxID_HIGHEST + 3000, // m_bStartObLinkageSide  m_pEndTableObject
    mode_insert, // m_bStartObLinkageSide  m_pEndTableObject
    mode_update
};


class RunForm : public wxWindow{

private:
    wxDECLARE_EVENT_TABLE();
    wxBoxSizer* m_MainFormSizer;
    int m_CalculatedHeightWindow;
    wxArrayString m_asSelectionItemArray;
    RecordScrollDisplay *m_pRecordScrollDisplay;

    ArrayFieldDataCtls m_CtrlDataItem; // Used to store information about each control

    wxString m_sOldSelectionText; //Remember the selectiontext
    int m_iRunMode; //This form may be a select, update or insert form.

    bool m_bLoadFields; //Set to true if we have a record selector. A record selector indicates that we need to load fields.

    wxString m_sFormId;
    wxString m_sBuildQuery; // The query the form is based.
    wxArrayString m_TableList;// A list of table name from the build query
    wxString m_sCurrentId; // The current table ID for the form.
    wxSize m_FormSize;// This is the size of this form.

    //We need to get a list of all the indexes for the query this form is based upon.
    wxString GetFormIndex();
    bool GetControlIndexWithLowestYValue(int &index);

    void ResizeTitleText();
    void LoadControlObjects(); // Loads all the Controls from this database
    FieldCtlItem* NewFieldDataCtrl();// Creates a new FieldCtlItem in the list and returns a point to that control
    int GetCtlIndex(wxString sIdentifier); // Get the index to the control by the identifier.

    void LoadSelectionItemsFromUsrControlItemsTable(wxString sControlId);

    //ACTIONS
    void OnComboDropDown( wxCommandEvent& event );
    void OnComboChange( wxCommandEvent& event );
    void OnButtonClick( wxCommandEvent& event );
    void OnMyEvent(MyEvent& event);

    //This the DialogBaseClass, these are public functions and sIdentifier can be set to any value.
    // In this class, controls are generated from the form builder, so in order to identify controls, we set
    // sIdentifier = sControlID which is a unique field.
    void* GetControl(wxString sIdentifier);
    wxString GetDataValue(wxString sIdentifier); //Used to get the data for this control
    void SetDataValue(wxString sIdentifier,wxString sData); //Used to set the data for this control.
    void SetFlags(wxString sIdentifier, wxString sFlags); //Set addition flags for the data: PASSWORD - READONLY

    bool GetCheckState(wxString sIdentifier); //Used to get the data for this control
    void SetCheckState(wxString sIdentifier, bool bCheckState); //Used to get the data for this control

    void InsertItem(wxString sTableName);
    void UpdateItem(wxString sTableName, wxString sId);
    void DeleteItem(wxString sTableName, wxString sId);
    void ParseSpecialCharacters();
    void LoadFields(wxString sId);

public:
    explicit RunForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400,400), long style = wxDEFAULT_FRAME_STYLE);

    //USER FUNCTIONS
    void RenderAllControls();
    void RenderControl(int index);
    void Create(wxString sQuery);

    wxSize GetFormSize();

    wxString GetFormID();
    void SetQuery(wxString sQuery);
    wxString GetQuery(wxString sFormId="");

    void SetFormID(wxString Id);

    void SetFormMode(int mode);
    int GetFormMode();
    wxArrayString* GetSelectionArrayString();


    ~RunForm() override;

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;

};

#endif //DBWORKS_RunForm_H
