//
//  DrawPan.cpp
//  P2
//
//  Created by Nicholas Zounis on 27/2/20.
//

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/gbsizer.h>
#include <wx/minifram.h>
#include <wx/sizer.h>
#include "wx/wx.h"
#include "wx/sizer.h"


#include "../global.h"
#include "../Utility.h"
#include "../MyEvent.h"
#include "../Dialog/DlgTableSelect.h"

#include "ObTable.h"
#include "mysql.h"
#include "mysql++.h"

#include "ObTablePanel.h"
#include <cmath>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayDrawObjects);
WX_DEFINE_OBJARRAY(ArrayTableLinkageLine);

using namespace mysqlpp;

enum {
    ID_MENU_ADD_TABLE = wxID_HIGHEST + 600,
    ID_MENU_EDIT_TABLE_DEFINITIONS,
    ID_MENU_EDIT_TABLE,
    ID_MENU_REMOVE_TABLE
};

BEGIN_EVENT_TABLE(ObTablePanel, wxPanel)
// some useful events

    EVT_MOTION(ObTablePanel::mouseMoved)
    EVT_LEFT_DOWN(ObTablePanel::mouseDown)
    EVT_LEFT_DCLICK(ObTablePanel::mouseDoubleClick)
    EVT_LEFT_UP(ObTablePanel::mouseReleased)
    EVT_RIGHT_DOWN(ObTablePanel::rightClick)
    EVT_LEAVE_WINDOW(ObTablePanel::mouseLeftWindow)
    EVT_KEY_DOWN(ObTablePanel::keyPressed)
    EVT_KEY_UP(ObTablePanel::keyReleased)
    EVT_MOUSEWHEEL(ObTablePanel::mouseWheelMoved)
    EVT_MENU(ID_MENU_ADD_TABLE, ObTablePanel::OnMenuAddTable)
    EVT_MENU(ID_MENU_REMOVE_TABLE, ObTablePanel::OnMenuRemoveTable)
    EVT_MENU(ID_MENU_EDIT_TABLE_DEFINITIONS, ObTablePanel::OnMenuEditTableDefinitions)
    EVT_MENU(ID_MENU_EDIT_TABLE, ObTablePanel::OnMenuEditTable)

// catch paint events
    EVT_PAINT(ObTablePanel::paintEvent)

END_EVENT_TABLE()

ObTablePanel::ObTablePanel(wxFrame* parent) : wxPanel(parent)
{
    m_pObjectToDrawTEMP = nullptr;
    m_pObCurrentTable = nullptr;
    m_MouseMoveOffset.x=0;
    m_MouseMoveOffset.y=0;
    m_sizeWinObjectsExtend.x=0;
    m_sizeWinObjectsExtend.y=0;
}

ObTablePanel::~ObTablePanel()
{
    DeleteDrawObject();
}

void ObTablePanel::DeleteDrawObject()
{
    /* You don't need to do this I believe, I haven't yet checked if the program leaks memory, but if you do this the program crashes.
     // I think the object array deletes the objects when it is destroyed.
    int count = m_ObTableList.GetCount();
    if(m_ObTableList.GetCount()>0){
        for (int index=0; index<m_ObTableList.GetCount();index++)
            delete &m_ObTableList[index];
    }
     */
}


wxSize ObTablePanel::GetSizeDiagramExtend()
{
    wxSize size = m_sizeWinObjectsExtend;
    size.x = m_sizeWinObjectsExtend.x + 300;
    size.y = m_sizeWinObjectsExtend.y + 300;
    return size;
}
void ObTablePanel::rightClick(wxMouseEvent& event)
{
    m_pObCurrentTable= nullptr;

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObTable* pObTable = GetObjectHitByMouse(m_MousePos);
    //Diagram menus
    //Create a context menu to do stuff here.
    auto *menu = new wxMenu;

    if(pObTable!= nullptr)
    {
        m_pObCurrentTable = pObTable;


        //Object menus
       // m_MouseMoveOffset=Utility::CalcPtInRectOffset(m_MousePos,pObTable->GetObjectRect());
       // m_pObjectToDraw = pObTable;
        //m_pObjectToDraw->TurnSnapOff();
        menu->Append(ID_MENU_EDIT_TABLE_DEFINITIONS, wxT("Edit table definitions"), wxT("Edit table definitions."));
        menu->Append(ID_MENU_EDIT_TABLE, wxT("Edit table"), wxT("Edit the table."));
        menu->Append(ID_MENU_REMOVE_TABLE, wxT("Remove this table from the database"), wxT("Edit table definitions."));

    }
    else{

        menu->Append(ID_MENU_ADD_TABLE, wxT("Add table"), wxT("At table at current position."));
        // menu->AppendSeparator();

    }


    PopupMenu( menu, m_MousePos);


    //Tell the parent to add a table to the drawing at the mouse point.
    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
//    my_event.m_sData = "Current mouse position: x="+m_MousePos.x" + " y="+m_MousePos.y;
    my_event.m_pMousePoint=m_MousePos;
    GetParent()->ProcessWindowEvent( my_event );

}



// some useful events

 void ObTablePanel::mouseMoved(wxMouseEvent& event)
 {
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw


    if (m_pObjectToDrawTEMP != nullptr){

        wxPoint pos;

        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;
        m_pObjectToDrawTEMP->SetObjectPosition(pos);

        //This is going to be too show. We need to only move the object.
        wxClientDC dc(this);
        m_pObjectToDrawTEMP->DrawObject(dc);
        DrawTableLinkageLines(dc);

    }

 }
void ObTablePanel::mouseDoubleClick(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw


    ObTable* pObTable = GetObjectHitByMouse(m_MousePos);
    if(pObTable!= nullptr)
    {
        MyEvent my_event( this );
        my_event.m_bOpen=true;
        my_event.m_sTableName=pObTable->GetTableName();
        my_event.m_sTableId=m_pObCurrentTable->GetTableID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}
// Testing draw the position when the mouse is down.
 void ObTablePanel::mouseDown(wxMouseEvent& event) {

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw


    ObTable* pObTable = GetObjectHitByMouse(m_MousePos);
    if(pObTable!= nullptr)
    {
        m_MouseMoveOffset=Utility::CalcPtInRectOffset(m_MousePos,pObTable->GetObjectRect());
        m_pObjectToDrawTEMP = pObTable;

        m_pObjectToDrawTEMP->TurnSnapOff();

    }


    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_sData="mouseDown";
    GetParent()->ProcessWindowEvent( my_event );

}

 void ObTablePanel::mouseWheelMoved(wxMouseEvent& event)
 {

     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseWheelMoved";
     GetParent()->ProcessWindowEvent( my_event );
 }

 void ObTablePanel::mouseReleased(wxMouseEvent& event)
 {
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

        if(m_pObjectToDrawTEMP!=nullptr){

            //Restore the snap condition to what is was before we moved the object
            // We don't want snap on because it results in clunky movement. v
            m_pObjectToDrawTEMP->RestorePreviousSnapCondition();

            wxPoint pos;

            pos.x = m_MousePos.x - m_MouseMoveOffset.x;
            pos.y = m_MousePos.y - m_MouseMoveOffset.y;
            m_pObjectToDrawTEMP->SetObjectPosition(pos);

            //This is going to be too show. We need to only move the object.
            wxClientDC dc(this);
            m_pObjectToDrawTEMP->DrawObject(dc);

            m_pObjectToDrawTEMP->SaveDB(); //We can save it directly
            //Reset these
            m_pObjectToDrawTEMP = nullptr;
            m_MouseMoveOffset.x=0;
            m_MouseMoveOffset.y=0;


        }


     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseReleased";
     GetParent()->ProcessWindowEvent( my_event );
 }


 void ObTablePanel::mouseLeftWindow(wxMouseEvent& event)
 {
    /*
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseLeftWindow";
     GetParent()->ProcessWindowEvent( my_event );*/
 }
 void ObTablePanel::keyPressed(wxKeyEvent& event)
 {
   /*  m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="keyPressed";
     GetParent()->ProcessWindowEvent( my_event );*/
 }

 void ObTablePanel::keyReleased(wxKeyEvent& event)
 {
  /*   m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="keyReleased";
     GetParent()->ProcessWindowEvent( my_event );*/
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void ObTablePanel::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
    DrawTableLinkageLines(dc);
}

void ObTablePanel::RedrawTableObjects()
{
    wxClientDC dc(this);

    dc.Clear();
    render(dc);
    DrawTableLinkageLines(dc);
    //dc.Clear(); //NOT SURE why I put this after render, was it a mistake? I should of commented this.

    //Now we might want to put the default background colour on
}

//This is where you place stuff you need to do on refresh.
void ObTablePanel::Refresh()
{
    RedrawTableObjects();
}
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void ObTablePanel::render(wxDC&  dc)
{
    //Draw all the object to the screen.
    for (int index=0; index<m_ObTableList.GetCount();index++) {

        ObTable* pObTable = &m_ObTableList[index];
        if(pObTable!= nullptr)
            pObTable->DrawObject(dc);
    }

}

void ObTablePanel::RemoveObjectFromDiagramByTableName(wxString sTableName)
{
    //Draw all the object to the screen.
    for (int index=0; index<m_ObTableList.GetCount();index++) {


        ObTable* pObTable = &m_ObTableList[index];

        if(pObTable->GetTableName()==sTableName){
            if(pObTable!= nullptr){

                m_ObTableList.RemoveAt(index);

                break;
            }

        }

    }
    Refresh();

}
ObTable * ObTablePanel::GetObjectHitByMouse(wxPoint mousePt)
{

    //Draw all the object to the screen.
    for (int index=0; index<m_ObTableList.GetCount();index++) {

        ObTable* pObTable = &m_ObTableList[index];
        if(pObTable!= nullptr){

            if(pObTable->HitTest(mousePt))
                return pObTable;
        }

    }

    return nullptr;

}


//Adds a drawing object to the diagram panel.
void ObTablePanel::AddTableObject(const wxString& sTableID, const wxString& sTableName)
{
    ObTable *pObTable= DoesTableObjectExist(sTableName);

    if(pObTable== nullptr)
    {
        //Get the selected field list from the database
        ArrayTableField fieldItemList;
        GetSelectedFieldItemListDB(fieldItemList, sTableID,pObTable->m_aListOfLinkageFlags);

        //The table doesn't exist, so add it.
        pObTable = AddNewTableObject();
        pObTable->SetTableName(sTableName);
        pObTable->SetTableID(sTableID);
        pObTable->SetTableFieldList(fieldItemList);
        pObTable->SetObjectPosition(m_MousePos);

        m_ObTableList.Add(pObTable);


    }
    else{
        //The table exist, change it's position and redraw the screen.
        pObTable->SetObjectPosition(m_MousePos);

    }
    pObTable->SaveDB();
    RedrawTableObjects(); // Redraw all the objects.
}

//Loads all the tables from the database and creates the drawing
void ObTablePanel::LoadTableObjects(const wxString& sDatabase)
{
    // If we have existing object, clear the list.
    if(m_ObTableList.GetCount()>0)
        m_ObTableList.Clear();

    wxArrayString saTableNames;

    Utility::LoadStringArrayWithTableNamesFromSysTables(Settings.sDatabase,saTableNames);
    int count = saTableNames.GetCount();
    if(saTableNames.GetCount()>0){
        for(int index=0;index<count;index++){
            wxString sData="";
            wxString sKey="ObTableShow";
            wxString TableName = saTableNames[index];
            Utility::LoadTableData(sDatabase,TableName,sKey,sData);
            if(sData=="yes"){
                //Load the data and create the table object.
                wxString sTableID = Utility::GetTableIdFromSYS_TABLESByTableName(sDatabase,saTableNames[index]);



                //The table doesn't exist, so add it.
                ObTable * pObTable = AddNewTableObject();
                pObTable->SetTableName(saTableNames[index]);
                pObTable->SetTableID(sTableID);


                ArrayTableField fieldItemList;
                GetSelectedFieldItemListDB(fieldItemList, sTableID,pObTable->m_aListOfLinkageFlags);
                pObTable->SetTableFieldList(fieldItemList);

                wxString xPos = "";
                wxString yPos = "";
                Utility::LoadTableData(sDatabase,saTableNames[index],wxT("ObTablePositionX"),xPos);
                Utility::LoadTableData(sDatabase,saTableNames[index],wxT("ObTablePositionY"),yPos);

                int lxPos = Utility::StringToInt(xPos);
                int lyPos = Utility::StringToInt(yPos);

                if(lxPos> m_sizeWinObjectsExtend.x){
                    m_sizeWinObjectsExtend.x = lxPos;
                }
                if(lyPos> m_sizeWinObjectsExtend.y){
                    m_sizeWinObjectsExtend.y = lyPos;
                }

                wxPoint pt(Utility::StringToInt(xPos),Utility::StringToInt(yPos));
                pObTable->SetObjectPosition(pt);

                m_ObTableList.Add(pObTable);

            }
        }

        RedrawTableObjects(); // Draw them to the screen.
    }
}

ObTable* ObTablePanel::AddNewTableObject()
{
        ObTable* oTable= new ObTable;
        return oTable;
}

//Check to make sure we already don't have an existing table.
ObTable* ObTablePanel::DoesTableObjectExist(wxString sTableName)
{
    for (int index=0; index<m_ObTableList.GetCount();index++) {

        ObTable *pObTable = &m_ObTableList[index];
        if(pObTable->GetTableName()== sTableName)
            return pObTable;
    }
    return nullptr;
}


//We can send a message to the parent that this window is destroyed.
bool ObTablePanel::Destroy()
{

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    bool bResult = wxPanel::Destroy();
    return bResult;
}

void ObTablePanel::GetSelectedFieldItemListDB(ArrayTableField& fieldItemList, wxString TableID, wxArrayString &linkageFlags)
{

    wxString FormName = "";


    //Before we view, we need to make sure that all the fields in the table are created or we will get an error.



    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);


    // Connect to the sample database.
    Connection conn(false);


    if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                     (const_cast<char*>((const char*)server.mb_str())),
                     (const_cast<char*>((const char*)user.mb_str())),
                     (const_cast<char*>((const char*)pass.mb_str())))) {

        wxString QueryString="";
        QueryString << "select "
                    << SYS_FIELDS << ".valtype, "
                    << SYS_FIELDS << ".title, "
                    << SYS_FIELDS << ".flags, "
                    << SYS_FIELDS << ".valdefault, "
                    << SYS_FIELDS << ".valextra, "
                    << SYS_FIELDS << ".valnull, "
                    << SYS_FIELDS << ".valkey, "
                    << SYS_FIELDS << ".valfield, "
                    << SYS_TABLES << ".title as FormName from " << SYS_FIELDS << " INNER JOIN "
                    << SYS_TABLES << " ON " << SYS_FIELDS << ".sys_tablesId="
                    << SYS_TABLES << ".sys_tablesId WHERE " << SYS_FIELDS << ".sys_tablesId="+TableID+";";

        Query query = conn.query(QueryString);
        StoreQueryResult res = query.store();

        // Display results
        if (res) {

            //Add the field items

            // Get each row in result set, and print its contents
            for (int i = 0; i < res.num_rows(); ++i) {

                try {

                    wxString title(res[i]["title"], wxConvUTF8);
                    wxString flags(res[i]["flags"], wxConvUTF8);
                    wxString valfield(res[i]["valfield"], wxConvUTF8);
                    wxString valtype(res[i]["valtype"], wxConvUTF8);
                    wxString valDefault(res[i]["valdefault"], wxConvUTF8);
                    wxString valKey(res[i]["valkey"], wxConvUTF8);
                    wxString valNull(res[i]["valnull"], wxConvUTF8);
                    wxString valExtra(res[i]["valextra"], wxConvUTF8);

                    auto * pItem = new TableField;
                    pItem->Title = title;
                    pItem->Flags=flags;
                    pItem->fieldName=valfield;
                    pItem->fieldType=valtype;
                    pItem->fieldDefault=valDefault;
                    pItem->fieldKey=valKey;
                    pItem->fieldNull=valNull;
                    pItem->fieldExtra=valExtra;

                    fieldItemList.Add(pItem);

                    //Now we want to check to see if we have linage flags and add them to the list.
                    wxArrayString aAllFlags;


                    //NOTE The end object is aways the object with the SELECTION_XXX field flag entries.

                    if(Utility::ExtractSelectionLookupItemsID(aAllFlags,flags,true)){
                        //We need to regenerate

                        wxString flagToAdd = "SELECTION_LOOKUP_ID{";
                        for(int flagIndex=0;flagIndex<aAllFlags.GetCount();flagIndex++){
                            wxString flag = aAllFlags[flagIndex];
                            //  SELECTION_LOOKUP_ID{TableID;ColumnNumber;}
                            //Note, the selection items may be anything so don't search for them, we only need to search for SELECTION_LOOKUP_NAME SELECTION_LINKED_NAME SELECTION_LOOKUP_ID SELECTION_LINKED_ID
                            // However we add the entire selection example SELECTION_LOOKUP_NAME{TableName;FieldName;} into the return array because it's the tablename and fieldname that will be used to determine linkages.

                            flagToAdd+= flag + ";";
                        }
                        flagToAdd+= Utility::IntToString(i) + ";";
                        flagToAdd+="}";
                        linkageFlags.Add(flagToAdd);

                    }

                    aAllFlags.Clear();

                    if(Utility::ExtractSelectionLookupItemsName(aAllFlags,flags,true)){

                        wxString flagToAdd = "SELECTION_LOOKUP_NAME{";
                        for(int flagIndex=0;flagIndex<aAllFlags.GetCount();flagIndex++){
                            wxString flag = aAllFlags[flagIndex];
                            //Looking for: SELECTION_LOOKUP_NAME{TableName;FieldName;}  SELECTION_LINKED_NAME{TableName;FieldName;}    SELECTION_LOOKUP_ID{TableID;ColumnNumber;}  SELECTION_LINKED_ID{TableID;ColumnNumber;}
                            //Note, the selection items may be anything so don't search for them, we only need to search for SELECTION_LOOKUP_NAME SELECTION_LINKED_NAME SELECTION_LOOKUP_ID SELECTION_LINKED_ID
                            // However we add the entire selection example SELECTION_LOOKUP_NAME{TableName;FieldName;} into the return array because it's the tablename and fieldname that will be used to determine linkages.

                            flagToAdd+= flag + ";";

                        }
                        flagToAdd+= Utility::IntToString(i) + ";";
                        flagToAdd+="}";
                        linkageFlags.Add(flagToAdd);

                    }

                    aAllFlags.Clear();
                    if(Utility::ExtractSelectionLinkedItemsID(aAllFlags,flags,true)){

                        wxString flagToAdd = "SELECTION_LINKED_ID{";
                        for(int flagIndex=0;flagIndex<aAllFlags.GetCount();flagIndex++){
                            wxString flag = aAllFlags[flagIndex];
                            //Looking for: SELECTION_LOOKUP_NAME{TableName;FieldName;}  SELECTION_LINKED_NAME{TableName;FieldName;}    SELECTION_LOOKUP_ID{TableID;ColumnNumber;}  SELECTION_LINKED_ID{TableID;ColumnNumber;}
                            //Note, the selection items may be anything so don't search for them, we only need to search for SELECTION_LOOKUP_NAME SELECTION_LINKED_NAME SELECTION_LOOKUP_ID SELECTION_LINKED_ID
                            // However we add the entire selection example SELECTION_LOOKUP_NAME{TableName;FieldName;} into the return array because it's the tablename and fieldname that will be used to determine linkages.

                            flagToAdd+= flag + ";";

                        }
                        flagToAdd+= Utility::IntToString(i) + ";";
                        flagToAdd+="}";
                        linkageFlags.Add(flagToAdd);
                    }

                    aAllFlags.Clear();
                    if(Utility::ExtractSelectionLinkedItemsName(aAllFlags,flags,true)){
                        wxString flagToAdd = "SELECTION_LINKED_NAME{";
                        for(int flagIndex=0;flagIndex<aAllFlags.GetCount();flagIndex++){
                            wxString flag = aAllFlags[flagIndex];
                            //Looking for: SELECTION_LOOKUP_NAME{TableName;FieldName;}  SELECTION_LINKED_NAME{TableName;FieldName;}    SELECTION_LOOKUP_ID{TableID;ColumnNumber;}  SELECTION_LINKED_ID{TableID;ColumnNumber;}
                            //Note, the selection items may be anything so don't search for them, we only need to search for SELECTION_LOOKUP_NAME SELECTION_LINKED_NAME SELECTION_LOOKUP_ID SELECTION_LINKED_ID
                            // However we add the entire selection example SELECTION_LOOKUP_NAME{TableName;FieldName;} into the return array because it's the tablename and fieldname that will be used to determine linkages.

                            flagToAdd+= flag + ";";

                        }
                        flagToAdd+= Utility::IntToString(i) + ";";
                        flagToAdd+="}";
                        linkageFlags.Add(flagToAdd);

                    }
                }
                catch (int num) {

                    //f->SetStatusText("Database Connected - Row doesn't exist:");
                    wxLogMessage(MSG_DATABASE_FAIL_ROW);
                    return;

                }
            }
        }
        else {
            //cerr << "Failed to get stock table: " << query.error() << endl;
            //return 1;
            //f->SetStatusText("Database Connected - Failed to get item list:");
            wxLogMessage(MSG_DATABASE_FAIL_ITEM_LIST);
            return;
        }
    }
    else{
        wxLogMessage(MSG_DATABASE_CONNECTION_FAILURE);
        //f->SetStatusText("Did not connect to database.");
        return;
    }
}

//===================
//MENU EVENT FUNCTIONS
void ObTablePanel::OnMenuAddTable(wxCommandEvent& event)
{
    //Bring up the
    DlgTableSelect * pDlg = new DlgTableSelect(this, -1, "Import MySQL Database", m_MousePos, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);

    if(pDlg->ShowModal()==wxOK){

        wxString sTableName = pDlg->GetDataValue("ID_SELECT_TABLE");
        if(!sTableName.IsEmpty()){
            wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,sTableName);
            //wxMessageBox("The you selected was: " + sTable);
            AddTableObject(sTableId,sTableName);
        }
    }
    pDlg->Destroy();
}

// Set the ObTableShow flag to no and remove the table from the diagram.
void ObTablePanel::OnMenuRemoveTable(wxCommandEvent& event)
{
    if(m_pObCurrentTable!= nullptr){
        Utility::SaveTableData(Settings.sDatabase,m_pObCurrentTable->GetTableName(),"ObTableShow","no");
        RemoveObjectFromDiagramByTableName(m_pObCurrentTable->GetTableName());
    }
}

void ObTablePanel::OnMenuEditTableDefinitions(wxCommandEvent& event)
{

    if(m_pObCurrentTable!= nullptr){
        MyEvent my_event( this );
        my_event.m_bTableFieldDefinitions=true;
        my_event.m_sTableName=m_pObCurrentTable->GetTableName();
        my_event.m_sTableId=m_pObCurrentTable->GetTableID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}

void ObTablePanel::OnMenuEditTable(wxCommandEvent& event)
{

    if(m_pObCurrentTable!= nullptr){
        MyEvent my_event( this );
        my_event.m_bEdit=true;
        my_event.m_sTableName=m_pObCurrentTable->GetTableName();
        my_event.m_sTableId = m_pObCurrentTable->GetTableID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}

void ObTablePanel::OnMenuOpenTable(wxCommandEvent& event) {

    if(m_pObCurrentTable!= nullptr){
        MyEvent my_event( this );
        my_event.m_bOpen=true;
        my_event.m_sTableName=m_pObCurrentTable->GetTableName();
        my_event.m_sTableId=m_pObCurrentTable->GetTableID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}

void ObTablePanel::DrawTableLinkageLines(wxDC& dc)
{
    GenerateLinkageLineArrayForDrawing();

    int flatLinelength=RELATIONSHIP_DIAGRAM_FIELD_HEIGHT;

    wxFontInfo info;
    info.Italic(true);
    info.Family(wxFONTFAMILY_ROMAN );
    wxFont ftItalic(info);
    dc.SetFont(ftItalic);

    for (int index =0; index < m_ObLinkageList.GetCount(); index++){



        TableLinkageLine*  pLink = &m_ObLinkageList[index];

        if( pLink->m_pStartTableObject != nullptr && pLink->m_pEndTableObject != nullptr){





            wxRect rectStart = pLink->m_pStartTableObject->GetObjectRect();
            wxRect rectEnd = pLink->m_pEndTableObject->GetObjectRect();

            int iYStartOffset = pLink->m_iStartObYOffset;
            int iYEndOffset = pLink->m_iEndObYOffset;


            wxPoint ptObStart;
            wxPoint ptObEnd;

            if(pLink->m_bStartObLinkageSide==db_link_right){
                ptObStart.x = rectStart.x + rectStart.width;
                ptObStart.y = rectStart.y + iYStartOffset;

            }else{
                ptObStart.x = rectStart.x;
                ptObStart.y = rectStart.y+ iYStartOffset;



            }

            if(pLink->m_bEndObLinkageSide==db_link_left){
                ptObEnd.x = rectEnd.x;
                ptObEnd.y = rectEnd.y+ iYEndOffset;

            }else{

                ptObEnd.x = rectEnd.x + rectEnd.width;
                ptObEnd.y = rectEnd.y + iYEndOffset;

            }

            if(pLink->m_bStartObLinkageSide==db_link_right){

                wxPoint pLeft(ptObStart);
                pLeft.x += flatLinelength;

                wxPoint pRight(ptObEnd);
                pRight.x -= flatLinelength;

                dc.SetPen( wxPen( wxColor(0,0,0), 4 ) );
                dc.DrawLine(ptObStart,pLeft);
                dc.SetPen( wxPen( wxColor(0,0,0), 2 ) );
                dc.DrawLine(pLeft,pRight);
                dc.SetPen( wxPen( wxColor(0,0,0), 4 ) );
                dc.DrawLine(pRight,ptObEnd);

                //Draw the relashipship
                dc.SetPen( wxPen( wxColor(0,0,0), 1 ) );
                dc.DrawText("1",ptObStart.x+ 3,ptObStart.y -15);

                wxBrush brush2(dc.GetBackground());
                dc.SetBrush(brush2); // green filling

                dc.DrawEllipse(wxRect(ptObEnd.x-13,ptObEnd.y-9,6,4));
                dc.DrawEllipse(wxRect(ptObEnd.x-7,ptObEnd.y-9,6,4));

            }else{

                wxPoint pLeft(ptObEnd);
                pLeft.x += flatLinelength;

                wxPoint pRight(ptObStart);
                pRight.x -= flatLinelength;

                dc.SetPen( wxPen( wxColor(0,0,0), 4 ) );
                dc.DrawLine(ptObEnd,pLeft);
                dc.SetPen( wxPen( wxColor(0,0,0), 2 ) );
                dc.DrawLine(pLeft,pRight);
                dc.SetPen( wxPen( wxColor(0,0,0), 4 ) );
                dc.DrawLine(pRight,ptObStart);

                //Draw the relashipship
                dc.SetPen( wxPen( wxColor(0,0,0), 1 ) );
                dc.DrawText("1",ptObStart.x- 10,ptObStart.y -15);

                wxBrush brush2(dc.GetBackground());
                dc.SetBrush(brush2); // green filling

                dc.DrawEllipse(wxRect(ptObEnd.x+7,ptObEnd.y-9,6,4));
                dc.DrawEllipse(wxRect(ptObEnd.x+1,ptObEnd.y-9,6,4));

            }




        }



    }


}

void ObTablePanel::GenerateLinkageLineArrayForDrawing()
{

    //Make sure we clear the list
    m_ObLinkageList.Clear();


    //Run through all the table objects looking for linkage flags
    for (int index=0; index<m_ObTableList.GetCount();index++) {

        ObTable* pObTable = &m_ObTableList[index]; //Get the object

        //It shouldn't be null, but check anyway.
        if(pObTable!= nullptr){

            if(pObTable->m_aListOfLinkageFlags.GetCount()>0) {


                //Note, each object might have more than one linkage, some might have several.
                //Run through the linkage flag list looking for linkages.
                for (int flagsIndex = 0; flagsIndex < pObTable->m_aListOfLinkageFlags.GetCount(); flagsIndex++) {
                    //We found a linkage, extract the data depending on the type of linkage.
                    wxString flags = pObTable->m_aListOfLinkageFlags[flagsIndex]; // Get the flags for the linkage, this will be in the form SELECTION_LOOKUP_NAME{TableName;FieldName;}

                    //Create a new linkage object to add to our list.
                    TableLinkageLine *pObLink = new TableLinkageLine();

                    //Step 2 We found a linkage, we need to extract the information and create the TableLinkageLine object
                    //At this point, flags will be ONE of the following,  note however, as we run through this for loop, a table might have more than one linkage.
                    // SELECTION_LOOKUP_NAME{TableName;FieldName;}
                    // SELECTION_LINKED_NAME{TableName;FieldName;}
                    // SELECTION_LOOKUP_ID{TableID;ColumnNumber;}
                    // SELECTION_LINKED_ID{TableID;ColumnNumber;}
                    wxArrayString sItemsList; // Get the items items from the list.

                    //NOTE: the end object is going to this pObTable and the start object will be the one in the linkage

                    //This will be the same regarless of the type of linkage
                    pObLink->m_pEndTableObject=pObTable;
                    pObLink->m_sEndObjectTableName=pObTable->GetTableName();
                    pObLink->m_sEndObjectTableId=pObTable->GetTableID();



                    if (Utility::HasFlag(flags, "SELECTION_LOOKUP_NAME")) {
                        Utility::ExtractSelectionLookupItemsName(sItemsList,flags,true);
                        pObLink->m_LinkageType=db_link_lookup_name;

                        //Make sure there are only two items in this list, we don't want the application to crash if the user puts in only one item.
                        if(sItemsList.GetCount()==3){
                            pObLink->m_sStartObjectTableName=sItemsList[0]; //start object TableName
                            pObLink->m_sStartObLinkedFieldName=sItemsList[1]; //start object FieldName
                            pObLink->m_sEndObLinkedFieldColNumber=sItemsList[2]; //end field column number


                        }
                        else{
                            //WRITE TO WARNING LOG
                            //NOTE This would be a great place to setup a warning system for the users, simply write to a log telling the user what is wrong
                            //This will help in the development of database applications.
                        }

                        pObLink->m_pStartTableObject=FindTableObjectByTableName(pObLink->m_sStartObjectTableName);

                    }
                    else if (Utility::HasFlag(flags, "SELECTION_LINKED_NAME")) {
                        Utility::ExtractSelectionLinkedItemsName(sItemsList,flags,true);
                        pObLink->m_LinkageType=db_link_linked_name;

                        if(sItemsList.GetCount()==3){
                            pObLink->m_sStartObjectTableName=sItemsList[0]; //TableName
                            pObLink->m_sStartObLinkedFieldName=sItemsList[1]; //FieldName
                            pObLink->m_sEndObLinkedFieldColNumber=sItemsList[2]; //end field column number
                        }
                        else{
                            //WRITE TO WARNING LOG
                            //NOTE This would be a great place to setup a warning system for the users, simply write to a log telling the user what is wrong
                            //This will help in the development of database applications.
                        }

                        pObLink->m_pStartTableObject=FindTableObjectByTableName(pObLink->m_sStartObjectTableName);

                    }
                    else if (Utility::HasFlag(flags, "SELECTION_LOOKUP_ID")) {
                        Utility::ExtractSelectionLookupItemsID(sItemsList,flags,true);
                        pObLink->m_LinkageType=db_link_lookup_id;

                        if(sItemsList.GetCount()==3){
                            pObLink->m_sStartObjectTableId=sItemsList[0]; //Table ID
                            pObLink->m_sStartObLinkedFieldColNumber=sItemsList[1]; // Column number
                            pObLink->m_sEndObLinkedFieldColNumber=sItemsList[2]; //end field column number
                        }
                        else{
                            //WRITE TO WARNING LOG
                            //NOTE This would be a great place to setup a warning system for the users, simply write to a log telling the user what is wrong
                            //This will help in the development of database applications.
                        }

                        pObLink->m_pStartTableObject=FindTableObjectByTableID(pObLink->m_sStartObjectTableId);

                    }
                    else if (Utility::HasFlag(flags, "SELECTION_LINKED_ID")) {
                        Utility::ExtractSelectionLinkedItemsID(sItemsList,flags,true);
                        pObLink->m_LinkageType=db_link_linked_id;

                        if(sItemsList.GetCount()==3){
                            pObLink->m_sStartObjectTableId=sItemsList[0]; //Table ID
                            pObLink->m_sStartObLinkedFieldColNumber=sItemsList[1]; // Col num
                            pObLink->m_sEndObLinkedFieldColNumber=sItemsList[2]; //end field column number
                        }
                        else{
                            //WRITE TO WARNING LOG
                            //NOTE This would be a great place to setup a warning system for the users, simply write to a log telling the user what is wrong
                            //This will help in the development of database applications.
                        }
                        pObLink->m_pStartTableObject=FindTableObjectByTableID(pObLink->m_sStartObjectTableId);

                    }
                    //Do the same regardless the type
                    DetermineOTableObjectLinkageSide(pObLink);
                    DetermineOTableObjectLinkageOffsets(pObLink); //This is dependant on the type, a lookup will link to the field and a linked to the primary key.

                    //We need to extract the flag table name or table ID and fields
                    m_ObLinkageList.Add(pObLink);



                }
            }
        }
    }
}
//Searches this drawing for the table object pointer.
ObTable * ObTablePanel::FindTableObjectByTableName(wxString sTableName) {
    //Run through all the table objects looking for linkage flags
    for (int index = 0; index < m_ObTableList.GetCount(); index++) {

        ObTable *pObTable = &m_ObTableList[index]; //Get the object
        //It shouldn't be null, but check anyway.
        if (pObTable != nullptr) {
            if(pObTable->GetTableName()==sTableName)
                return pObTable;
        }
    }
    return nullptr;
}
//Searches this drawing for the table object pointer.
ObTable * ObTablePanel::FindTableObjectByTableID(wxString sTableId)
{
    for (int index=0; index<m_ObTableList.GetCount();index++) {

        ObTable* pObTable = &m_ObTableList[index]; //Get the object
        //It shouldn't be null, but check anyway.
        if(pObTable!= nullptr) {
            if(pObTable->GetTableID()==sTableId)
                return pObTable;
        }
    }
    return nullptr;
}

//Determine which side to link up on the position of both the start and end objects
void ObTablePanel::DetermineOTableObjectLinkageSide(TableLinkageLine *pObLink)
{

    ObTable * pStart = pObLink->m_pStartTableObject;
    ObTable *pEnd = pObLink->m_pEndTableObject;

    if(pStart != nullptr && pEnd != nullptr){


        wxRect rectEndOb = pEnd->GetObjectRect();
        wxRect rectStartOb = pStart->GetObjectRect();

        wxPoint pEnd(rectEndOb.x,0);
        wxPoint pStart(rectStartOb.x,0);

        //Start object is to the left of the end object
        if(pEnd.x > pStart.x){

            pObLink->m_bStartObLinkageSide = db_link_right;
            pObLink->m_bEndObLinkageSide= db_link_left;

        } else{
            pObLink->m_bStartObLinkageSide = db_link_left;
            pObLink->m_bEndObLinkageSide= db_link_right;
        }

    }

}

//Determine where to start drawing the linkage, find the offset relative to the top position of the table object.
void ObTablePanel::DetermineOTableObjectLinkageOffsets(TableLinkageLine *pObLink)
{
    int heightOfField = RELATIONSHIP_DIAGRAM_FIELD_HEIGHT + RELATIONSHIP_DIAGRAM_GAP;
    int TitleHeight = RELATIONSHIP_DIAGRAM_TITLE_HEIGHT + RELATIONSHIP_DIAGRAM_FIELD_HEIGHT + RELATIONSHIP_DIAGRAM_GAP + RELATIONSHIP_DIAGRAM_GAP; // you also need to account for the primary key column
    int StartFieldIndex = 0;

    //Determine the start object
    if(pObLink->m_LinkageType==db_link_lookup_id || pObLink->m_LinkageType==db_link_linked_id){
        // This will be one of these
        // SELECTION_LOOKUP_ID{TableID;ColumnNumber;}
        // SELECTION_LINKED_ID{TableID;ColumnNumber;}

        //We can determine the offset by the columnNumber
        //It is a simple matter of multiplying it by some factor
        // The field column number was supplied by the user in the SELECTION_LOOKUP_ID OR SELECTION_LINKED_ID

        StartFieldIndex = Utility::StringToInt(pObLink->m_sStartObLinkedFieldColNumber);

    } else{
        // This will be one of these
        // SELECTION_LOOKUP_NAME{TableName;FieldName;}
        // SELECTION_LINKED_NAME{TableName;FieldName;}

        //We don't have the column numbers available so we have to look it up from the object
        // The field column number needs to be found from the object because it wasn't supplied by the user, only the field name was supplied.
        // We can get the index directly from the objects field list.
        ObTable *pObject = pObLink->m_pStartTableObject;
        if(pObject != nullptr)
            StartFieldIndex = pObject->GetFieldIndexByFieldName(pObLink->m_sStartObLinkedFieldName);

    }


    //Set the offset
    pObLink->m_iStartObYOffset= (StartFieldIndex + 1) * (heightOfField ) + TitleHeight + RELATIONSHIP_DIAGRAM_GAP + RELATIONSHIP_DIAGRAM_GAP+3;
    //The ends objects offset is independant of the linkage beacause the index was found while loading the objects fields from the Database.
    pObLink->m_iEndObYOffset= (Utility::StringToInt(pObLink->m_sEndObLinkedFieldColNumber) + 1 ) * (heightOfField) + TitleHeight+ RELATIONSHIP_DIAGRAM_GAP+ RELATIONSHIP_DIAGRAM_GAP+3;
}