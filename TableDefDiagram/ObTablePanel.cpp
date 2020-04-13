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
#include "../Dialog/MultiTextDlg.h"
#include "../Dialog/SingleTextDlg.h"
#include "../Generic/GenericQueryGrid.h"

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
    ID_MENU_REMOVE_TABLE,
    ID_BUTTON_ADD_VALUE,
    ID_BUTTON_REMOVE_LAST,
    ID_CHK_AS,
    ID_CHK_EQUAL,
    ID_CHK_LIKE
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
    EVT_BUTTON(ID_BUTTON_ADD_VALUE,ObTablePanel::OnAddValue)
    EVT_BUTTON(ID_BUTTON_REMOVE_LAST,ObTablePanel::OnRemoveValue)
    EVT_MOUSEWHEEL(ObTablePanel::mouseWheelMoved)
    EVT_MENU(ID_MENU_ADD_TABLE, ObTablePanel::OnMenuAddTable)
    EVT_MENU(ID_MENU_REMOVE_TABLE, ObTablePanel::OnMenuRemoveTable)
    EVT_MENU(ID_MENU_EDIT_TABLE_DEFINITIONS, ObTablePanel::OnMenuEditTableDefinitions)
    EVT_MENU(ID_MENU_EDIT_TABLE, ObTablePanel::OnMenuEditTable)
    EVT_CHECKBOX(ID_CHK_AS,ObTablePanel::OnChkAs)
    EVT_CHECKBOX(ID_CHK_EQUAL,ObTablePanel::OnChkEqual)
    EVT_CHECKBOX(ID_CHK_LIKE,ObTablePanel::OnChkLike)
// catch paint events
    EVT_PAINT(ObTablePanel::paintEvent)

END_EVENT_TABLE()

ObTablePanel::~ObTablePanel()
{
    DeleteDrawObject();
    textValue->Destroy();
    AddValue->Destroy();
    m_ChkAS->Destroy();
    m_ChkEql->Destroy();
    QueryCombo->Destroy();
    m_bDrawDraggedField=false;
    m_sLastDraggedFieldType="";

    if(m_pFieldDragged!= nullptr)
        delete m_pFieldDragged;

    if(m_pObCurrentTable!= nullptr)
        delete m_pObCurrentTable;
}

void ObTablePanel::OnChkAs(wxCommandEvent& event)
{
    m_ChkAS->SetValue(true);
    m_ChkEql->SetValue(false);
    m_ChkLike->SetValue(false);
}
void ObTablePanel::OnChkEqual(wxCommandEvent& event)
{
    m_ChkEql->SetValue(true);
    m_ChkAS->SetValue(false);
    m_ChkLike->SetValue(false);
}

void ObTablePanel::OnChkLike(wxCommandEvent& event)
{
    m_ChkLike->SetValue(true);
    m_ChkAS->SetValue(false);
    m_ChkEql->SetValue(false);
}

ObTablePanel::ObTablePanel(wxFrame* parent) : wxPanel(parent)
{
    m_pObjectToDrawTEMP = nullptr;
    m_pObCurrentTable = nullptr;
    m_pFieldDragged = nullptr;
    m_MouseMoveOffset.x=0;
    m_MouseMoveOffset.y=0;
    m_sizeWinObjectsExtend.x=0;
    m_sizeWinObjectsExtend.y=0;

    m_sBuildQuery="";
    sPrevTableName="";
    bHitClear=false;
    bHitRunQuery=false;

    m_queryBuilderRect.x=5;
    m_queryBuilderRect.y=5;
    m_queryBuilderRect.width=500;
    m_queryBuilderRect.height=HEIGHT_QUERY_BUILDER;

    //----------------------
    //Right Button Rects
    m_ClearBuilderRect.x=5;
    m_ClearBuilderRect.y=5;
    m_ClearBuilderRect.width=80;
    m_ClearBuilderRect.height=20;

    m_EditQueryRect.x=5;
    m_EditQueryRect.y=30;
    m_EditQueryRect.width=80;
    m_EditQueryRect.height=20;

    m_SaveQueryRect.x=5;
    m_SaveQueryRect.y=55;
    m_SaveQueryRect.width=80;
    m_SaveQueryRect.height=20;

    m_RunQueryRect.x=5;
    m_RunQueryRect.y=80;
    m_RunQueryRect.width=80;
    m_RunQueryRect.height=20;



    //----------------------

    m_rectLeftJoin.x=5;
    m_rectLeftJoin.y=HEIGHT_QUERY_BUILDER-15;
    m_rectLeftJoin.width=80;
    m_rectLeftJoin.height=20;

    m_rectInnerJoin.x=84;
    m_rectInnerJoin.y=HEIGHT_QUERY_BUILDER-15;
    m_rectInnerJoin.width=80;
    m_rectInnerJoin.height=20;

    m_rectRightJoin.x=163;
    m_rectRightJoin.y=HEIGHT_QUERY_BUILDER-15;
    m_rectRightJoin.width=80;
    m_rectRightJoin.height=20;

    m_rectWhere.x=300;
    m_rectWhere.y=HEIGHT_QUERY_BUILDER-15;
    m_rectWhere.width=80;
    m_rectWhere.height=20;

    m_rectOn.x=379;
    m_rectOn.y=HEIGHT_QUERY_BUILDER-15;
    m_rectOn.width=80;
    m_rectOn.height=20;

    m_rectEquals.x=458;
    m_rectEquals.y=HEIGHT_QUERY_BUILDER-15;
    m_rectEquals.width=80;
    m_rectEquals.height=20;

    textValue = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxPoint(600,HEIGHT_QUERY_BUILDER-18), wxSize(200,20), 0);
    AddValue = new wxButton( this, ID_BUTTON_ADD_VALUE, "Add Value", wxPoint(810,HEIGHT_QUERY_BUILDER-18), wxSize(100,20), wxTB_DEFAULT_STYLE);
    RemoveValue = new wxButton( this, ID_BUTTON_REMOVE_LAST, "Undo", wxPoint(920,HEIGHT_QUERY_BUILDER-18), wxSize(80,20), wxTB_DEFAULT_STYLE);


    QueryCombo = new wxComboBox( this, wxID_ANY, "Saved queries will appear here.", wxPoint(1090,HEIGHT_QUERY_BUILDER-18), wxSize(250,25), 0,0,wxCB_READONLY);
    QueryCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( ObTablePanel::OnDatabaseComboChange ), nullptr, this );
    LoadQueryCombo();
    QueryCombo->SetSelection(0);

    m_ChkAS = new wxCheckBox(this,ID_CHK_AS,"AS", wxPoint(470,HEIGHT_QUERY_BUILDER-18) );
    m_ChkLike = new wxCheckBox(this,ID_CHK_LIKE,"LIKE", wxPoint(510,HEIGHT_QUERY_BUILDER-18) );
    m_ChkEql = new wxCheckBox(this,ID_CHK_EQUAL,"=", wxPoint(560,HEIGHT_QUERY_BUILDER-18) );

    m_ChkEql->SetValue(true);
}

void ObTablePanel::LoadQueryCombo()
{
    QueryCombo->Clear();
    QueryCombo->Append(" -- QUERIES ARE SAVED HERE --");
    wxArrayString sSelectionItemArray;
    Utility::LoadStringArrayAvailableQueriesFromUsrQueries(sSelectionItemArray);
    Utility::FillComboFromStringArray(QueryCombo,sSelectionItemArray);

}

void ObTablePanel::OnDatabaseComboChange(wxCommandEvent& event)
{

    wxArrayString ArrayFieldRecords;
    Utility:: GetSingleFieldRecords("SELECT queryDefinition FROM usr_queries WHERE queryName='"+QueryCombo->GetValue()+"' LIMIT 1","queryDefinition",ArrayFieldRecords);

    if(ArrayFieldRecords.GetCount()==1){
        m_sBuildQuery=ArrayFieldRecords[0];
        wxClientDC dc(this);
        render(dc);
    }
}
void ObTablePanel::DrawQueryBuilderArea(wxDC& dc)
{
    wxString txt="";
    wxSize txtExtent;

    int screenWidth = GetRect().width;
    m_queryBuilderRect.width = screenWidth-10;
    dc.DrawRectangle(m_queryBuilderRect);

    wxFontInfo info;
    info.Light(true);
    info.Family(wxFONTFAMILY_ROMAN );
    wxFont ft(info);

    dc.SetFont(ft);

    dc.SetPen( wxPen( wxColor(100,100,100), 1 ) );

    wxBrush brush(wxColor(150,150,150),wxBRUSHSTYLE_SOLID);
    dc.SetBrush(brush); // green filling

    m_ClearBuilderRect.x = screenWidth-m_ClearBuilderRect.width-5;
    txt="Clear";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_ClearBuilderRect);
    dc.DrawText(txt,(screenWidth-m_ClearBuilderRect.width-5) + ((m_ClearBuilderRect.width - txtExtent.x)/2),9);

    m_EditQueryRect.x = screenWidth-m_EditQueryRect.width-5;
    txt="Edit";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_EditQueryRect);
    dc.DrawText(txt,(screenWidth-m_EditQueryRect.width-5) + ((m_EditQueryRect.width - txtExtent.x)/2),34);

    m_SaveQueryRect.x = screenWidth-m_SaveQueryRect.width-5;
    txt="Save";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_SaveQueryRect);
    dc.DrawText(txt,(screenWidth-m_SaveQueryRect.width-5) + ((m_SaveQueryRect.width - txtExtent.x)/2),59);

    m_RunQueryRect.x = screenWidth-m_RunQueryRect.width-5;
    txt="Run";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_RunQueryRect);
    dc.DrawText(txt,(screenWidth-m_RunQueryRect.width-5) + ((m_RunQueryRect.width - txtExtent.x)/2),84);

    wxBrush brush2(wxColor(255,255,255),wxBRUSHSTYLE_SOLID);
    dc.SetBrush(brush2); // green filling

    txt="LEFT JOIN";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_rectLeftJoin);
    dc.DrawText(txt,(m_rectLeftJoin.width - txtExtent.x)/2,HEIGHT_QUERY_BUILDER-11);

    txt="INNER JOIN";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_rectInnerJoin);
    dc.DrawText(txt,((m_rectInnerJoin.width - txtExtent.x)/2) + 84,HEIGHT_QUERY_BUILDER-11);

    txt="RIGHT JOIN";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_rectRightJoin);
    dc.DrawText(txt,((m_rectRightJoin.width - txtExtent.x)/2) + 163,HEIGHT_QUERY_BUILDER-11);

    txt="WHERE";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_rectWhere);
    dc.DrawText(txt,((m_rectWhere.width - txtExtent.x)/2) + 300,HEIGHT_QUERY_BUILDER-11);

    txt="ON";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_rectOn);
    dc.DrawText(txt,((m_rectOn.width - txtExtent.x)/2) + 379,HEIGHT_QUERY_BUILDER-11);
/*
    txt="EQUALS";
    txtExtent = dc.GetTextExtent(txt);
    dc.DrawRectangle(m_rectEquals);
    dc.DrawText(txt,((m_rectEquals.width - txtExtent.x)/2) + 458,HEIGHT_QUERY_BUILDER-11);
*/
    if(m_sBuildQuery.IsEmpty()){
        txt="Drag fields here or on one of the mysql keyword rectangles to build query";
    } else
        txt = m_sBuildQuery;

    txtExtent = dc.GetTextExtent(txt);
    int startX=(screenWidth- txtExtent.x)/2;
    if(startX < 100)
        startX=100;
    dc.DrawText(txt,startX,19);
}

//Build the query
void ObTablePanel::DetermineQuery(wxString sTableName, wxString sFieldName, wxPoint mousePt)
{

    bool bDoesNotHaveLeftJoin = (m_sBuildQuery.Find(" LEFT JOIN ")==wxNOT_FOUND);
    bool bDoesNotHaveInnorJoin = (m_sBuildQuery.Find(" INNER JOIN ")==wxNOT_FOUND);
    bool bDoesNotHaveRightJoin = (m_sBuildQuery.Find(" RIGHT JOIN ")==wxNOT_FOUND);
    bool bDoesNotHaveAstroFrom = (m_sBuildQuery.Find(" * FROM ")==wxNOT_FOUND);
    bool bDoesNotHaveSelect = (m_sBuildQuery.Find(" SELECT ")==wxNOT_FOUND);

    bool bHasOn = (m_sBuildQuery.Find(" ON ")!=wxNOT_FOUND);
    bool bHasEquals = (m_sBuildQuery.Find(" = ")!=wxNOT_FOUND);
    bool bHasWhere = (m_sBuildQuery.Find(" WHERE ")!=wxNOT_FOUND);

    int posOfFrom = m_sBuildQuery.Find(" FROM ")+1; //account for the space at the front
    int posOfOn = m_sBuildQuery.Find(" ON ")+1; //account for the space at the front
    int QueryLength = m_sBuildQuery.Length();

    int MaxQueryWidth = GetRect().width-100;

    if(!sTableName.IsEmpty() && !sFieldName.IsEmpty()){

        if(sFieldName=="*"){

            if(m_sBuildQuery.IsEmpty()){
                //Left Join
                if(Utility::IsPointInRect(m_MousePos,m_rectLeftJoin)) {
                    m_sBuildQuery = " SELECT * FROM " + sTableName + "\n LEFT JOIN ";
                    sPrevTableName=sTableName;
                }
                else if(Utility::IsPointInRect(m_MousePos,m_rectInnerJoin)) {

                    m_sBuildQuery = " SELECT * FROM " + sTableName + "\n INNER JOIN ";
                    sPrevTableName=sTableName;
                }
                else if(Utility::IsPointInRect(m_MousePos,m_rectRightJoin)) {

                    m_sBuildQuery = " SELECT * FROM " + sTableName + "\n RIGHT JOIN ";
                    sPrevTableName=sTableName;
                }
                else{
                    m_sBuildQuery = " SELECT * FROM " + sTableName;
                    sPrevTableName=sTableName;
                }
            }
            else{
                if(Utility::IsPointInRect(m_MousePos,m_rectLeftJoin)) {
                    m_sBuildQuery += " LEFT JOIN " + sTableName;
                    sPrevTableName=sTableName;
                }
                else if(Utility::IsPointInRect(m_MousePos,m_rectInnerJoin)) {

                    m_sBuildQuery += " INNER JOIN " + sTableName;
                    sPrevTableName=sTableName;
                }
                else if(Utility::IsPointInRect(m_MousePos,m_rectRightJoin)) {

                    m_sBuildQuery += " RIGHT JOIN " + sTableName;
                    sPrevTableName=sTableName;
                }
                else if(!bDoesNotHaveLeftJoin || !bDoesNotHaveInnorJoin || !bDoesNotHaveRightJoin){
                    //We have a join and dropped an *, this means at the table
                    m_sBuildQuery += " " + sTableName;
                }
            }
        }
        else
        {
            if(Utility::IsPointInRect(m_MousePos,m_rectLeftJoin) && bDoesNotHaveLeftJoin ) {
                //m_sBuildQuery = " SELECT "+sTableName+"."+sFieldName+"\n FROM " + sTableName + " LEFT JOIN ";
                m_sBuildQuery = m_sBuildQuery + " LEFT JOIN " + sTableName + " \n" + " ON " + sTableName +"." + sFieldName;
                sPrevTableName=sTableName;
            }
            else if(Utility::IsPointInRect(m_MousePos,m_rectInnerJoin) && bDoesNotHaveInnorJoin) {
                //m_sBuildQuery = " SELECT "+sTableName+"."+sFieldName+"\n FROM " + sTableName + " INNER JOIN ";
                m_sBuildQuery = m_sBuildQuery + " INNER JOIN " + sTableName + " \n" + " ON " + sTableName +"." + sFieldName;
                sPrevTableName=sTableName;

            }
            else if(Utility::IsPointInRect(m_MousePos,m_rectRightJoin) && bDoesNotHaveRightJoin) {
                //m_sBuildQuery = " SELECT "+sTableName+"."+sFieldName+"\n FROM " + sTableName + " RIGHT JOIN ";
                m_sBuildQuery = m_sBuildQuery + " RIGHT JOIN " + sTableName + " \n" + " ON " + sTableName +"." + sFieldName;
                sPrevTableName=sTableName;
            }
            else if(Utility::IsPointInRect(m_MousePos,m_rectOn) || bHasOn) {
                if(bHasOn && !bHasEquals && m_sBuildQuery.Right(3)!= "ON ")
                {
                    m_sBuildQuery += " = " + sTableName + "." + sFieldName;
                }else if(m_sBuildQuery.Right(3)== "ON ")
                {
                    m_sBuildQuery += sTableName + "." + sFieldName;
                }
                else if(bHasOn && bHasEquals)
                {
                    m_sBuildQuery += " " + sTableName + "." + sFieldName;
                }
                else{
                    m_sBuildQuery += "\n ON " + sTableName + "." + sFieldName;
                }
            }
            else if(Utility::IsPointInRect(m_MousePos,m_rectWhere) || bHasWhere) {
                if(bHasWhere && !bHasEquals){
                    m_sBuildQuery += " = " + sTableName + "." + sFieldName;

                }
                else if (bHasWhere && bHasEquals){
                    m_sBuildQuery += sTableName + "." + sFieldName;
                }else{
                    m_sBuildQuery += "\n WHERE " + sTableName + "." + sFieldName;
                }
            }
            else if(!bDoesNotHaveLeftJoin || !bDoesNotHaveInnorJoin || !bDoesNotHaveRightJoin){//We already have a join, append the field name to it

                if(m_sBuildQuery.Find(sTableName+"."+sFieldName)==wxNOT_FOUND && bDoesNotHaveAstroFrom && !bHasOn){ // Don't add the same field twice

                    if(posOfFrom!=wxNOT_FOUND){
                        wxString sLeft = m_sBuildQuery.Left(posOfFrom-2);
                        wxString sRight = m_sBuildQuery.Right(QueryLength-posOfFrom);
                        m_sBuildQuery = sLeft + ", " + sTableName + "." + sFieldName + " \n "+ sRight;
                        //m_sBuildQuery = sLeft + +",\n " + sTableName + "." + sFieldName + " \n "+ sRight;
                        sPrevTableName=sTableName;
                    }
                    else{
                        m_sBuildQuery = m_sBuildQuery.Right(m_sBuildQuery.Length() - 7); // Chop off select
                        m_sBuildQuery = " SELECT " + sTableName + "." + sFieldName + " ,\n " + m_sBuildQuery;
                        sPrevTableName=sTableName;
                    }

                }
                else if(m_sBuildQuery.Find(sTableName+"."+sFieldName)==wxNOT_FOUND && bDoesNotHaveAstroFrom){
                        // Just paste at the end, after the on
                        m_sBuildQuery += " " + sTableName + "." + sFieldName;
                }
            }
            else {
                if(m_sBuildQuery.IsEmpty()){
                    m_sBuildQuery = " SELECT " + sTableName+"."+sFieldName  +"\n FROM " + sTableName;
                    sPrevTableName=sTableName;
                }else{
                    if(m_sBuildQuery.Find(sTableName+"."+sFieldName)==wxNOT_FOUND && bDoesNotHaveAstroFrom) { // Don't add the same field twice

                       if(posOfFrom!=wxNOT_FOUND){
                           wxString sLeft = m_sBuildQuery.Left(posOfFrom-2);//The -2 is to remove the /n
                           wxString sRight = m_sBuildQuery.Right(QueryLength-posOfFrom);
                           m_sBuildQuery = sLeft.Trim(true)  +", " + sTableName + "." + sFieldName + " \n "+ sRight;
                           sPrevTableName=sTableName;
                           //Place a new line if too long
                           //m_sBuildQuery = sLeft  +",\n " + sTableName + "." + sFieldName + " \n "+ sRight;
                       }
                       else{
                           m_sBuildQuery = m_sBuildQuery.Right(m_sBuildQuery.Length() - 7); // Chop off select
                           m_sBuildQuery = " SELECT " + sTableName + "." + sFieldName + " ,\n " + m_sBuildQuery;
                           sPrevTableName=sTableName;
                       }
                    }
                }
            }
        }           //Left Join
    }
}

void ObTablePanel::OnAddValue(wxCommandEvent& event)
{
    //Get the data type for the last field;
    bool IsString=false;

    if(!m_sLastDraggedFieldType.IsEmpty()){
        m_sLastDraggedFieldType.Lower(); // this is ok, it can stay lower.
        if(m_sLastDraggedFieldType.find("varchar")!=wxNOT_FOUND || m_sLastDraggedFieldType.find("text")!=wxNOT_FOUND)
            IsString=true;
    }

    if(m_ChkEql->GetValue()){
        bool bHasEquals = (m_sBuildQuery.Find(" = ")!=wxNOT_FOUND || m_sBuildQuery.Find(" =")!=wxNOT_FOUND);
        wxString sValue = textValue->GetValue();
        if(!sValue.IsEmpty()){

            if(IsString && sValue.Find("'")==wxNOT_FOUND && sValue.Find("\"")==wxNOT_FOUND){
                if (bHasEquals)
                    m_sBuildQuery += " '" + sValue +"'";
                else
                    m_sBuildQuery += " = '" + sValue +"'";

            }else{
                if (bHasEquals)
                    m_sBuildQuery += " " + sValue;
                else
                    m_sBuildQuery += " = " + sValue;
            }

            wxClientDC dc(this);
            render(dc);
        }
    }
    else if(m_ChkAS->GetValue()){
        int posOfFrom = m_sBuildQuery.Find(" FROM ")+1; //account for the space at the front
        int QueryLength = m_sBuildQuery.Length();
        bool bDoesNotHaveAstroFrom = (m_sBuildQuery.Find(" * FROM ")==wxNOT_FOUND);
        //AS
        if(posOfFrom!=wxNOT_FOUND && bDoesNotHaveAstroFrom){
            wxString sLeft = m_sBuildQuery.Left(posOfFrom-2);
            wxString sRight = m_sBuildQuery.Right(QueryLength-posOfFrom);
            m_sBuildQuery = sLeft.Trim(true) + " AS " + textValue->GetValue() + " \n " + sRight;

            wxClientDC dc(this);
            render(dc);
        }
    }
    else if(m_ChkLike->GetValue()){
        bool bHasLike = (m_sBuildQuery.Find(" LIKE ")!=wxNOT_FOUND || m_sBuildQuery.Find(" LIKE")!=wxNOT_FOUND);
        wxString sValue = textValue->GetValue();
        if(!sValue.IsEmpty()){

            if(IsString && sValue.Find("'")==wxNOT_FOUND && sValue.Find("\"")==wxNOT_FOUND){
                if (bHasLike)
                    m_sBuildQuery += " '" + sValue +"'";
                else
                    m_sBuildQuery += " LIKE '" + sValue +"'";

            }else{
                if (bHasLike)
                    m_sBuildQuery += " " + sValue;
                else
                    m_sBuildQuery += " LIKE " + sValue;
            }

            wxClientDC dc(this);
            render(dc);
        }
    }

}

void ObTablePanel::OnRemoveValue(wxCommandEvent& event) {
/*    int posOfEquals = m_sBuildQuery.Find(" = ");
    if(posOfEquals!= wxNOT_FOUND){
        m_sBuildQuery = m_sBuildQuery.Left(posOfEquals);

        wxClientDC dc(this);
        render(dc);
    }*/

    m_sBuildQuery.Trim(true);
    char spaceCH = 32; // Look for a space starting from the end.

    int posOfEquals = m_sBuildQuery.Find(spaceCH,true);
    if(posOfEquals!= wxNOT_FOUND){
        m_sBuildQuery = m_sBuildQuery.Left(posOfEquals) + " ";


    }
    else
        m_sBuildQuery="";

    wxClientDC dc(this);
    render(dc);
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

// Testing draw the position when the mouse is down.
void ObTablePanel::mouseDown(wxMouseEvent& event) {

    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw
    wxString fieldName="";
    wxRect fieldRect;
    ObTable* pObTable = GetObjectHitByMouse(m_MousePos,fieldName,fieldRect);
    if(pObTable!= nullptr)
    {
        if(!fieldName.IsEmpty()){

            if(m_pFieldDragged!= nullptr)
                delete m_pFieldDragged;

            //We want to delete this object when we are finished with it.
            m_pFieldDragged = new FieldRect();
            m_pFieldDragged->m_sFieldName = fieldName;
            m_pFieldDragged->m_Rect=fieldRect;
            m_bDrawDraggedField=true;
        }
        else
            m_MouseMoveOffset=Utility::CalcPtInRectOffset(m_MousePos,pObTable->GetObjectRect());

        //NOTE we never want to delete m_pObjectToDrawTEMP because this is the object on the screen, but we want to make the pointer null when we are finished with it.
        m_pObjectToDrawTEMP = pObTable;
        m_pObjectToDrawTEMP->TurnSnapOff();
    }else if(Utility::IsPointInRect(m_MousePos,m_ClearBuilderRect))
        bHitClear=true;
    else if(Utility::IsPointInRect(m_MousePos,m_RunQueryRect))
        bHitRunQuery=true;


    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_sData="mouseDown";
    GetParent()->ProcessWindowEvent( my_event );
}

void ObTablePanel::mouseMoved(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    if (m_pObjectToDrawTEMP != nullptr){

        wxPoint pos;

        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;

        //We will either drag the object or drag the field
        if(m_bDrawDraggedField && m_pFieldDragged != nullptr){

            m_pFieldDragged->m_Rect.x=pos.x;
            m_pFieldDragged->m_Rect.y=pos.y;

            wxClientDC dc(this);
            m_pFieldDragged->Draw(dc);

        }else{

            //Drag the object
            m_pObjectToDrawTEMP->SetObjectPosition(pos);

            //This is going to be too show. We need to only move the object.
            wxClientDC dc(this);
            m_pObjectToDrawTEMP->DrawObject(dc);
            DrawTableLinkageLines(dc);
        }
    }
}

void ObTablePanel::mouseReleased(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    if(m_pObjectToDrawTEMP!=nullptr){

        //Restore the snap condition to what is was before we moved the object
        m_pObjectToDrawTEMP->RestorePreviousSnapCondition();
        wxPoint pos;
        pos.x = m_MousePos.x - m_MouseMoveOffset.x;
        pos.y = m_MousePos.y - m_MouseMoveOffset.y;


        if(m_bDrawDraggedField && m_pFieldDragged != nullptr ){
            m_pFieldDragged->m_Rect.x=m_MousePos.x;
            m_pFieldDragged->m_Rect.y=m_MousePos.y;

            m_bDrawDraggedField=false;
            wxClientDC dc(this);
            m_pFieldDragged->Draw(dc);

            wxString sFieldName = m_pFieldDragged->m_sFieldName;

            m_sLastDraggedFieldType=m_pObjectToDrawTEMP->GetFieldTypeByFieldName(sFieldName);
            //NOTE: Do not delete this pointer here, we need it to lookup type information

            if(Utility::IsPointInRect(m_MousePos,m_queryBuilderRect))
                DetermineQuery(m_pObjectToDrawTEMP->GetTableName(),sFieldName,m_MousePos);
            //This is where we want to do something.
            //We are going to drag the field into a box area that will create a form.

        }else{

            m_pObjectToDrawTEMP->SetObjectPosition(pos); //This also checks the limits and doesn't draw the object in the query builder section.

            wxClientDC dc(this);
            m_pObjectToDrawTEMP->DrawObject(dc);

            m_pObjectToDrawTEMP->SaveDB(); //We can save it directly
            //Reset these
        }

        m_pObjectToDrawTEMP = nullptr;
        m_MouseMoveOffset.x=0;
        m_MouseMoveOffset.y=0;

    }
    else if(Utility::IsPointInRect(m_MousePos,m_ClearBuilderRect) && bHitClear){
        bHitClear=false;
        m_sBuildQuery="";
        sPrevTableName="";
        QueryCombo->SetSelection(0);
        textValue->SetValue("");
        wxClientDC dc(this);
        render(dc);
    }
    else if(Utility::IsPointInRect(m_MousePos,m_RunQueryRect)){
        bHitRunQuery=false;
        RunQuery(m_sBuildQuery.Left(300)+" ...",m_sBuildQuery);
    }
    else if(Utility::IsPointInRect(m_MousePos,m_EditQueryRect)){
        bHitEdit=false;

        MultiTextDlg * pDlg = new MultiTextDlg(this, "Edit Query", "Query");

        pDlg->SetDataValue("ID_TEXT",m_sBuildQuery);
        if(pDlg->ShowModal()==wxOK) {

            m_sBuildQuery = pDlg->GetDataValue("ID_TEXT");
            wxClientDC dc(this);
            render(dc);
        }
        pDlg->Destroy();


    }
    else if(Utility::IsPointInRect(m_MousePos,m_SaveQueryRect)){

        bool bFail=false;
        wxString initialName = QueryCombo->GetValue();
        bHitSave=false;
        SingleTextDlg * pSaveDlg = new SingleTextDlg(this, "The query is saved to form queries on tables definition page.", "Query Name");
        bool bInsert = (QueryCombo->GetSelection()==0); //If we are
        if(!bInsert)
            pSaveDlg->SetDataValue("ID_TEXT",QueryCombo->GetValue());
        if(pSaveDlg->ShowModal()==wxOK) {

            wxString sQueryName = pSaveDlg->GetDataValue("ID_TEXT");

            if(sQueryName!=initialName)
                bInsert = true;

            //Check to see if the user changed the name

            if(bInsert){
                //Make sure it doesn't already exist.
                if(!Utility::DoesRecordExist(Settings.sDatabase,"usr_queries","queryName",sQueryName)){
                    wxString sQueryString = "INSERT INTO usr_queries (queryName,queryDefinition,description) VALUES ('"+sQueryName+"','"+m_sBuildQuery+"','')";
                    Utility::ExecuteQueryEscapeAscii(sQueryString);

                    LoadQueryCombo();
                    QueryCombo->SetValue(sQueryName);

                    //Reload the combo
                }
                else{
                    bFail=true;
                }

            }
            else{
                //Updating
                wxString sQueryString = "UPDATE usr_queries SET queryDefinition = '"+m_sBuildQuery+"' WHERE   queryName = '"+sQueryName +"'";
                Utility::ExecuteQueryEscapeAscii(sQueryString);
            }

            //We need to create a new entry in the Form queries.
        }
        pSaveDlg->Destroy();

        if(bFail){
            wxLogMessage("Failed to save because you changed the name to an existing saved query. Either change the name or select the query from the selection box and try again.");
        }

    }
    bHitClear=false;


    MyEvent my_event( this );
    my_event.m_bStatusMessage=true;
    my_event.m_sData="mouseReleased";
    GetParent()->ProcessWindowEvent( my_event );
}

//Run the query for testing.

void ObTablePanel::RunQuery(wxString sTitle, wxString sFormGridQuery)
{

    //Remove the /n
    sFormGridQuery.Replace("\n","");
    sFormGridQuery.Replace("\"","'");

    GenericQueryGrid * m_pQueryGrid = new GenericQueryGrid((wxFrame*) this, -1,sTitle,wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE);

    if (m_pQueryGrid != nullptr){
        m_pQueryGrid->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword);
        m_pQueryGrid->SetFormQuery(sFormGridQuery);
        if(m_pQueryGrid->Create())
            m_pQueryGrid->Show(true);
        else{
            m_pQueryGrid->Close();
            m_pQueryGrid= nullptr;
        }
    }
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

void ObTablePanel::mouseDoubleClick(wxMouseEvent& event)
{
    m_MousePos =  event.GetPosition(); //Remember the mouse position to draw

    ObTable* pObTable = GetObjectHitByMouse(m_MousePos);
    if(pObTable!= nullptr)
    {
        MyEvent my_event( this );
        my_event.m_bOpen=true;
        my_event.m_sTableName=pObTable->GetTableName();
        my_event.m_sTableId=pObTable->GetTableID();
        GetParent()->ProcessWindowEvent( my_event );
    }
}


 void ObTablePanel::mouseWheelMoved(wxMouseEvent& event)
 {
     MyEvent my_event( this );
     my_event.m_bStatusMessage=true;
     my_event.m_sData="mouseWheelMoved";
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
    DrawQueryBuilderArea(dc);
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

    if(m_bDrawDraggedField && m_pFieldDragged!= nullptr)
        m_pFieldDragged->Draw(dc);
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

            if(pObTable->HitTest(mousePt)){

                return pObTable;
            }

        }
    }
    return nullptr;
}

//The the mouse point hit a field, will return the field name
ObTable * ObTablePanel::GetObjectHitByMouse(wxPoint mousePt, wxString& sHitFieldName, wxRect& sfieldRect)
{
    ObTable * pOb = GetObjectHitByMouse(mousePt);
    if(pOb != nullptr)
        sHitFieldName = pOb->HitTestField(mousePt,sfieldRect);
    return pOb;
}


//Adds a drawing object to the diagram panel.
void ObTablePanel::AddTableObject(const wxString& sTableID, const wxString& sTableName)
{
    ObTable *pObTable= DoesTableObjectExist(sTableName);

    if(pObTable == nullptr)
    {
        //The table doesn't exist, so add it.
        pObTable = AddNewTableObject();


        //Get the selected field list from the database
        ArrayTableField fieldItemList;
        GetSelectedFieldItemListDB(fieldItemList, sTableID,pObTable->m_aListOfLinkageFlags);


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

                            //NOTE: Linked linkages will always link to the start objects primary key regardless of the field value.
                            //HOWEVER: By enforcing this, the user can't SELECTION_LINKE to a ID field that isn't the primarary key
                            if(flagIndex==1 && Settings.ENFORCE_SELECTION_LINKED_TO_PRIMARY_KEY)
                                flagToAdd+= "0;";
                            else
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

                            //NOTE: Linked linkages will always link to the start objects primary key regardless of the field value.
                            if(flagIndex==1 && Settings.ENFORCE_SELECTION_LINKED_TO_PRIMARY_KEY)
                                flagToAdd+= "0;";
                           else
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
    ObTable *pStartObject = pObLink->m_pStartTableObject;

    switch (pObLink->m_LinkageType) {

        case db_link_lookup_name :
            // This will be one of these
            // SELECTION_LOOKUP_NAME{TableName;FieldName;}
            // SELECTION_LINKED_NAME{TableName;FieldName;}

            //We don't have the column numbers available so we have to look it up from the object
            // The field column number needs to be found from the object because it wasn't supplied by the user, only the field name was supplied.
            // We can get the index directly from the objects field list.

            if (pStartObject != nullptr)
                StartFieldIndex = pStartObject->GetFieldIndexByFieldName(pObLink->m_sStartObLinkedFieldName)+1;

            break;
        case db_link_linked_name :
            // This will be one of these
            // SELECTION_LOOKUP_NAME{TableName;FieldName;}
            // SELECTION_LINKED_NAME{TableName;FieldName;}

            //We don't have the column numbers available so we have to look it up from the object
            // The field column number needs to be found from the object because it wasn't supplied by the user, only the field name was supplied.
            // We can get the index directly from the objects field list.



            if (pStartObject != nullptr)
                StartFieldIndex = pStartObject->GetFieldIndexByFieldName(pObLink->m_sStartObLinkedFieldName)+1;
            break;
        case db_link_lookup_id :
            // This will be one of these
            // SELECTION_LOOKUP_ID{TableID;ColumnNumber;}
            // SELECTION_LINKED_ID{TableID;ColumnNumber;}

            //We can determine the offset by the columnNumber
            //It is a simple matter of multiplying it by some factor
            // The field column number was supplied by the user in the SELECTION_LOOKUP_ID OR SELECTION_LINKED_ID

            StartFieldIndex = Utility::StringToInt(pObLink->m_sStartObLinkedFieldColNumber);

            break;
        case db_link_linked_id :

            StartFieldIndex = Utility::StringToInt(pObLink->m_sStartObLinkedFieldColNumber);

            break;


        default:
            break;

    }
    //Set the offset
    pObLink->m_iStartObYOffset= ((StartFieldIndex ) * (heightOfField ) )+ TitleHeight + RELATIONSHIP_DIAGRAM_GAP + RELATIONSHIP_DIAGRAM_GAP+3;
    //The ends objects offset is independant of the linkage beacause the index was found while loading the objects fields from the Database.
    pObLink->m_iEndObYOffset= ((Utility::StringToInt(pObLink->m_sEndObLinkedFieldColNumber) +1) * (heightOfField) )+ TitleHeight+ RELATIONSHIP_DIAGRAM_GAP+ RELATIONSHIP_DIAGRAM_GAP+3;

    /*

    //Set the offset
    pObLink->m_iStartObYOffset= (StartFieldIndex + 1) * (heightOfField ) + TitleHeight + RELATIONSHIP_DIAGRAM_GAP + RELATIONSHIP_DIAGRAM_GAP+3;
    //The ends objects offset is independant of the linkage beacause the index was found while loading the objects fields from the Database.
    pObLink->m_iEndObYOffset= (Utility::StringToInt(pObLink->m_sEndObLinkedFieldColNumber) + 1 ) * (heightOfField) + TitleHeight+ RELATIONSHIP_DIAGRAM_GAP+ RELATIONSHIP_DIAGRAM_GAP+3;

     */
}