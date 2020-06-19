//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Shared/global.h"
#include "../Shared/Utility.h"

#include "RunForm.h"

#include "MainRunPage.h"

MainRunPage::MainRunPage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_sPageId="";

    //Step 1: We need to get the usr_pagesId with the pageName field 'MainPage', this is the starting point.
    wxArrayString sArray;
    Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase,sArray,"usr_pages","PageName","MainPage");
    if(sArray.GetCount()==1){
        wxString pageId = sArray[0];


        //Step 2: With the MainPage index, we need to load all forms from usr_forms having the usr_pageId of the MainPage.
        wxString QueryString = "SELECT * ";
        QueryString += " FROM usr_forms ";
        QueryString += " WHERE usr_pagesId = " + pageId;

        ArrayFieldRecord saControl;
        Utility::LoadArrayFieldRecordFromQuery(saControl, QueryString);

        int count = saControl.GetCount();
        if(saControl.GetCount()>0){
            for(int index=0;index<count;index++){


                wxString usr_formsId = saControl[index].GetData("usr_formsId");
                wxString formName = saControl[index].GetData("formName");
                wxString xPos = saControl[index].GetData("xPosition");
                wxString yPos = saControl[index].GetData("yPosition");
                wxString width = saControl[index].GetData("width");
                wxString height = saControl[index].GetData("height");
                
                RunForm *pRunForm = nullptr;

                long style = wxBORDER_SIMPLE;

                pRunForm = new RunForm((wxFrame *) this, -1, "Run Form", wxPoint(Utility::StringToInt(xPos),Utility::StringToInt(yPos)), wxSize(Utility::StringToInt(width),Utility::StringToInt(height)),style);

                pRunForm->SetFormID(usr_formsId); // The formId is used to load the form definition from the database.
                pRunForm->Create(pRunForm->GetQuery(usr_formsId));//We need to get the query for this form in order to run it.
                pRunForm->Show(true);

            }
        }


        //
        m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

        //wxBoxSizer *gSizer1 = new wxBoxSizer( wxHORIZONTAL );

        //wxStaticText *txt = new wxStaticText( this, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize, 0 );

        //gSizer1->Add( txt, 0, wxEXPAND, 0);

        //m_MainFormSizer->Add( gSizer1, 0, wxCenter, 5 );
        //Create a HTML display for the help file
    }

}

MainRunPage::~MainRunPage(){

}

void MainRunPage::SetPageID(wxString sPageId)
{
    m_sPageId=sPageId;
}

//We can send a message to the parent that this window is destroyed.
bool MainRunPage::Destroy()
{
    bool bResult = wxFrame::Destroy();

    //If we are running the engine, there is no parent for this frame.
    if(!Settings.RUN_ENGINE_ENABLED){
        MyEvent my_event( this );
        my_event.m_bMainRunPageWasDestroyed=true;
        GetParent()->ProcessWindowEvent( my_event );
    }

    return bResult;
}