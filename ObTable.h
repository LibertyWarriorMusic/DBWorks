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

    ArrayTableField m_TableFieldItem; //Store the list of fields for this table
    wxPoint m_ObPosition; // Stores the position on the screen the object will be drawn

    bool m_bShowTable;

    bool m_bSnapToGrid; // Will snap each object to a grid value; DEFAULT true
    bool m_SnapToGridRememberState; // Used if we want.
    wxSize m_gridDistance; //The distance between grid lines.  DEFAULT 1

    wxRect m_TableRect; // This store this objects rect screen position


   public:
    ObTable();
    ~ObTable();
    void SaveDB(); //Save this table to the database;
    wxString GetTableName(); //Return the name of this table.

    void TurnSnapOff();
    void TurnSnapOn();
    void RestorePreviousSnapCondition();

    wxRect GetObjectRect();


    bool HitTest(wxPoint pt); //Returns true if point is in this objects rect.

    void SetTableFieldList(const ArrayTableField &fieldItemList); //Saves all the table fields for this object.
    void SetTableName(wxString sTableName);//Saves the table name of this object.
    void SetObjectPosition(wxPoint pt); //Save the table position of this object.
    void DrawObject(wxDC&  dc);

};

#endif //DBWORKS_OBTABLE_H
