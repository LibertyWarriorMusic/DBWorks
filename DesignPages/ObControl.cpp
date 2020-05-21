//
// Created by Nicholas Zounis on 20/4/20.
//
#include<wx/wx.h>

#include "../Shared/global.h"
#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "ObControl.h"

ObControl::ObControl()
{
    m_sControlId=""; //This is the d
    m_sTypeName="";
    m_sTypeId="";
    m_sFormId="";
    m_sLabel="";
    m_sDescription="";
    m_sField="";

    m_ControlRect.x=0;
    m_ControlRect.y=0;
    m_ControlRect.width=300;
    m_ControlRect.height=25;

    m_gridDistance.x=20;
    m_gridDistance.y=20;
    m_bSnapToGrid=true;

}

wxString ObControl::SaveDB(wxString sControlId)
{

    if(sControlId.IsEmpty()){

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
    }

    wxString posX;
    wxString posY;
    wxString show="no";
    posX << m_ControlRect.x;
    posY << m_ControlRect.y;

    wxString keyX="ObControlPositionX";
    wxString keyY="ObControlPositionY";
    wxString keyShow="ObControlShow";

    //if(m_bShowTable)
        show="yes";

    Utility::SaveTableData(Settings.sDatabase,"usr_controls",GetControlID(),keyX ,posX);
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",GetControlID(),keyY ,posY);
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",GetControlID(),keyShow ,show);

    // Utility::SaveTableDataBulk(Settings.sDatabase,m_sTableId, "<key:ObTablePositionX>"+posX+"</key><key:ObTablePositionY>"+posY+"<key:ObTableShow>"+show+"</key>");


    return GetControlID();

}

void ObControl::RestorePreviousSnapCondition()
{
    m_bSnapToGrid = m_SnapToGridRememberState;

}

wxRect ObControl::GetControlRect()
{
    return m_ControlRect;
}

wxString ObControl::GetTypeName()
{
    return m_sTypeName;
}

wxString ObControl::GetTypeID()
{
    return m_sTypeId;
}

wxString ObControl::GetControlID()
{
    return m_sControlId;
}

void ObControl::SetTypeName(wxString sType)
{
    m_sTypeName= sType;
}

void ObControl::SetControlID(wxString sId)
{
    m_sControlId = sId;
}

void ObControl::SetTypeID(wxString sId)
{
    m_sTypeId = sId;
}

void ObControl::SetFormID(wxString sId)
{
    m_sFormId = sId;
}

void ObControl::SetLabel(wxString sLabel)
{
    m_sLabel=sLabel;
}

void ObControl::SetField(wxString sField)
{
    m_sField=sField;
}

void ObControl::SetAction(wxString sAction)
{

    m_sAction = sAction;
}

void ObControl::SetDescription(wxString sDescription)
{
    m_sDescription=sDescription;
}
wxString ObControl::GetField()
{
    return m_sField;
}

wxString ObControl::GetAction()
{
    return m_sAction;
}

wxString ObControl::GetLabel()
{
    return m_sLabel;
}

wxString ObControl::GetFormID()
{
    return m_sFormId;
}

void ObControl::TurnSnapOff()
{
    m_SnapToGridRememberState = m_bSnapToGrid;
    m_bSnapToGrid =false;

}

bool ObControl::HitTest(wxPoint pt) //Returns true if point is in this objects rect.
{
    //wxString name = GetType();

    if(Utility::IsPointInRect(pt,m_ControlRect))
        return true;

    return false;
}
void ObControl::SetControlPosition(wxPoint pt)
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
    m_ControlRect.x = ptToSnap.x;
    m_ControlRect.y = ptToSnap.y;

    if(m_ControlRect.x < 0)
        m_ControlRect.x=0;

    if(m_ControlRect.y<0)
        m_ControlRect.y=0;

    //Set the default width and heights
    if(GetTypeName()=="CTL_BUTTON")
        m_ControlRect.width=100;
    else if(GetTypeName()=="CTL_CHECKBOX")
        m_ControlRect.width=100;
}

void ObControl::DrawControlObject(wxDC&  dc){

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

    dc.DrawRectangle(m_ControlRect);

    wxString sTextToDraw = m_sTypeName;

    if(!m_sDescription.IsEmpty()){
        sTextToDraw += " - Description: "+m_sDescription;
    }

    if(!m_sLabel.IsEmpty()){
        sTextToDraw += " - Label: "+m_sLabel;
    }

    if(!m_sField.IsEmpty()){
        sTextToDraw += " - Field: "+m_sField;
    }

    if(!m_sAction.IsEmpty()){
        sTextToDraw += " - Action: "+m_sAction;
    }

    dc.DrawText(sTextToDraw,m_ControlRect.x+10,m_ControlRect.y+10);

    wxSize extent=dc.GetTextExtent(sTextToDraw);

    if(extent.x > 200)
        m_ControlRect.width=extent.x+15;

    if(GetTypeName()=="CTL_BUTTON")
        m_ControlRect.width=extent.x+15;
    else if(GetTypeName()=="CTL_CHECKBOX")
        m_ControlRect.width=extent.x+15;




}