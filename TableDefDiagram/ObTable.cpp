//
// Created by Nicholas Zounis on 30/3/20.
//
#include<wx/wx.h>

#include "../global.h"
#include "../MyEvent.h"
#include "../Utility.h"
#include "../Generic/GenericTable.h"
#include "ObTable.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayFieldRect);

ObTable::ObTable()
{
    m_sTableName="";
    m_sTableId="";
    m_gridDistance.x=20;
    m_gridDistance.y=20;
    m_bSnapToGrid=true;
    m_bShowTable=true;
    m_TableRect.x=0;
    m_TableRect.y=0;
    m_ObPosition.x=0;
    m_ObPosition.y=0;
}

ObTable::~ObTable()
{

}

wxString ObTable::GetFieldTypeByFieldName(wxString sFieldName)
{
    for(int index=0;index < m_TableFieldList.GetCount();index++){

        TableField * pField= &m_TableFieldList[index];
        if(pField!= nullptr){
            if(pField->fieldName==sFieldName){
                return pField->fieldType;
            }
        }
    }
    return "";
}

wxRect ObTable::GetObjectRect()
{
    return m_TableRect;
}
bool ObTable::HitTest(wxPoint pt) //Returns true if point is in this objects rect.
{
    wxString name = GetTableName();

    if(Utility::IsPointInRect(pt,m_TableRect))
        return true;

    return false;
}


void ObTable::SetObjectPosition(wxPoint pt)
{

    //LIMIT Y
    if(pt.y<=HEIGHT_QUERY_BUILDER+10)
        pt.y=HEIGHT_QUERY_BUILDER+10;

   // if(pt.y<0)
     //   pt.y=0;

    if(pt.x<0)
        pt.x=0;
    
    wxPoint ptToSnap = pt;

    if(m_bSnapToGrid){


        //Create a rect from the grid distand
        wxRect rect = Utility::CalcGridRectThatPtLiesInside(pt,m_gridDistance);

        //Will find the closest corner to snap to
        wxPoint ptToSnap = Utility::CalPtSnapToClosestCornerInRect(pt,rect);

        m_ObPosition = ptToSnap;
    } else{
        m_ObPosition = ptToSnap;
    }

    if(m_ObPosition.x < 0)
        m_ObPosition.x=0;

    if(m_ObPosition.y<0)
        m_ObPosition.y=0;
}
void ObTable::TurnSnapOn()
{
    m_SnapToGridRememberState = m_bSnapToGrid;
    m_bSnapToGrid =true;

}
void ObTable::TurnSnapOff()
{
    m_SnapToGridRememberState = m_bSnapToGrid;
    m_bSnapToGrid =false;

}
void ObTable::RestorePreviousSnapCondition()
{
    m_bSnapToGrid = m_SnapToGridRememberState;

}
void ObTable::SetTableFieldList(const ArrayTableField &fieldItemList)
{
    m_TableFieldList=fieldItemList;
}

void ObTable::SetTableName(wxString sTableName)
{
    m_sTableName=sTableName;
}
void ObTable::SetTableID(wxString sTableId)
{
    m_sTableId=sTableId;
}

wxString ObTable::GetTableName()
{
    return m_sTableName;
}
wxString ObTable::GetTableID()
{
    return m_sTableId;
}

//Save this table to the database so it persists.
//At the moment, you only need to save the x and y position on the screen as wells a display flag
void ObTable::SaveDB()
{
// <key:ObTablePositionX>10</key>
// <key:ObTablePositionY>50</key>
// <key:ObTableShow>yes</key>

    wxString posX;
    wxString posY;
    wxString show="no";
    posX << m_ObPosition.x;
    posY << m_ObPosition.y;

    wxString keyX="ObTablePositionX";
    wxString keyY="ObTablePositionY";
    wxString keyShow="ObTableShow";

    if(m_bShowTable)
        show="yes";

    Utility::SaveTableData(Settings.sDatabase,m_sTableName,keyX ,posX);
    Utility::SaveTableData(Settings.sDatabase,m_sTableName,keyY ,posY);
    Utility::SaveTableData(Settings.sDatabase,m_sTableName,keyShow ,show);

   // Utility::SaveTableDataBulk(Settings.sDatabase,m_sTableId, "<key:ObTablePositionX>"+posX+"</key><key:ObTablePositionY>"+posY+"<key:ObTableShow>"+show+"</key>");
}
int ObTable::GetFieldIndexByFieldName(wxString sFieldName)
{
    if(m_TableFieldList.GetCount()>0){

        for (int index=0; index<m_TableFieldList.GetCount(); index++) {
            TableField fieldItem;
            fieldItem = m_TableFieldList[index];
            if(fieldItem.fieldName==sFieldName)
                return index;
        }

    }

   return wxNOT_FOUND;
}


//Draw the table object
void ObTable::DrawObject(wxDC&  dc)
{
    int GAP = RELATIONSHIP_DIAGRAM_GAP;
    int HEIGHT_TITLE= RELATIONSHIP_DIAGRAM_TITLE_HEIGHT + RELATIONSHIP_DIAGRAM_FIELD_HEIGHT;
    int FieldHeight = RELATIONSHIP_DIAGRAM_FIELD_HEIGHT;

    FieldRect * pFR = nullptr;
    m_FieldRectList.Clear(); // This will store all the field rects for the field hit test

    //If we are hiding this table, don't draw it.
    if (!m_bShowTable)
        return;


    //DRAW THE TABLE

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


    // Draw width label
    int XStartPos = m_ObPosition.x;
    int YStartPos = m_ObPosition.y;

    int XEndPos = XStartPos + 200;
    int XStartOfType = XStartPos + 100;
    wxPoint pt = m_ObPosition;
    wxPoint ptType(0,0);

    pt.y +=GAP;
    pt.y +=HEIGHT_TITLE;

    //Find the largest X and Y
    wxSize extentFieldName(0,0);
    wxSize extentFieldType(0,0);
    wxSize extentTempFieldName(0,0);
    wxSize extentTempFieldType(0,0);

    for (int index=0; index<m_TableFieldList.GetCount(); index++) {
        TableField fieldItem;
        fieldItem = m_TableFieldList[index];
        extentTempFieldName = dc.GetTextExtent(fieldItem.fieldName);
        extentTempFieldType = dc.GetTextExtent(fieldItem.fieldType);

        extentTempFieldName.y = FieldHeight;// Fix the heights to know value for the linkage lines line up
        extentTempFieldType.y = FieldHeight;

        //extentFieldName.x += extentTemp2.x +5; // five for the gap

        if(extentTempFieldName.x > extentFieldName.x )
            extentFieldName.x = extentTempFieldName.x;

        if(extentTempFieldName.y > extentFieldName.y )
            extentFieldName.y = extentTempFieldName.y;

        if(extentTempFieldType.x > extentFieldType.x )
            extentFieldType.x = extentTempFieldType.x;

        if(extentTempFieldType.y > extentFieldType.y )
            extentFieldType.y = extentTempFieldType.y;
    }

    //Check the primary key width
    extentTempFieldName = dc.GetTextExtent(m_sTableName+"Id");
    if(extentTempFieldName.x > extentFieldName.x )
        extentFieldName.x = extentTempFieldName.x;

   // if(extentTempFieldName.y > extentFieldName.y )
    //    extentFieldName.y = extentTempFieldName.y;

    extentTempFieldType = dc.GetTextExtent("int auto");
    if(extentTempFieldType.x > extentFieldType.x )
        extentFieldType.x = extentTempFieldType.x;

  //  if(extentTempFieldType.y > extentFieldType.y )
  //      extentFieldType.y = extentTempFieldType.y;

    // Make sure we have enough room to show the "auto int *"
    if(extentFieldType.x<55)
        extentFieldType.x = 55;

    int width = 0;

    if (extentFieldName.x==0 && extentFieldType.x ==0){
        //Set a default width if we have no fields.
        extentFieldName = dc.GetTextExtent(m_sTableName);
        extentFieldName.x += GAP;
        width = extentFieldName.x + extentFieldType.x + (3*GAP);

    } else{
        extentFieldName.x += GAP;
        width= extentFieldName.x + extentFieldType.x + (3*GAP);
    }

    extentFieldName.x +=GAP;
    extentFieldName.y += GAP;

    ptType.x = pt.x + extentFieldName.x + GAP;
    ptType.y = pt.y;

    int HeightOfTable = m_TableFieldList.GetCount() * (extentFieldName.y);

   // HeightOfTable += 25 ; // This is space for the primary key

    HeightOfTable += HEIGHT_TITLE; //For the title area

    wxSize rectSize( width,HeightOfTable);

    //Draw the background rect
    dc.SetPen( wxPen( wxColor(150,150,150), 2 ) );
    wxBrush brush(wxColor(150,150,150),wxBRUSHSTYLE_SOLID);
    dc.SetBrush(brush); // green filling
    wxPoint ptStart(XStartPos+5,YStartPos+5);
    wxSize szTitle(width-2,HeightOfTable+25);

    // save the rect for the hit test.
    m_TableRect.x = ptStart.x-6;
    m_TableRect.y = ptStart.y-6;
    m_TableRect.width = szTitle.x+6;
    m_TableRect.height = szTitle.y+6;

    dc.DrawRectangle(ptStart,szTitle);

    wxBrush brush2(wxColor(230,230,230),wxBRUSHSTYLE_SOLID);
    dc.SetBrush(brush2); // green filling

    //Draw the title rect
    wxPoint ptStart2(XStartPos,YStartPos);
    wxSize szTitle2(width,HEIGHT_TITLE);
    dc.DrawRectangle(ptStart2,szTitle2);
    dc.SetPen( wxPen( wxColor(0,0,0), 1 ) ); // black line, 3 pixels thick
    dc.DrawText(m_sTableName,XStartPos+GAP,YStartPos+GAP);


    //FOR DEBUGGING
 //   wxRect obRect = GetObjectRect();
 //   dc.DrawText("XPos="+Utility::IntToString(obRect.x),XStartPos+GAP,YStartPos-30);


    dc.SetPen( wxPen( wxColor(0,0,0), 2 ) );

    wxBrush brush3(wxColor(100,200,220),wxBRUSHSTYLE_SOLID);
    //Draw main rect
    dc.SetBrush(brush3); // green filling

    //Draw the main rectangle
    dc.DrawRectangle(pt,rectSize);

    //Draw horzontal line between fieldName and Type
    dc.SetPen( wxPen( wxColor(150,150,170), 1 ) );
    dc.DrawLine(XStartPos + extentFieldName.x+2,pt.y+1,XStartPos + extentFieldName.x+2, pt.y-2 + HeightOfTable);

    //Drw the Primary Key
    dc.SetFont(ftItalicBold);
    dc.SetPen( wxPen( wxColor(0,0,0), 2 ) );
    pt.x = XStartPos +GAP ;
    wxPoint drawPt;
    drawPt = pt;

    //Add the primary key to the field
    pFR = new FieldRect();
    pFR->m_Rect.x = drawPt.x;
    pFR->m_Rect.y = drawPt.y;
    pFR->m_Rect.width=szTitle2.x-20;
    pFR->m_Rect.height=15;
    pFR->m_sFieldName=m_sTableName+"Id";
    m_FieldRectList.Add(pFR);

    drawPt.y +=2;
    dc.DrawText(m_sTableName+"Id",drawPt);
    dc.SetFont(ftItalic);

    drawPt.x += (extentFieldName.x + extentFieldType.x) -10;
    drawPt.y += 2;
    dc.DrawText("*",drawPt);
    drawPt.y -= 2;
    pFR = new FieldRect();
    pFR->m_Rect.x = drawPt.x;
    pFR->m_Rect.y = drawPt.y;
    pFR->m_Rect.width=20;
    pFR->m_Rect.height=20;
    pFR->m_sFieldName="*";
    m_FieldRectList.Add(pFR);

    pt.x += extentFieldName.x;

    drawPt = ptType;
    drawPt.y +=2;
    dc.DrawText("int auto",drawPt);
    pt.y +=extentFieldName.y;
    ptType.y = pt.y;

    dc.SetPen( wxPen( wxColor(150,150,170), 1 ) );
    dc.DrawLine(XStartPos +GAP,pt.y,XStartPos+width-GAP,pt.y);

    int count = m_TableFieldList.GetCount();

    for (int index=0; index<count; index++){

        dc.SetPen( wxPen( wxColor(0,0,0), 2 ) );
        TableField fieldItem;
        fieldItem = m_TableFieldList[index];
        pt.x = XStartPos +GAP ;

        drawPt = pt;
        //Create the dragging field
        pFR = new FieldRect();
        pFR->m_Rect.x = drawPt.x;
        pFR->m_Rect.y = drawPt.y;
        pFR->m_Rect.width=szTitle2.x;
        pFR->m_Rect.height=15;
        pFR->m_sFieldName=fieldItem.fieldName;
        m_FieldRectList.Add(pFR);

        drawPt.y +=2;
        dc.DrawText(fieldItem.fieldName,drawPt);




        pt.x += extentFieldName.x;

        drawPt = ptType;
        drawPt.y +=2;
        dc.DrawText(fieldItem.fieldType,drawPt);




        pt.y +=extentFieldName.y;
        ptType.y = pt.y;

        if(index<(count-1))
        {
            dc.SetPen( wxPen( wxColor(150,150,170), 1 ) );
            dc.DrawLine(XStartPos +GAP,pt.y,XStartPos+width-GAP,pt.y);
        }




    }
}

//Returns the field name if point is in a field rect
wxString ObTable::HitTestField(wxPoint mousePoint, wxRect& sfieldRect)
{
    int count = m_FieldRectList.GetCount();

    for (int index=0; index<count; index++) {
        sfieldRect = m_FieldRectList[index].m_Rect;
        if(Utility::IsPointInRect(mousePoint,m_FieldRectList[index].m_Rect))
            return m_FieldRectList[index].m_sFieldName;
    }

    return "";
}
