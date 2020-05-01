//
//  DrawPan.hpp
//  P2
//
//  Created by Nicholas Zounis on 27/2/20.
//

#ifndef DesignFormPanel_hpp
#define DesignFormPanel_hpp


class ObControl;

class DesignFormPanel : public wxPanel
{
private:
    wxString m_sFormId; // The formId for this panel.
    wxString m_sBuildQuery; // The query the form is based.
    wxPoint m_MousePos;


    ObControl* m_pObCurrentFormControl;
    ArrayDrawControls m_ControlList; // Holds a list of all our objects


    ObControl * GetObjectHitByMouse(wxPoint mousePt);
    ObControl * GetObjectHitByMouse(wxPoint mousePt, wxString& sHitFieldName, wxRect& sfieldRect);
    ObControl * m_pObjectToDrawTEMP; //If we click on an table object in a mouse event, this is a temp object saved after the HitTest to either move the object, open properties, open form etc

    wxSize m_sizeWinObjectsExtend;//When the objects are load, this extends the diagram to the that size.
    wxSize m_MouseMoveOffset; // When you click on an object, you don't want the top left corner of the object to snap to the mouse point when you drag it. You want it to remain relative to where you clicked on the object

    void RemoveControlFromList(wxString sControlId);
    //Private paint function
    void paintEvent(wxPaintEvent & evt);
    void render(wxDC& dc);

    void OnDatabaseComboChange(wxCommandEvent& event);
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseDoubleClick(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);
    void OnMenuOpenControl(wxCommandEvent& event);
    void OnMenuAddControl(wxCommandEvent& event);
    void OnMenuEditLinkedFields(wxCommandEvent& event);
    void OnSetFlags(wxCommandEvent& event);
    void OnSetSelectionOptions(wxCommandEvent& event);
    void OnSetLinkedTable(wxCommandEvent& event);
    void OnMenuDeleteControl(wxCommandEvent& event);
    void OnMenuEditStatic(wxCommandEvent& event);
    void OnRunQuery(wxCommandEvent& event);

public:

    void SetFormID(wxString sFormId);
    void SetQuery(wxString sQuery);
    wxString GetQuery();

    wxString GetFormID();
    DesignFormPanel(wxFrame* parent);
    ~DesignFormPanel();
    void DeleteDrawObject();

    wxSize GetSizeDiagramExtend();//Used to determine the size of the diagram based on the object, we will probably save this info in the database later.

    void AddControlObject( const wxString& sTypeID);
    ObControl* NewCtlObject();
    //ObControl* DoesControlExist(wxString sControlId);

    void LoadControlObjects(); // Loads all the Controls from this database
    void RedrawControlObjects();
    void Refresh();

    wxString getResult();

    bool Destroy() override;
    DECLARE_EVENT_TABLE()
};
#endif /* DesignFormPanel_hpp */