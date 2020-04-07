//
//  DrawPan.hpp
//  P2
//
//  Created by Nicholas Zounis on 27/2/20.
//

#ifndef ObTablePanel_hpp
#define ObTablePanel_hpp

//#include <stdio.h>

class ObTable;
class TableField;
class TableLinkageLine;




//Declare an array of drawing objects. This respresents all our defined tables as an object that will be drawn on the screen.
//WX_DECLARE_OBJARRAY(ObTable, ArrayDrawObjects);

class ObTablePanel : public wxPanel
{
private:

    wxPoint m_MousePos;
    ObTable* m_pObCurrentTable;

    ArrayDrawObjects m_ObTableList; // Holds a list of all our objects
    ArrayTableLinkageLine m_ObLinkageList; //Holds a list of all our linkage lines.

    ObTable * GetObjectHitByMouse(wxPoint mousePt);

    ObTable * m_pObjectToDrawTEMP; //If we click on an table object in a mouse event, this is a temp object saved after the HitTest to either move the object, open properties, open form etc

    wxSize m_sizeWinObjectsExtend;//When the objects are load, this extends the diagram to the that size.

    wxSize m_MouseMoveOffset; // When you click on an object, you don't want the top left corner of the object to snap to the mouse point when you drag it. You want it to remain relative to where you clicked on the object

    //Private paint function
    void paintEvent(wxPaintEvent & evt);
    void render(wxDC& dc);

    //Draw table linkage and associated functions
    void DrawTableLinkageLines(wxDC& dc);
    void GenerateLinkageLineArrayForDrawing(); // Searches each table object and looks for  SELECTION_LOOKUP_NAME SELECTION_LINKED_NAME SELECTION_LOOKUP_ID SELECTION_LINKED_ID fields extracting table and field information.
    ObTable * FindTableObjectByTableName(wxString sTableName); //Searches this drawing for the table object pointer.
    ObTable * FindTableObjectByTableID(wxString sTableId);//Searches this drawing for the table object pointer.
    void DetermineOTableObjectLinkageSide(TableLinkageLine *pObLink);
    void DetermineOTableObjectLinkageOffsets(TableLinkageLine *pObLink);

    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseDoubleClick(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);
    void OnMenuOpenTable(wxCommandEvent& event);
    void OnMenuAddTable(wxCommandEvent& event);
    void OnMenuEditTableDefinitions(wxCommandEvent& event);
    void OnMenuEditTable(wxCommandEvent& event);
    void OnMenuRemoveTable(wxCommandEvent& event);
    void GetSelectedFieldItemListDB(ArrayTableField& fieldItemList, wxString TableID, wxArrayString &linkageFlags);
    void RemoveObjectFromDiagramByTableName(wxString sTableName);

public:
    ObTablePanel(wxFrame* parent);
    ~ObTablePanel();
    void DeleteDrawObject();

    wxSize GetSizeDiagramExtend();//Used to determine the size of the diagram based on the object, we will probably save this info in the database later.

    void AddTableObject( const wxString& sTableID, const wxString& sTableName);
    ObTable* AddNewTableObject();
    ObTable* DoesTableObjectExist(wxString sTableName);

    void LoadTableObjects(const wxString& sDatabase); // Loads all the tables from this database
    void RedrawTableObjects();
    void Refresh();

    wxString getResult();

    bool Destroy() override;
    DECLARE_EVENT_TABLE()
};
#endif /* ObTablePanel_hpp */