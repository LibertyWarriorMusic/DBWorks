//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_DialogBaseClass_H
#define DBWORKS_DialogBaseClass_H


class DialogBaseClass : public wxDialog{

private:
    wxDECLARE_EVENT_TABLE();
    wxBoxSizer* m_MainFormSizer;
    wxButton* m_Cancel;
    wxButton* m_Import;
    FieldCtlItem* NewFieldDataCtrl();// Creates a new FieldCtlItem in the list and returns a point to that control
    ArrayFieldDataCtls m_CtrlDataItem; // Used to store information about each control
    int m_CalculatedHeightWindow;
    wxString m_sOldSelectionText; //Remember the selectiontext
    void ResizeTitleText();

    void OnbCancel( wxCommandEvent& event );
    void OnbOk( wxCommandEvent& event );
    void OnComboDropDown( wxCommandEvent& event );
    void OnComboChange( wxCommandEvent& event );
    void OnComboCloseUp( wxCommandEvent& event );
protected:
    wxBoxSizer* GetMainSizer();
    int GetCtlIndex(wxString sIdentifier); // Get the index to the control by the identifier.
public:
    DialogBaseClass( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);

    void Create();
    ~DialogBaseClass() override;


    void* GetControl(wxString sIdentifier);
    wxString GetDataValue(wxString sIdentifier); //Used to get the data for this control
    void SetDataValue(wxString sIdentifier,wxString sData); //Used to set the data for this control.
    void SetFlags(wxString sIdentifier, wxString sFlags); //Set addition flags for the data: PASSWORD - READONLY

    bool GetCheckState(wxString sIdentifier); //Used to get the data for this control
    void SetCheckState(wxString sIdentifier, bool bCheckState); //Used to get the data for this control

    //USER FUNCTIONS
    void AddCtlItem(int iTypeOfControl, wxString TitleName, wxString Description, wxString sIdentifier);
    void RenderAllControls();

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;


};
#endif //DBWORKS_DialogBaseClass_H
