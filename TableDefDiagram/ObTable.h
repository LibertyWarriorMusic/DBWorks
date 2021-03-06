//
// Created by Nicholas Zounis on 30/3/20.
//

#ifndef DBWORKS_OBTABLE_H
#define DBWORKS_OBTABLE_H

//TABLE DATA FOR PERSISTANCE stored in table_data in the sys_tables
// <key:ObTablePositionX>10</key>
// <key:ObTablePositionY>50</key>
// <key:ObTableShow>yes</key>



class ObTable {

private:
    wxString m_sTableName; //The table name this object is based on
    wxString m_sTableId;

    ArrayFieldRect m_FieldRectList; //This list is generated  at drawing and used for the field hit test.
    ArrayTableField m_TableFieldList; //Store the list of fields for this table

    bool m_bShowTable;

    bool m_bSnapToGrid; // Will snap each object to a grid value; DEFAULT true
    bool m_SnapToGridRememberState; // Used if we want.
    wxSize m_gridDistance; //The distance between grid lines.  DEFAULT 1
    wxRect m_TableRect; // This store this objects rect screen position

   public:
    ObTable();
    ~ObTable();

    //This is used to store a list of the linkages to assist and speed up drawing so we don't have to search for them, they will be in this list.
    wxArrayString m_aListOfLinkageFlags;

    int GetFieldIndexByFieldName(wxString sFieldName);

    void SaveDB(); //Save this table to the database;
    wxString GetTableName(); //Return the name of this table.
    wxString GetTableID(); //Return the name of this table.
    void TurnSnapOff();
    void TurnSnapOn();
    void RestorePreviousSnapCondition();

    wxString GetFieldTypeByFieldName(wxString sFieldName);

    wxRect GetObjectRect();

    bool HitTest(wxPoint pt); //Returns true if point is in this objects rect.
    wxString HitTestField(wxPoint mousePoint, wxRect& sfieldRect); //Returns the fieldname if point is in a field rect

    void SetTableFieldList(const ArrayTableField &fieldItemList); //Saves all the table fields for this object.
    void SetTableName(wxString sTableName);//Saves the table name of this object.
    void SetTableID(wxString sTableId);
    void SetObjectPosition(wxPoint pt); //Save the table position of this object.
    void DrawObject(wxDC&  dc);

};

#endif //DBWORKS_OBTABLE_H
