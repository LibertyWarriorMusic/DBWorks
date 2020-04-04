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
class ArrayTableField;
//class PropertyTable;

//Declare an array of drawing objects. This respresents all our defined tables as an object that will be drawn on the screen.
WX_DECLARE_OBJARRAY(ObTable, ArrayDrawObjects);

class ObTablePanel : public wxPanel
{
private:

    wxPoint m_MousePos;
    ObTable* m_pObCurrentTable;
    ArrayDrawObjects m_ObTableList; // Holds a list of all our objects
    ObTable * GetObjectHitByMouse(wxPoint mousePt);

    ObTable * m_pObjectToDraw;

    wxSize m_MouseMoveOffset; // When you click on an object, you don't want the top left corner of the object to snap to the mouse point when you drag it. You want it to remain relative to where you clicked on the object


    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);
    void OnMenuAddTable(wxCommandEvent& event);
    void OnMenuEditTable(wxCommandEvent& event);
    void OnMenuRemoveTable(wxCommandEvent& event);

public:
    ObTablePanel(wxFrame* parent);
    ~ObTablePanel();

    void Refresh();

    void RemoveObjectFromDiagramByTableName(wxString sTableName);

    void AddDrawObject( const wxString& sTableID, const wxString& sTableName);
    ObTable* AddNewTableObject();

    ObTable* DoesTableObjectExist(wxString sTableName);

    void LoadTableObjects(const wxString& sDatabase); // Loads all the tables from this database
    void RedrawTableObjects();

    void GetSelectedFieldItemListDB(ArrayTableField& fieldItemList, wxString TableID);

    void paintEvent(wxPaintEvent & evt);

    wxString getResult();
    
    void render(wxDC& dc);
    
    // some useful events


    bool Destroy() override;
    DECLARE_EVENT_TABLE()
};
#endif /* ObTablePanel_hpp */