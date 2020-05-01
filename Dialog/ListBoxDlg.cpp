//
// Created by Nicholas Zounis on 23/3/20.
//

#define BORDERWIDTH 3
#define TEXTWIDTH 140
#define ALLOW_TO_GROW 1

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "../Shared/global.h"

#include "ListBoxDlg.h"

enum {
    ID_BUTTON_CLICKED= wxID_HIGHEST + 1600,
    ID_BUTTON_DELETE
};


wxBEGIN_EVENT_TABLE(ListBoxDlg, DialogBaseClass)
                EVT_BUTTON(ID_BUTTON_CLICKED, ListBoxDlg::OnClickButton)
                EVT_BUTTON(ID_BUTTON_DELETE, ListBoxDlg::OnClickDeleteButton)
wxEND_EVENT_TABLE()

ListBoxDlg::ListBoxDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ), wxDEFAULT_FRAME_STYLE | wxID_OK | wxSTAY_ON_TOP)
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);
    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    //AddCtlItem(CTL_LISTBOX, sFieldTitle, "", "ID_OPTIONS");
    AddCtlItem(CTL_TEXT, "Enter selection item.", "", "ID_TEXT");
    AddCtlItem(CTL_BUTTON, "Add", "Add selection item to list.", "ID_ADD",ID_BUTTON_CLICKED);
    AddCtlItem(CTL_BUTTON, "Delete", "Delete selected item.", "ID_DELETE",ID_BUTTON_DELETE);

    AddCtlItem(CTL_LISTBOX, "Items", "", "ID_OPTIONS");

    pListBox = (wxListBox*)GetControl("ID_OPTIONS");

    RenderAllControls();
    SetSize(wxSize(600,200));
}

void ListBoxDlg::SetControlID(wxString id)
{
    m_sControlID=id;
}

void ListBoxDlg::DeleteItems()
{
    for(int index=0;index<asItemsToDelete.GetCount();index++)
            Utility::ExecuteQuery(asItemsToDelete[index]);
}

void ListBoxDlg::OnClickDeleteButton( wxCommandEvent& event )
{
    wxListBox * pListBox = (wxListBox*) GetControl("ID_OPTIONS");
    int sel = pListBox->GetSelection();
    if(sel!=wxNOT_FOUND){

        ComboDataItem* comboItem = (ComboDataItem*) pListBox->GetClientObject(sel);

        //Only add to list if it exists in the table
        if(!comboItem->tableID.IsEmpty()){
            wxString sQuery = "DELETE FROM usr_control_items WHERE usr_control_itemsId="+comboItem->tableID;
            asItemsToDelete.Add(sQuery);
        }

        pListBox->Delete(sel);
    }else
        wxLogMessage("Select an item from the list to delete.");
}


void ListBoxDlg::OnClickButton( wxCommandEvent& event )
{
    wxListBox * pListBox = (wxListBox*) GetControl("ID_OPTIONS");
    wxTextCtrl * pTextCtrl = (wxTextCtrl*) GetControl("ID_TEXT");


    wxString text = pTextCtrl->GetValue();

    if (!text.IsEmpty()){
        ComboDataItem* comboItem = new ComboDataItem;

        //A new item has a zero control ID.
        comboItem->tableID = "";
        comboItem->linkedTableID = m_sControlID;

        //Note, the title and data is the same thing
        comboItem->title=text;
        comboItem->data=text;
        pListBox->Append(comboItem->title,(ComboDataItem*)comboItem);
    } else
        wxLogMessage("Enter an item then press add.");


}

void ListBoxDlg::LoadItems()
{
    ArrayFieldRecord aRecord;
    wxString sQuery = "SELECT usr_control_itemsId, itemData, usr_controlsId FROM usr_control_items WHERE itemType='SELECTION_ITEM' AND usr_controlsId="+m_sControlID;
    Utility::LoadArrayFieldRecordFromQuery(aRecord,sQuery);

    for (int index=0; index<aRecord.GetCount();index++){
        wxString sData = aRecord[index].GetData("itemData");
        wxString sControlID = aRecord[index].GetData("usr_controlsId");
        wxString sUsrControlItemsId = aRecord[index].GetData("usr_control_itemsId");

        ComboDataItem* comboItem = new ComboDataItem;
        comboItem->tableID = sUsrControlItemsId;
        comboItem->linkedTableID = sControlID;
        comboItem->data=sData;

        //We also want to save the controlID
        pListBox->Append(comboItem->data,(ComboDataItem*)comboItem);
    }
}

void ListBoxDlg::SaveItems()
{
    //First delete any items removed.
    DeleteItems();

    //Go through the list box list.
    for (int index=0; index<pListBox->GetCount();index++){


        ComboDataItem* comboItem = (ComboDataItem*) pListBox->GetClientObject(index);

        //This is the control ID.
        if(comboItem != nullptr){
            wxString sUsrControlItemsId = comboItem->tableID;
            wxString sControlId= comboItem->linkedTableID;
            wxString sItemData = comboItem->data;

            if(sUsrControlItemsId.IsEmpty()){
                //Insert Items
                wxString sQuery="INSERT INTO usr_control_items (usr_controlsId, itemName,itemData) VALUES ("+sControlId+",'"+sItemData+"','"+sItemData+"') ";

                Utility::ExecuteQuery(sQuery);
                comboItem->tableID = Utility::GetHighestId("usr_control_items");
            } else
            {
                //Update Item
                wxString sQuery="UPDATE usr_control_items set itemName='"+sItemData+"', itemData='"+sItemData+"', usr_controlsId="+m_sControlID +" WHERE usr_control_ItemsId="+sUsrControlItemsId;
                Utility::ExecuteQuery(sQuery);
            }
        }
    }

    //If we have a controlID of zero( empty String) , insert Item.
    //If we have a control ID, update item.

}