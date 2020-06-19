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



wxBEGIN_EVENT_TABLE(MainRunPage, wxFrame)
                EVT_MYEVENT(MainRunPage::OnMyEvent)
wxEND_EVENT_TABLE()

//#include <wx/arrimpl.cpp>
//WX_DEFINE_OBJARRAY(ArrayRunForms);

MainRunPage::MainRunPage( wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
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

                pRunForm = new RunForm((wxWindow *) this, -1, "Run Form", wxDefaultPosition, wxDefaultSize,style);
                pRunForm->SetFormID(usr_formsId); // The formId is used to load the form definition from the database.
                pRunForm->Create(pRunForm->GetQuery(usr_formsId));//We need to get the query for this form in order to run it.


                pRunForm->SetPosition(wxPoint(Utility::StringToInt(xPos),Utility::StringToInt(yPos)));
                pRunForm->SetSize(wxSize(Utility::StringToInt(width),Utility::StringToInt(height)));
                pRunForm->Show(true);

            }
            Layout();
        }
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


void MainRunPage::OnMyEvent(MyEvent& event) {

    if (event.m_bButtonClicked) {


    }
}

