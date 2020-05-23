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

#include "ObForm.h"
#include "../Shared/global.h"
#include "../Shared/Utility.h"
#include "../MyEvent.h"
#include "../Dialog/DlgTableSelect.h"
#include "../Dialog/LinkedTableDlg.h"
#include "../Dialog/MultiTextDlg.h"
#include "../Dialog/SelectionFieldQueryDlg.h"
#include "../Dialog/SetFlagsDlg.h"
#include "../Dialog/ListBoxDlg.h"
#include "../Dialog/SingleTextDlg.h"
#include "../Dialog/ManageActionsDlg.h"
#include "../Generic/GenericQueryGrid.h"

#include "mysql.h"
#include "mysql++.h"

#include <cmath>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayDrawForms);

#include "DesignPagePanel.h"

using namespace mysqlpp;

enum {
    ID_MENU_ADD_CONTROL = wxID_HIGHEST + 700,
    ID_MENU_EDIT_LINKED_FIELD,
    ID_MENU_DELETE_CONTROL,
    ID_BUTTON_ADD_VALUE,
    ID_BUTTON_REMOVE_LAST,
    ID_CHK_AS,
    ID_CHK_EQUAL,
    ID_CHK_LIKE,
    ID_MENU_EDIT_STATIC,
    ID_MENU_RUN_QUERY,
    ID_MENU_EDIT_SET_FLAGS,
    ID_MENU_SELECTION_OPTIONS,
    ID_MENU_EDIT_LINKED_TABLE,
    ID_MENU_EDIT_DESCRIPTION,
    ID_MENU_MANAGE_ACTIONS
};

BEGIN_EVENT_TABLE(DesignPagePanel, wxPanel)
// some useful events

    EVT_MOTION(DesignPagePanel::mouseMoved)
    EVT_LEFT_DOWN(DesignPagePanel::mouseDown)
    EVT_LEFT_DCLICK(DesignPagePanel::mouseDoubleClick)
    EVT_LEFT_UP(DesignPagePanel::mouseReleased)
    EVT_RIGHT_DOWN(DesignPagePanel::rightClick)
    EVT_LEAVE_WINDOW(DesignPagePanel::mouseLeftWindow)
    EVT_KEY_DOWN(DesignPagePanel::keyPressed)
    EVT_KEY_UP(DesignPagePanel::keyReleased)
    EVT_MOUSEWHEEL(DesignPagePanel::mouseWheelMoved)
    EVT_MENU(ID_MENU_EDIT_LINKED_FIELD, DesignPagePanel::OnMenuEditLinkedFields)
    EVT_MENU(ID_MENU_DELETE_CONTROL, DesignPagePanel::OnMenuDeleteControl)
    EVT_MENU(ID_MENU_EDIT_STATIC, DesignPagePanel::OnMenuEditStatic)
    EVT_MENU(ID_MENU_EDIT_DESCRIPTION, DesignPagePanel::OnMenuEditDescription)
    EVT_MENU(ID_MENU_RUN_QUERY, DesignPagePanel::OnRunQuery)
    EVT_MENU(ID_MENU_EDIT_SET_FLAGS, DesignPagePanel::OnSetFlags)
    EVT_MENU(ID_MENU_SELECTION_OPTIONS, DesignPagePanel::OnSetSelectionOptions)
    EVT_MENU(ID_MENU_EDIT_LINKED_TABLE, DesignPagePanel::OnSetLinkedTable)
    EVT_MENU(ID_MENU_MANAGE_ACTIONS, DesignPagePanel::OnManageActions)
    EVT_PAINT(DesignPagePanel::paintEvent)

END_EVENT_TABLE()
void DesignPagePanel::rightClick(wxMouseEvent& event)
{
    m_pObCurrentFormControl= nullptr;

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObForm* pCtl = GetObjectHitByMouse(m_MousePos);
    //Diagram menus
    //Create a context menu to do stuff here.
    auto *menu = new wxMenu;

    if(pCtl!= nullptr)
    {
        m_pObCurrentFormControl = pCtl;
/*
        if(m_pObCurrentFormControl->GetTypeName()=="CTL_STATIC" )
        {
            menu->Append(ID_MENU_EDIT_STATIC, wxT("Set Label"), wxT("Set the Label."));
        }
        else if(m_pObCurrentFormControl->GetTypeName()=="CTL_SPACER" ){

        }
        else if(m_pObCurrentFormControl->GetTypeName()=="CTL_SELECTION" || m_pObCurrentFormControl->GetTypeName()=="CTL_SELECTION_ADDITIVE"){
            menu->Append(ID_MENU_EDIT_STATIC, wxT("Set Label"), wxT("Set the Label."));
            menu->Append(ID_MENU_EDIT_LINKED_FIELD, wxT("Set Linked Field"), wxT("Edit table definitions."));
            menu->Append(ID_MENU_SELECTION_OPTIONS, wxT("Manage Options"), wxT("Manage options."));
        }
        else if(m_pObCurrentFormControl->GetTypeName()=="CTL_SELECTION_LINKED_NAME"
                || m_pObCurrentFormControl->GetTypeName()=="CTL_SELECTION_LOOKUP_NAME"){
            menu->Append(ID_MENU_EDIT_STATIC, wxT("Set Label"), wxT("Set the Label."));
            menu->Append(ID_MENU_EDIT_LINKED_FIELD, wxT("Set Linked Field"), wxT("Edit table definitions."));
            menu->Append(ID_MENU_EDIT_LINKED_TABLE, wxT("Set Linked Table"), wxT("Set Linked Table."));
        }
        else if(m_pObCurrentFormControl->GetTypeName()=="CTL_BUTTON")
        {
            menu->Append(ID_MENU_EDIT_STATIC, wxT("Set Button Label"), wxT("Set the Button Label."));
            menu->Append(ID_MENU_EDIT_DESCRIPTION, wxT("Set Button Description"), wxT("Set Button Description."));
            menu->Append(ID_MENU_MANAGE_ACTIONS, wxT("Manage Actions"), wxT("Manage Actions"));

        }
        else if(m_pObCurrentFormControl->GetTypeName()!="CTL_RECORD_SELECTOR")
        {
            menu->Append(ID_MENU_EDIT_STATIC, wxT("Set Label"), wxT("Set the Label."));
            menu->Append(ID_MENU_EDIT_LINKED_FIELD, wxT("Set Linked Field"), wxT("Edit table definitions."));

            //Don't do this.
            //menu->Append(ID_MENU_EDIT_SET_FLAGS, wxT("Set Flags"), wxT("Set flags for this controlFS."));
        }
*/

        menu->Append(ID_MENU_DELETE_CONTROL, wxT("Delete Control"), wxT("Delete Control."));
    }
    else{
        menu->Append(ID_MENU_RUN_QUERY, wxT("Test query"), wxT("Test query."));
        // menu->AppendSeparator();
    }

    PopupMenu( menu, m_MousePos);

    //Tell the parent to add a table to the drawing at the mouse point.
    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_pMousePoint=m_MousePos;
    GetParent()->ProcessWindowEvent( my_event );

}

DesignPagePanel::~DesignPagePanel()
{
    DeleteDrawObject();
}

DesignPagePanel::DesignPagePanel(wxFrame* parent) : wxPanel(parent)
{
    m_pObjectToDrawTEMP = nullptr;
    m_pObCurrentFormControl = nullptr;
    m_MouseMoveOffset.x=0;
    m_MouseMoveOffset.y=0;
    m_sizeWinObjectsExtend.x=0;
    m_sizeWinObjectsExtend.y=0;
}

//===================
//MENU EVENT FUNCTIONS
void DesignPagePanel::OnMenuEditStatic(wxCommandEvent& event)
{
    wxString sData = PromptEditSingleTextDialog("Label","Set Label","Label");
   // m_pObCurrentFormControl->SetLabel(sData);
    RedrawFormObjects();
}

void DesignPagePanel::OnMenuEditDescription(wxCommandEvent& event)
{
    wxString sData = PromptEditSingleTextDialog("Short_Description","Edit short description","Short Description.");
    //m_pObCurrentFormControl->SetDescription(sData);
    RedrawFormObjects();
}

wxString DesignPagePanel::PromptEditSingleTextDialog(wxString sKey, wxString sDialogTitle, wxString sLabel)
{
    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlId = "";// m_pObCurrentFormControl->GetControlID();

        SingleTextDlg * pDlg = new SingleTextDlg(nullptr, sDialogTitle, sLabel);

        Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,sKey,sLabel);

        pDlg->SetDataValue("ID_TEXT",sLabel);

        if(pDlg->ShowModal()==wxOK) {

            sLabel = pDlg->GetDataValue("ID_TEXT");
            Utility::SaveTableData(Settings.sDatabase,"usr_controls",sControlId,sKey,sLabel);
            //We need to create a new entry in the Form queries.
        }
        pDlg->Destroy();
    }

    return sLabel;
}

void DesignPagePanel::OnManageActions(wxCommandEvent& event)
{

    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlId = ""; //m_pObCurrentFormControl->GetControlID();
        ManageActionsDlg * pDlg = new ManageActionsDlg(nullptr, "Avaliable actions", "Select Action");
        pDlg->SetControlID(sControlId);
        pDlg->Load();

        if(pDlg->ShowModal()==wxOK) {

            pDlg->Save();
            //So we can redraw on the screen.
       //     m_pObCurrentFormControl->SetAction(pDlg->GetAction());
            RedrawFormObjects();

            //We need to create a new entry in the Form queries.
        }
        pDlg->Destroy();

    }
}

void DesignPagePanel::OnMenuEditLinkedFields(wxCommandEvent& event)
{
    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlId = "";//m_pObCurrentFormControl->GetControlID();
        SelectionFieldQueryDlg * pDlg = new SelectionFieldQueryDlg(nullptr, "Avaliable Fields", "Select Field");
        pDlg->SetControlID(sControlId);
        pDlg->SetQuery(m_sBuildQuery);
        pDlg->Load();

        if(pDlg->ShowModal()==wxOK) {

            pDlg->Save();
            //So we can redraw on the screen.
   //         m_pObCurrentFormControl->SetField(pDlg->GetField());
            RedrawFormObjects();

            //We need to create a new entry in the Form queries.
        }
        pDlg->Destroy();

    }
}


void DesignPagePanel::OnSetFlags(wxCommandEvent& event)
{
    /*
    SetFlagsDlg * pDlg = new SetFlagsDlg(nullptr, "Avaliable Fields", "Select Field");
    wxString sControlId = m_pObCurrentFormControl->GetControlID();
    wxString flags="";
    wxString sKey="FLAGS";
    Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,sKey,flags);
    pDlg->SetDataValue("ID_FLAGS",flags);
    if(pDlg->ShowModal()==wxOK) {
        wxString flags = pDlg->GetDataValue("ID_FLAGS");
        Utility::SaveTableData(Settings.sDatabase,"usr_controls",sControlId,"FLAGS",flags);
    }
    pDlg->Destroy();
     */
}

void DesignPagePanel::OnSetSelectionOptions(wxCommandEvent& event)
{
    ListBoxDlg * pDlg = new ListBoxDlg(nullptr, "Selection Options List", "Selection Options");
    wxString sControlId = ""; //m_pObCurrentFormControl->GetControlID();
    pDlg->SetControlID(sControlId);
    pDlg->LoadItems();

    if(pDlg->ShowModal()==wxOK)
        pDlg->SaveItems();

    pDlg->Destroy();

}

void DesignPagePanel::OnSetLinkedTable(wxCommandEvent& event)
{
    /*
    LinkedTableDlg * pDlg = new LinkedTableDlg(nullptr, "Selection Options List", "Selection Options");
    wxString sControlId = m_pObCurrentFormControl->GetControlID();
    pDlg->SetControlID(sControlId);
    pDlg->Load();

    if(pDlg->ShowModal()==wxOK)
        pDlg->Save();

    pDlg->Destroy();*/
}

void DesignPagePanel::OnMenuDeleteControl(wxCommandEvent& event)
{
    /*
    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlTypeName = m_pObCurrentFormControl->GetTypeName();
        wxString sControlId = m_pObCurrentFormControl->GetControlID();

        //The field doesn't exist, do you want to add it.
        auto *dlg = new wxMessageDialog(nullptr, "Are you sure you want to delete this control? \n\n", wxT("Delete Control"), wxYES_NO | wxICON_EXCLAMATION);

        if ( dlg->ShowModal() == wxID_YES ){
            wxString dropQuery= "DELETE FROM usr_controls WHERE usr_controlsId= "+m_pObCurrentFormControl->GetControlID();

            Utility::ExecuteQuery(dropQuery);
            RemoveControlFromList(m_pObCurrentFormControl->GetControlID());
            RedrawControlObjects();
            dlg->Destroy();

        }
    }*/
}

void DesignPagePanel::OnRunQuery(wxCommandEvent& event) {

    MyEvent my_event( this );
    my_event.m_bOpenRunQuery=true;
    GetParent()->ProcessWindowEvent( my_event );
}

void DesignPagePanel::OnMenuOpenControl(wxCommandEvent& event) {
/*
    if(m_pObCurrentFormControl!= nullptr){
        MyEvent my_event( this );
        my_event.m_bOpen=true;
        my_event.m_sTableName=m_pObCurrentFormControl->GetTypeName();
        my_event.m_sTableId=m_pObCurrentFormControl->GetControlID();
        GetParent()->ProcessWindowEvent( my_event );
    }*/
}

// Testing draw the position when the mouse is down.
void DesignPagePanel::mouseDown(wxMouseEvent& event) {

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObForm* pCtl = GetObjectHitByMouse(m_MousePos);
    if(pCtl!= nullptr)
    {
        m_MouseMoveOffset=Utility::CalcPtInRectOffset(m_MousePos,pCtl->GetFormRect());

        //NOTE we never want to delete m_pObjectToDrawTEMP because this is the object on the screen, but we want to make the pointer null when we are finished with it.
        m_pObjectToDrawTEMP = pCtl;
        m_pObjectToDrawTEMP->TurnSnapOff();
    }

    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_sData="mouseDown";
    GetParent()->ProcessWindowEvent( my_event );
}

void DesignPagePanel::mouseMoved(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    if (m_pObjectToDrawTEMP != nullptr){

        wxPoint pos;
        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;

        //Drag the object
        m_pObjectToDrawTEMP->SetFormPosition(pos);

        //This is going to be too show. We need to only move the object.
        wxClientDC dc(this);
        m_pObjectToDrawTEMP->DrawFormObject(dc);

    }
}

void DesignPagePanel::mouseReleased(wxMouseEvent& event)
{

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    if(m_pObjectToDrawTEMP!=nullptr){

        //Restore the snap condition to what is was before we moved the object
        m_pObjectToDrawTEMP->RestorePreviousSnapCondition();
        wxPoint pos;
        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;

        m_pObjectToDrawTEMP->SetFormPosition(pos); //This also checks the limits and doesn't draw the object in the query builder section.

        wxClientDC dc(this);
        m_pObjectToDrawTEMP->DrawFormObject(dc);

        m_pObjectToDrawTEMP->SaveDB(m_pObjectToDrawTEMP->GetFormID()); //We can save it directly
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

void DesignPagePanel::DeleteDrawObject()
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

wxSize DesignPagePanel::GetSizeDiagramExtend()
{
    wxSize size = m_sizeWinObjectsExtend;
    size.x = m_sizeWinObjectsExtend.x + 300;
    size.y = m_sizeWinObjectsExtend.y + 300;
    return size;
}



void DesignPagePanel::mouseDoubleClick(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObForm* pCtl = GetObjectHitByMouse(m_MousePos);
    if(pCtl!= nullptr)
    {
        MyEvent my_event( this );
        my_event.m_bOpen=true;
   //     my_event.m_sTableName=pCtl->GetTypeName();
    //    my_event.m_sTableId=pCtl->GetControlID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}

 void DesignPagePanel::mouseWheelMoved(wxMouseEvent& event)
 {
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseWheelMoved";
     GetParent()->ProcessWindowEvent( my_event );
 }

 void DesignPagePanel::mouseLeftWindow(wxMouseEvent& event)
 {
    /*
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseLeftWindow";
     GetParent()->ProcessWindowEvent( my_event );*/
 }
 void DesignPagePanel::keyPressed(wxKeyEvent& event)
 {
   /*  m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="keyPressed";
     GetParent()->ProcessWindowEvent( my_event );*/
 }

 void DesignPagePanel::keyReleased(wxKeyEvent& event)
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

void DesignPagePanel::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void DesignPagePanel::RedrawFormObjects()
{
    wxClientDC dc(this);

    dc.Clear();
    render(dc);

    //dc.Clear(); //NOT SURE why I put this after render, was it a mistake? I should of commented this.

    //Now we might want to put the default background colour on
}

//This is where you place stuff you need to do on refresh.
void DesignPagePanel::Refresh()
{
    RedrawFormObjects();
}
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void DesignPagePanel::render(wxDC&  dc)
{

    //Draw all the object to the screen.
    for (int index=0; index<m_FormList.GetCount();index++) {

        ObForm* pCtl = &m_FormList[index];
        if(pCtl!= nullptr)
            pCtl->DrawFormObject(dc);
    }
}

ObForm * DesignPagePanel::GetObjectHitByMouse(wxPoint mousePt)
{


    //Draw all the object to the screen.
    for (int index=0; index<m_FormList.GetCount();index++) {

        ObForm* pCtl = &m_FormList[index];
        if(pCtl!= nullptr){

            if(pCtl->HitTest(mousePt)){

                return pCtl;
            }

        }
    }
    return nullptr;
}

//The the mouse point hit a field, will return the field name
ObForm * DesignPagePanel::GetObjectHitByMouse(wxPoint mousePt, wxString& sHitFieldName, wxRect& sfieldRect)
{
    ObForm * pCtl = GetObjectHitByMouse(mousePt);
    //if(pCtl != nullptr)
        //sHitFieldName = pCtl->HitTestField(mousePt,sfieldRect);
    return pCtl;
}

//Adds a drawing object to the diagram panel.
void DesignPagePanel::AddFormObject(const wxString& sFormID)
{
   /*
    ObForm *pCtl = NewCtlObject();
    if(pCtl != nullptr)
    {
        //The Control doesn't exist, so add it.
        pCtl->SetControlPosition(m_MousePos);
        pCtl->SetFormID(sFormID);
        wxArrayString sArray;
        Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_forms", "typeName", "usr_formsId", sFormID);
        if(sArray.GetCount()==1){
            pCtl->SetTypeName(sArray[0]);
        }
        m_ControlList.Add(pCtl);

        pCtl->SaveDB("");
    }
    RedrawControlObjects(); // Redraw all the objects.
     */
}
void DesignPagePanel::RemoveControlFromList(wxString sControlId)
{
    /*
    int count = m_ControlList.GetCount();
    if(m_ControlList.GetCount()>0) {
        for (int index = 0; index < count; index++) {
            if(m_ControlList[index].GetControlID()==sControlId){
                //Remove this from the list, redraw and exit
                m_ControlList.RemoveAt(index);
                break;
            }
        }
    }

*/
}

void DesignPagePanel::SetPageID(wxString sPageId){
    m_sPageId = sPageId;
}

wxString DesignPagePanel::GetPageID()
{
    return m_sPageId;
}

//Loads all the controls from the database and creates the drawing
void DesignPagePanel::LoadFormObjects()
{

    // If we have existing object, clear the list.
    if(m_FormList.GetCount()>0)
        m_FormList.Clear();

    wxString QueryString = "SELECT * ";
    QueryString += " FROM usr_forms ";
    QueryString += " WHERE usr_pagesId = " + m_sPageId;

    ArrayFieldRecord saControl;
    Utility::LoadArrayFieldRecordFromQuery(saControl, QueryString);

    int count = saControl.GetCount();
    if(saControl.GetCount()>0){
        for(int index=0;index<count;index++){


            wxString usr_formsId = saControl[index].GetData("usr_formsId");
            wxString usr_pagesId = saControl[index].GetData("usr_pagesId");
            wxString formName = saControl[index].GetData("formName");
            wxString xPos = saControl[index].GetData("xPosition");
            wxString yPos = saControl[index].GetData("yPosition");
            wxString width = saControl[index].GetData("width");
            wxString height = saControl[index].GetData("height");


            //Load the data and create the table object.
            //wxString sTableID = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,saControlId[index]);

            //The table doesn't exist, so add it.
            ObForm * pCtl = NewCtlObject();
            pCtl->SetFormID(usr_formsId);
            pCtl->SetPageID(usr_pagesId);
            pCtl->SetFormName(formName);

            int lxPos = Utility::StringToInt(xPos);
            int lyPos = Utility::StringToInt(yPos);

            if(lxPos> m_sizeWinObjectsExtend.x){
                m_sizeWinObjectsExtend.x = lxPos;
            }
            if(lyPos> m_sizeWinObjectsExtend.y){
                m_sizeWinObjectsExtend.y = lyPos;
            }

            wxPoint pt(Utility::StringToInt(xPos),Utility::StringToInt(yPos));
            pCtl->SetFormPosition(pt);

            m_FormList.Add(pCtl);

        }
        RedrawFormObjects(); // Draw them to the screen.
    }

}

void DesignPagePanel::SetQuery(wxString sQuery)
{
    m_sBuildQuery=sQuery;
}

wxString DesignPagePanel::GetQuery()
{
    return m_sBuildQuery;
}

ObForm* DesignPagePanel::NewCtlObject()
{
    ObForm* pCtl= new ObForm;
    return pCtl;
}

//We can send a message to the parent that this window is destroyed.
bool DesignPagePanel::Destroy()
{

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    //bool bResult = wxPanel::Destroy();
    return true;
}
