//
// Created by Nicholas Zounis on 20/4/20.
//
#include<wx/wx.h>

#include "../Shared/global.h"
#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "ObForm.h"

ObForm::ObForm()
{
    m_FormRect.x=0;
    m_FormRect.y=0;
    m_FormRect.width=300;
    m_FormRect.height=125;

    m_gridDistance.x=20;
    m_gridDistance.y=20;
    m_bSnapToGrid=true;

    m_sFormId="";
    m_sPageId="";

}

wxString ObForm::SaveDB(wxString sFormId)
{

    //Insert a new form.
    if(sFormId.IsEmpty()){
        /*
        //create a new control in the table
        //save teh ID
        wxString QueryString="";

        QueryString << "INSERT INTO usr_controls (usr_formsId,controlName,usr_control_typesId,table_data) VALUES (";
        QueryString << "'" << GetFormID() << "',";
        QueryString << "'" << GetTypeName() << "',"; //Make the control name the type name then the users can change it later.
        QueryString << "'" << GetTypeID() << "',";
        QueryString << "'" << "" << "')";      //Empty table data for now.

        if(Utility::ExecuteQuery(Settings.sDatabase,QueryString)){
            SetControlID(Utility::GetHighestId("usr_controls"));
        }
         */

    }else{
        // Update form
        wxString QueryString="";

        QueryString << "UPDATE usr_forms SET xPosition=" << m_FormRect.x;
        QueryString << ", yPosition=" << m_FormRect.y;
        QueryString << " WHERE usr_formsId = " << sFormId; //Make the control name the type name then the users can change it later.

        Utility::ExecuteQuery(Settings.sDatabase,QueryString);
    }

    return m_sFormId;
}

wxRect ObForm::GetFormRect()
{
    return m_FormRect;
}

void ObForm::SetFormID(wxString sFormId){
    m_sFormId = sFormId;
}

wxString ObForm::GetFormID(){
    return m_sFormId;
}

void ObForm::SetPageID(wxString sPageId){
    m_sPageId = sPageId;
}

void ObForm::RestorePreviousSnapCondition()
{
    m_bSnapToGrid = m_SnapToGridRememberState;

}

void ObForm::TurnSnapOff()
{
    m_SnapToGridRememberState = m_bSnapToGrid;
    m_bSnapToGrid =false;

}
void ObForm::SetFormName(wxString sFormName)
{
    m_sFormName = sFormName;
}
bool ObForm::HitTest(wxPoint pt) //Returns true if point is in this objects rect.
{
    //wxString name = GetType();

    if(Utility::IsPointInRect(pt,m_FormRect))
        return true;

    return false;
}

void ObForm::SetFormPosition(wxPoint pt)
{
    //LIMIT Y
    if(pt.y<0)
        pt.y=0;

    if(pt.x<0)
        pt.x=0;

    wxPoint ptToSnap = pt;
    if(m_bSnapToGrid){
        //Create a rect from the grid distand
        wxRect rect = Utility::CalcGridRectThatPtLiesInside(pt,m_gridDistance);
        //Will find the closest corner to snap to
        ptToSnap = Utility::CalPtSnapToClosestCornerInRect(pt,rect);

    }
    m_FormRect.x = ptToSnap.x;
    m_FormRect.y = ptToSnap.y;

    if(m_FormRect.x < 0)
        m_FormRect.x=0;

    if(m_FormRect.y<0)
        m_FormRect.y=0;

}

void ObForm::DrawFormObject(wxDC&  dc){

    wxFontInfo info;
    info.Italic(true);
    info.Family(wxFONTFAMILY_ROMAN );
    wxFont ftItalic(info);

    wxFontInfo infoBold;
    infoBold.Italic(true);
    infoBold.Bold(true);
    infoBold.Family(wxFONTFAMILY_ROMAN );
    wxFont ftItalicBold(infoBold);

    dc.SetFont(ftItalic);

    dc.SetPen( wxPen( wxColor(150,150,150), 1 ) );
    wxBrush brush(wxColor(150,150,150),wxBRUSHSTYLE_SOLID);

    dc.DrawRectangle(m_FormRect);

    wxString sTextToDraw = "";

    if(!m_sFormName.IsEmpty()){
        sTextToDraw += " Form Name: "+m_sFormName;
    }

    dc.DrawText(sTextToDraw,m_FormRect.x+10,m_FormRect.y+10);

    wxSize extent=dc.GetTextExtent(sTextToDraw);

    if(extent.x > 200)
        m_FormRect.width=extent.x+15;
}
