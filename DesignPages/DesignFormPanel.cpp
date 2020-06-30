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

#include "ObControl.h"
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

#include "DesignFormPanel.h"
#include <cmath>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayDrawControls);

using namespace mysqlpp;

enum {
    ID_MENU_ADD_CONTROL = wxID_HIGHEST + 600,
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

BEGIN_EVENT_TABLE(DesignFormPanel, wxPanel)
// some useful events

    EVT_MOTION(DesignFormPanel::mouseMoved)
    EVT_LEFT_DOWN(DesignFormPanel::mouseDown)
    EVT_LEFT_DCLICK(DesignFormPanel::mouseDoubleClick)
    EVT_LEFT_UP(DesignFormPanel::mouseReleased)
    EVT_RIGHT_DOWN(DesignFormPanel::rightClick)
    EVT_LEAVE_WINDOW(DesignFormPanel::mouseLeftWindow)
    EVT_KEY_DOWN(DesignFormPanel::keyPressed)
    EVT_KEY_UP(DesignFormPanel::keyReleased)
    EVT_MOUSEWHEEL(DesignFormPanel::mouseWheelMoved)
    EVT_MENU(ID_MENU_EDIT_LINKED_FIELD, DesignFormPanel::OnMenuEditLinkedFields)
    EVT_MENU(ID_MENU_DELETE_CONTROL, DesignFormPanel::OnMenuDeleteControl)
    EVT_MENU(ID_MENU_EDIT_STATIC, DesignFormPanel::OnMenuEditStatic)
    EVT_MENU(ID_MENU_EDIT_DESCRIPTION, DesignFormPanel::OnMenuEditDescription)
    EVT_MENU(ID_MENU_RUN_QUERY, DesignFormPanel::OnRunQuery)
    EVT_MENU(ID_MENU_EDIT_SET_FLAGS, DesignFormPanel::OnSetFlags)
    EVT_MENU(ID_MENU_SELECTION_OPTIONS, DesignFormPanel::OnSetSelectionOptions)
    EVT_MENU(ID_MENU_EDIT_LINKED_TABLE, DesignFormPanel::OnSetLinkedTable)
    EVT_MENU(ID_MENU_MANAGE_ACTIONS, DesignFormPanel::OnManageActions)
    EVT_PAINT(DesignFormPanel::paintEvent)

END_EVENT_TABLE()
void DesignFormPanel::rightClick(wxMouseEvent& event)
{
    m_pObCurrentFormControl= nullptr;

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObControl* pCtl = GetObjectHitByMouse(m_MousePos);
    //Diagram menus
    //Create a context menu to do stuff here.
    auto *menu = new wxMenu;

    if(pCtl!= nullptr)
    {
        m_pObCurrentFormControl = pCtl;

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
        else if(m_pObCurrentFormControl->GetTypeName()=="CTL_GRID_DISPLAY")
        {

        }
        else if(m_pObCurrentFormControl->GetTypeName()!="CTL_RECORD_SELECTOR")
        {
            menu->Append(ID_MENU_EDIT_STATIC, wxT("Set Label"), wxT("Set the Label."));
            menu->Append(ID_MENU_EDIT_LINKED_FIELD, wxT("Set Linked Field"), wxT("Edit table definitions."));

            //Don't do this.
            //menu->Append(ID_MENU_EDIT_SET_FLAGS, wxT("Set Flags"), wxT("Set flags for this controlFS."));
        }


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

DesignFormPanel::~DesignFormPanel()
{
    DeleteDrawObject();
}

DesignFormPanel::DesignFormPanel(wxFrame* parent) : wxPanel(parent)
{
    m_pObjectToDrawTEMP = nullptr;
    m_pObCurrentFormControl = nullptr;
    m_MouseMoveOffset.x=0;
    m_MouseMoveOffset.y=0;
    m_sizeWinObjectsExtend.x=0;
    m_sizeWinObjectsExtend.y=0;
}
void DesignFormPanel::SetFormID(wxString sFormId)
{
    m_sFormId=sFormId;
}

wxString DesignFormPanel::GetFormID()
{
    return m_sFormId;
}
//===================
//MENU EVENT FUNCTIONS
void DesignFormPanel::OnMenuEditStatic(wxCommandEvent& event)
{
    wxString sData = PromptEditSingleTextDialog("Label","Set Label","Label");
    m_pObCurrentFormControl->SetLabel(sData);
    RedrawControlObjects();
}

void DesignFormPanel::OnMenuEditDescription(wxCommandEvent& event)
{
    wxString sData = PromptEditSingleTextDialog("Short_Description","Edit short description","Short Description.");
    m_pObCurrentFormControl->SetDescription(sData);
    RedrawControlObjects();
}

wxString DesignFormPanel::PromptEditSingleTextDialog(wxString sKey, wxString sDialogTitle, wxString sLabel)
{
    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlId = m_pObCurrentFormControl->GetControlID();

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

void DesignFormPanel::OnManageActions(wxCommandEvent& event)
{

    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlId = m_pObCurrentFormControl->GetControlID();
        ManageActionsDlg * pDlg = new ManageActionsDlg(nullptr, sControlId, "Avaliable actions", "Select Action");

        if(pDlg->ShowModal()==wxOK) {

            pDlg->Save();
            //So we can redraw on the screen.
            m_pObCurrentFormControl->SetAction(pDlg->GetAction());
            RedrawControlObjects();

            //We need to create a new entry in the Form queries.
        }
        pDlg->Destroy();

    }
}

void DesignFormPanel::OnMenuEditLinkedFields(wxCommandEvent& event)
{
    if(m_pObCurrentFormControl!= nullptr){

        wxString sControlId = m_pObCurrentFormControl->GetControlID();
        SelectionFieldQueryDlg * pDlg = new SelectionFieldQueryDlg(nullptr, "Avaliable Fields", "Select Field");
        pDlg->SetControlID(sControlId);
        pDlg->SetQuery(m_sBuildQuery);
        pDlg->Load();

        if(pDlg->ShowModal()==wxOK) {

            pDlg->Save();
            //So we can redraw on the screen.
            m_pObCurrentFormControl->SetField(pDlg->GetField());
            RedrawControlObjects();

            //We need to create a new entry in the Form queries.
        }
        pDlg->Destroy();

    }
}


void DesignFormPanel::OnSetFlags(wxCommandEvent& event)
{
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
}

void DesignFormPanel::OnSetSelectionOptions(wxCommandEvent& event)
{
    ListBoxDlg * pDlg = new ListBoxDlg(nullptr, "Selection Options List", "Selection Options");
    wxString sControlId = m_pObCurrentFormControl->GetControlID();
    pDlg->SetControlID(sControlId);
    pDlg->LoadItems();

    if(pDlg->ShowModal()==wxOK)
        pDlg->SaveItems();

    pDlg->Destroy();

}

void DesignFormPanel::OnSetLinkedTable(wxCommandEvent& event)
{
    LinkedTableDlg * pDlg = new LinkedTableDlg(nullptr, "Selection Options List", "Selection Options");
    wxString sControlId = m_pObCurrentFormControl->GetControlID();
    pDlg->SetControlID(sControlId);
    pDlg->Load();

    if(pDlg->ShowModal()==wxOK)
        pDlg->Save();

    pDlg->Destroy();
}

void DesignFormPanel::OnMenuDeleteControl(wxCommandEvent& event)
{
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
    }
}

void DesignFormPanel::OnRunQuery(wxCommandEvent& event) {

    MyEvent my_event( this );
    my_event.m_bOpenRunQuery=true;
    GetParent()->ProcessWindowEvent( my_event );
}

void DesignFormPanel::OnMenuOpenControl(wxCommandEvent& event) {

    if(m_pObCurrentFormControl!= nullptr){
        MyEvent my_event( this );
        my_event.m_bOpen=true;
        my_event.m_sTableName=m_pObCurrentFormControl->GetTypeName();
        my_event.m_sTableId=m_pObCurrentFormControl->GetControlID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}

// Testing draw the position when the mouse is down.
void DesignFormPanel::mouseDown(wxMouseEvent& event) {

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
    wxString fieldName="";
    wxRect fieldRect;
    ObControl* pCtl = GetObjectHitByMouse(m_MousePos,fieldName,fieldRect);
    if(pCtl!= nullptr)
    {
        if(!fieldName.IsEmpty()){

        }
        else
            m_MouseMoveOffset=Utility::CalcPtInRectOffset(m_MousePos,pCtl->GetControlRect());

        //NOTE we never want to delete m_pObjectToDrawTEMP because this is the object on the screen, but we want to make the pointer null when we are finished with it.
        m_pObjectToDrawTEMP = pCtl;
        m_pObjectToDrawTEMP->TurnSnapOff();
    }

    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_sData="mouseDown";
    GetParent()->ProcessWindowEvent( my_event );
}

void DesignFormPanel::mouseMoved(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    if (m_pObjectToDrawTEMP != nullptr){

        wxPoint pos;
        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;

        //Drag the object
        m_pObjectToDrawTEMP->SetControlPosition(pos);

        //This is going to be too show. We need to only move the object.
        wxClientDC dc(this);
        m_pObjectToDrawTEMP->DrawControlObject(dc);

    }
}

void DesignFormPanel::mouseReleased(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    if(m_pObjectToDrawTEMP!=nullptr){

        //Restore the snap condition to what is was before we moved the object
        m_pObjectToDrawTEMP->RestorePreviousSnapCondition();
        wxPoint pos;
        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;

        m_pObjectToDrawTEMP->SetControlPosition(pos); //This also checks the limits and doesn't draw the object in the query builder section.

        wxClientDC dc(this);
        m_pObjectToDrawTEMP->DrawControlObject(dc);

        m_pObjectToDrawTEMP->SaveDB(m_pObjectToDrawTEMP->GetControlID()); //We can save it directly
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

void DesignFormPanel::DeleteDrawObject()
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

wxSize DesignFormPanel::GetSizeDiagramExtend()
{
    wxSize size = m_sizeWinObjectsExtend;
    size.x = m_sizeWinObjectsExtend.x + 300;
    size.y = m_sizeWinObjectsExtend.y + 300;
    return size;
}



void DesignFormPanel::mouseDoubleClick(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObControl* pCtl = GetObjectHitByMouse(m_MousePos);
    if(pCtl!= nullptr)
    {
        MyEvent my_event( this );
        my_event.m_bOpen=true;
        my_event.m_sTableName=pCtl->GetTypeName();
        my_event.m_sTableId=pCtl->GetControlID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}

 void DesignFormPanel::mouseWheelMoved(wxMouseEvent& event)
 {
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseWheelMoved";
     GetParent()->ProcessWindowEvent( my_event );
 }

 void DesignFormPanel::mouseLeftWindow(wxMouseEvent& event)
 {
    /*
     m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseLeftWindow";
     GetParent()->ProcessWindowEvent( my_event );*/
 }
 void DesignFormPanel::keyPressed(wxKeyEvent& event)
 {
   /*  m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="keyPressed";
     GetParent()->ProcessWindowEvent( my_event );*/
 }

 void DesignFormPanel::keyReleased(wxKeyEvent& event)
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

void DesignFormPanel::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void DesignFormPanel::RedrawControlObjects()
{
    wxClientDC dc(this);

    dc.Clear();
    render(dc);

    //dc.Clear(); //NOT SURE why I put this after render, was it a mistake? I should of commented this.

    //Now we might want to put the default background colour on
}

//This is where you place stuff you need to do on refresh.
void DesignFormPanel::Refresh()
{
    RedrawControlObjects();
}
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void DesignFormPanel::render(wxDC&  dc)
{
    //Draw all the object to the screen.
    for (int index=0; index<m_ControlList.GetCount();index++) {

        ObControl* pCtl = &m_ControlList[index];
        if(pCtl!= nullptr)
            pCtl->DrawControlObject(dc);
    }
}

ObControl * DesignFormPanel::GetObjectHitByMouse(wxPoint mousePt)
{
    //Draw all the object to the screen.
    for (int index=0; index<m_ControlList.GetCount();index++) {

        ObControl* pCtl = &m_ControlList[index];
        if(pCtl!= nullptr){

            if(pCtl->HitTest(mousePt)){

                return pCtl;
            }

        }
    }
    return nullptr;
}

//The the mouse point hit a field, will return the field name
ObControl * DesignFormPanel::GetObjectHitByMouse(wxPoint mousePt, wxString& sHitFieldName, wxRect& sfieldRect)
{
    ObControl * pCtl = GetObjectHitByMouse(mousePt);
    //if(pCtl != nullptr)
        //sHitFieldName = pCtl->HitTestField(mousePt,sfieldRect);
    return pCtl;
}

//Adds a drawing object to the diagram panel.
void DesignFormPanel::AddControlObject(const wxString& sTypeID)
{
    ObControl *pCtl = NewCtlObject();
    if(pCtl != nullptr)
    {
        //The Control doesn't exist, so add it.
        pCtl->SetTypeID(sTypeID);
        pCtl->SetControlPosition(m_MousePos);
        pCtl->SetFormID(GetFormID());
        wxArrayString sArray;
        Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_control_types", "typeName", "usr_control_typesId", sTypeID);
        if(sArray.GetCount()==1){
            pCtl->SetTypeName(sArray[0]);
        }
        m_ControlList.Add(pCtl);

        pCtl->SaveDB("");
    }
    RedrawControlObjects(); // Redraw all the objects.
}
void DesignFormPanel::RemoveControlFromList(wxString sControlId)
{
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


}
//Loads all the controls from the database and creates the drawing
void DesignFormPanel::LoadControlObjects()
{
    // If we have existing object, clear the list.
    if(m_ControlList.GetCount()>0)
        m_ControlList.Clear();

    wxString QueryString = "SELECT usr_control_types.usr_control_typesId, usr_control_types.typeName, usr_controls.usr_controlsId, usr_controls.table_data ";
    QueryString += " FROM usr_control_types, usr_controls ";
    QueryString += " WHERE usr_control_types.usr_control_typesId = usr_controls.usr_control_typesId";
    QueryString += " AND usr_controls.usr_formsId = "+GetFormID();

    ArrayFieldRecord saControl;
    Utility::LoadArrayFieldRecordFromQuery(saControl, QueryString);

    int count = saControl.GetCount();
    if(saControl.GetCount()>0){
        for(int index=0;index<count;index++){
            wxString sData="";

            wxString sControlId = saControl[index].GetData("usr_controlsId");
            wxString sTypeName = saControl[index].GetData("typeName");
            wxString sTypeID = saControl[index].GetData("usr_control_typesId");
            wxString sEntireTableData = saControl[index].GetData("table_data");

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,"ObControlShow",sData,sEntireTableData);
            if(sData=="yes"){
                //Load the data and create the table object.
                //wxString sTableID = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,saControlId[index]);

                //The table doesn't exist, so add it.
                ObControl * pCtl = NewCtlObject();
                pCtl->SetControlID(sControlId);
                pCtl->SetFormID(GetFormID());
                pCtl->SetTypeName(sTypeName);
                pCtl->SetTypeID(sTypeID);

                wxString sLabel="";
                wxString sField="";
                wxString xPos = "";
                wxString yPos = "";
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("ObControlPositionX"),xPos);
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("ObControlPositionY"),yPos);

                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Label"),sLabel);
                pCtl->SetLabel(sLabel);
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Short_Description"),sLabel);
                pCtl->SetDescription(sLabel);
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Action"),sLabel);
                pCtl->SetAction(sLabel);
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Field"),sField);
                pCtl->SetField(sField);

                int lxPos = Utility::StringToInt(xPos);
                int lyPos = Utility::StringToInt(yPos);

                if(lxPos> m_sizeWinObjectsExtend.x){
                    m_sizeWinObjectsExtend.x = lxPos;
                }
                if(lyPos> m_sizeWinObjectsExtend.y){
                    m_sizeWinObjectsExtend.y = lyPos;
                }

                wxPoint pt(Utility::StringToInt(xPos),Utility::StringToInt(yPos));
                pCtl->SetControlPosition(pt);

                m_ControlList.Add(pCtl);
            }
        }
        RedrawControlObjects(); // Draw them to the screen.
    }
}

void DesignFormPanel::SetQuery(wxString sQuery)
{
    m_sBuildQuery=sQuery;
}

wxString DesignFormPanel::GetQuery()
{
    return m_sBuildQuery;
}

ObControl* DesignFormPanel::NewCtlObject()
{
    ObControl* pCtl= new ObControl;
    return pCtl;
}
/*
//Check to make sure we already don't have an existing table.
ObControl* DesignFormPanel::DoesControlExist(wxString sControlId)
{
    for (int index=0; index<m_ControlList.GetCount();index++) {

        ObControl *pCtl = &m_ControlList[index];
        if(pCtl->GetID()== sControlId)
            return pCtl;
    }

    return nullptr;
}
*/

//We can send a message to the parent that this window is destroyed.
bool DesignFormPanel::Destroy()
{

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    //bool bResult = wxPanel::Destroy();
    return true;
}
