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


#include "global.h"
#include "MyEvent.h"

#include "Utility.h"
#include "ObTable.h"
#include "mysql.h"
#include "mysql++.h"


#include "ObTablePanel.h"
#include <cmath>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayDrawObjects);

using namespace mysqlpp;



BEGIN_EVENT_TABLE(ObTablePanel, wxPanel)
// some useful events

 EVT_MOTION(ObTablePanel::mouseMoved)
 EVT_LEFT_DOWN(ObTablePanel::mouseDown)
 EVT_LEFT_UP(ObTablePanel::mouseReleased)
 EVT_RIGHT_DOWN(ObTablePanel::rightClick)
 EVT_LEAVE_WINDOW(ObTablePanel::mouseLeftWindow)
 EVT_KEY_DOWN(ObTablePanel::keyPressed)
 EVT_KEY_UP(ObTablePanel::keyReleased)
 EVT_MOUSEWHEEL(ObTablePanel::mouseWheelMoved)


// catch paint events
EVT_PAINT(ObTablePanel::paintEvent)

END_EVENT_TABLE()

ObTablePanel::ObTablePanel(wxFrame* parent) : wxPanel(parent)
{
    m_pObjectToDraw = nullptr;
    m_MouseMoveOffset.x=0;
    m_MouseMoveOffset.y=0;
}

ObTablePanel::~ObTablePanel()
{

}
// some useful events

 void ObTablePanel::mouseMoved(wxMouseEvent& event)
 {
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw


    if (m_pObjectToDraw != nullptr){

        wxPoint pos;

        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;
        m_pObjectToDraw->SetObjectPosition(pos);

            //This is going to be too show. We need to only move the object.
            wxClientDC dc(this);
        m_pObjectToDraw->DrawObject(dc);

    }

 }

// Testing draw the position when the mouse is down.
 void ObTablePanel::mouseDown(wxMouseEvent& event) {

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw


    ObTable* pObTable = GetObjectHitByMouse(event.GetPosition());
    if(pObTable!= nullptr)
    {
        m_MouseMoveOffset=Utility::CalcPtInRectOffset(m_MousePos,pObTable->GetObjectRect());
        m_pObjectToDraw = pObTable;
        m_pObjectToDraw->TurnSnapOff();

    }


    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_sData="mouseDown";
    GetParent()->ProcessWindowEvent( my_event );

}

void ObTablePanel::rightClick(wxMouseEvent& event)
{

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    //Tell the parent to add a table to the drawing at the mouse point.
    MyEvent my_event( this );
    my_event.m_bAddTableObject = true;
    my_event.m_pMousePoint=m_MousePos;
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

        if(m_pObjectToDraw!=nullptr){

            //Restore the snap condition to what is was before we moved the object
            // We don't want snap on because it results in clunky movement. v
            m_pObjectToDraw->RestorePreviousSnapCondition();

            wxPoint pos;

            pos.x = m_MousePos.x - m_MouseMoveOffset.x;
            pos.y = m_MousePos.y - m_MouseMoveOffset.y;
            m_pObjectToDraw->SetObjectPosition(pos);

            //This is going to be too show. We need to only move the object.
            wxClientDC dc(this);
            m_pObjectToDraw->DrawObject(dc);

            m_pObjectToDraw->SaveDB(); //We can save it directly
            //Reset these
            m_pObjectToDraw = nullptr;
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
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseLeftWindow";
     GetParent()->ProcessWindowEvent( my_event );
 }
 void ObTablePanel::keyPressed(wxKeyEvent& event)
 {
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="keyPressed";
     GetParent()->ProcessWindowEvent( my_event );
 }

 void ObTablePanel::keyReleased(wxKeyEvent& event)
 {
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="keyReleased";
     GetParent()->ProcessWindowEvent( my_event );
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
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
void ObTablePanel::RedrawTableObjects()
{
    wxClientDC dc(this);
    render(dc);
    dc.Clear();

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
void ObTablePanel::AddDrawObject(const wxString& sTableID, const wxString& sTableName)
{
    ObTable *pObTable= DoesTableObjectExist(sTableName);

    if(pObTable== nullptr)
    {
        //Get the selected field list from the database
        ArrayTableField fieldItemList;
        GetSelectedFieldItemListDB(fieldItemList, sTableID);

        //The table doesn't exist, so add it.
        pObTable = AddNewTableObject();
        pObTable->SetTableName(sTableName);
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
                wxString sTableID = Utility::GetTableIdFromSYS_TABLES(sDatabase,saTableNames[index]);

                ArrayTableField fieldItemList;
                GetSelectedFieldItemListDB(fieldItemList, sTableID);

                //The table doesn't exist, so add it.
                ObTable * pObTable = AddNewTableObject();
                pObTable->SetTableName(saTableNames[index]);
                pObTable->SetTableFieldList(fieldItemList);

                wxString xPos = "";
                wxString yPos = "";
                Utility::LoadTableData(sDatabase,saTableNames[index],wxT("ObTablePositionX"),xPos);
                Utility::LoadTableData(sDatabase,saTableNames[index],wxT("ObTablePositionY"),yPos);

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

void ObTablePanel::GetSelectedFieldItemListDB(ArrayTableField& fieldItemList, wxString TableID)
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
            for (size_t i = 0; i < res.num_rows(); ++i) {

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


