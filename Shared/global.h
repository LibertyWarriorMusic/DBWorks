class wxDatePickerCtrl;
class wxHyperlinkCtrl;

#define FLAG_OPTIONS "SELECTION{MULTILINE;READONLY;HIDE;PASSWORD;WEBLINK;DOCUMENT;SELECTION_ADDITIVE;SELECTION{val1;val2;etc;};SELECTION_LOOKUP_NAME{TableName;FieldName;};SELECTION_LINKED_NAME{TableName;FieldName;};SELECTION_LOOKUP_ID{TableID;ColumnNumber;};SELECTION_LINKED_ID{TableID;ColumnNumber;}}" //The different type of flags you can create with this database.
#define MYSQL_TYPE_OPTIONS "SELECTION{VARCHAR(255)     [String(0-255 Variable String)];CHAR(255)            [String(0-255) Padded String];TINYTEXT              [String(0-255)];TEXT                      [String(0-65535)];MEDIUMTEXT        [String(0-16777215)];LONGTEXT             [String(0-4294967295)];BINARY                  [0-255];TINYBLOB              [0-255];BLOB                     [0-65535];MEDIUMBLOB         [0-16777215];LONGBLOB          [0-4294967295];TINYINT           [-128 to 127];SMALLINT     [-32768 to 32767];MEDIUMINT     [-8388608 to 8388607];INT                  [-2147483648 to 2147483647];BIGINT            [-9223372036854775808 to 9223372036854775807];FLOAT(p)         [Where p is the precision];DOUBLE(m,d)     [m:total digits d:digits after the decimal];DECIMAL;DATE;TIME;DATETIME;TIMESTAMP;YEAR;ENUM;BOOLEAN;}"

#define SYS_TABLES "sys_tables" //This is the name of the table in the information database that holds all user tables.
#define SYS_FIELDS "sys_fields"//This is the name of the table in the information database that holds all user fields linked to sys_table.
#define USR_QUERIES "usr_queries"
#define USR_FILTERS "usr_filters"
#define SYS_DOCS "sys_docs" //This is the name of the table in the information database that holds system documents.


 // This is all the mysql reserved words.
//Place all global settings here, can be use anywhere in the project.
//Thse setting are only set from settings.ini file.
struct  GlobalSettings {
    wxString sMSQLReservedWords="";
    wxString sMessage="";
    wxString sDatabase="information";
    wxString sDatabaseUser="root";
    wxString sPassword="password";
    wxString sServer="127.0.0.1:3306";
    wxString sUsergroup="SYSTEM_DATABASE_DEVELOPER";
    wxString sDClickGridCell="EDIT";

    long lMainWindowWidth=1400;
    long lMainWindowHeight=700;
    long lMaxGridColWidth=500;
    long lMinGridColWidth=100;
    int iSysTablesDocID=4;
    int iSysFieldsDocID=9;
    int iSysDocsDocID=10;
    bool IMPORT_FIELDS_HAVING_PRIMARY_KEYS=true;
    bool bImportCreateTables=false;
    bool bImportData=false;
    bool bAutoCheckDefinitions=false;
    bool bShowGridColumnFields=false;
    bool ENFORCE_SELECTION_LINKED_TO_PRIMARY_KEY=true;
    bool SHOW_USERGROUP_COMBO_ON_TOOLBAR=false;
    bool RUN_ENGINE_ENABLED=false;

    wxString sDatbaseSelectionList="SELECTION{information;test;final_release;}";
};
extern GlobalSettings Settings;


// This is where all our messages to users will be placed. It allows easy editing continuity testing.
#define MSG_FIELD_NOT_CREATED "The table has not been created or needs updating. Right click and select 'Field Definitions', you can create the table by clicking the create table toolbar button."
#define MSG_DATABASE_DOES_NOT_EXIST "The database doesn't exist, you need to create it first."
#define MSG_DATABASE_CONNECTION_FAILURE "There was a problem connecting to the database, make sure you have MySQL running."
#define MSG_DATABASE_FAIL_ITEM_LIST "The database connected, but failed to get an item list."
#define MSG_DATABASE_FAIL_ROW "The database connected, but the row you are trying to retrieve doesn't exist."
#define MSG_DELETE_TABLE_PROMPT "Are you sure you want to delete this table?"
#define MSG_UNABLE_TO_LOAD_SETTINGS "The settings.ini file was not found, loading default settings. User=root Password=password"
#define MSG_ALTER_TABLE "Are you sure you want to alter this table with:?"
#define MSG_MYSQL_RESERVED_WORD "Table not altered, the following field is a MySQL reserved word, you need to change the field name:"
#define MSG_ONLY_DATABASE_DEVELOPERS "Only database developers are allowed access to this feature."
#define MSG_NEED_TO_SELECT_ROW "You need to select a single row first."
#define MSG_FAILED_TO_CREATE_DATABASE "Failed to create the database, check that you have MySQL running."
#define MSG_FAILED_QUERY_FILTER "The query or filter you created failed to run, their is an error with your statement"


#define TITLE_WIDTH 100
#define BUTTON_WIDTH 100
#define TEXTCTL_WIDTH 500
#define CTRL_HEIGHT 20
#define TEXTCTLMULTI_HEIGHT 60
#define BORDER_WIDTH 2
#define ALLOW_TO_GROW 1 // 1, the control will grow with the sizer
#define DONT_ALLOW_TO_GROW 0
#define WINDOW_WIDTH 600
#define RELATIONSHIP_DIAGRAM_FIELD_HEIGHT 12
#define RELATIONSHIP_DIAGRAM_TITLE_HEIGHT 12
#define RELATIONSHIP_DIAGRAM_GAP 3
#define HEIGHT_QUERY_BUILDER 190
//#define WIDTH_QUERY_BUILDER 300

//These lo
enum {
    db_link_left = wxID_HIGHEST + 800, // m_bStartObLinkageSide  m_pEndTableObject
    db_link_right, // m_bStartObLinkageSide  m_pEndTableObject
    db_link_lookup_name, //m_LinkageType
    db_link_linked_name, //m_LinkageType
    db_link_lookup_id, //m_LinkageType
    db_link_linked_id //m_LinkageType
};

class ObTable;
class ObControl;
class ObForm;
//class RunForm;

//Object Array Declarations.
class TableLinkageLine
{
public:

    int m_bStartObLinkageSide; //Either link from the left side of the table or the right side
    int m_bEndObLinkageSide;

    int m_iStartObYOffset;//Where we start to draw the line depends on the objects position on the screen Y offset down to the field name to link
    int m_iEndObYOffset;

    ObTable* m_pStartTableObject; //The start and end objects to link
    ObTable* m_pEndTableObject;

    //These will be filled up depending on the type of linkage
    int m_LinkageType;
    wxString m_sStartObLinkedFieldName;
    wxString m_sStartObLinkedFieldColNumber;
    wxString m_sEndObLinkedFieldName;
    wxString m_sEndObLinkedFieldColNumber;

    wxString m_sStartObjectTableName;
    wxString m_sEndObjectTableName;

    wxString m_sStartObjectTableId;
    wxString m_sEndObjectTableId;

    TableLinkageLine(){
        m_LinkageType = db_link_lookup_name;

        //These will be in toggle configuration, if Ob1 is left Ob2 will be right and visa versa.
        m_bStartObLinkageSide=db_link_right;
        m_bEndObLinkageSide=db_link_left;

        m_iStartObYOffset=0;
        m_iEndObYOffset=0;

        m_pStartTableObject= nullptr;
        m_pEndTableObject= nullptr;

        //Only one of these will be set depending on m_LinkageType
        m_sStartObjectTableName="";
        m_sStartObLinkedFieldName=""; //Set for db_link_lookup_name  and db_link_linked_name

        m_sStartObjectTableId="";
        m_sStartObLinkedFieldColNumber=""; //Set for db_link_lookup_id db_link_linked_id


        m_sEndObjectTableName="";
        m_sEndObLinkedFieldName=""; //Set for db_link_lookup_name  and db_link_linked_name

        m_sEndObjectTableId="";
        m_sEndObLinkedFieldColNumber=""; //Set for db_link_lookup_id db_link_linked_id

    };
};


//Used to store the field definitions
class TableField
{
public:
    wxString fieldName;
    wxString fieldType;
    wxString fieldKey;
    wxString fieldExtra;
    wxString fieldNull;
    wxString fieldDefault;
    wxString Flags;
    wxString Title;

    TableField& operator=(const TableField& field) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        fieldName = field.fieldName;
        fieldType = field.fieldType;
        fieldKey = field.fieldKey;
        fieldExtra = field.fieldExtra;
        fieldNull = field.fieldNull;
        fieldDefault = field.fieldDefault;
        Flags = field.Flags;
        Title = field.Title;


        return *this; // return the result by reference
    }

    TableField& operator=(const wxString& str) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        fieldName = str;
        fieldType = str;
        fieldKey = str;
        fieldExtra = str;
        fieldNull = str;
        fieldDefault = str;
        Flags = str;
        Title = str;

        return *this; // return the result by reference
    }
};


enum {
    CTL_NULL = wxID_HIGHEST + 1700 ,
    CTL_SPACER,
    CTL_STATIC,
    CTL_TEXT,
    CTL_PASSWORD,
    CTL_MULTI_TEXT,
    CTL_SELECTION_ADDITIVE,
    CTL_SELECTION,
    CTL_CHECKBOX,
    CTL_DATE,
    CTL_WEBLINK,
    CTL_BUTTON,
    CTL_SELECTION_LOOKUP_NAME,
    CTL_SELECTION_LINKED_NAME,
    CTL_SELECTION_LOOKUP_ID,
    CTL_SELECTION_LINKED_ID,
    CTL_RECORD_SELECTOR,
    CTL_LISTBOX,
    CTL_MYTEXTCTRL,
    CTL_TIME
};
class ListBoxManager;
class MyTextCtrl;
class myButton;
class RecordSelector;

//This class is used to generate fields for either database forms or dialog based forms
class FieldCtlItem  : public TableField
{
public:
    //Pointers to controls used in our forms. One of these will be active at any one time this this class represents only one single field of information.

    //Add more control pointers here when you need to use them in the forms.
    wxStaticText *TitleCtl;
    wxStaticText *SpacerCtl;
    wxTextCtrl *textCtl;
    wxComboBox *comCtl;
    wxHyperlinkCtrl *linkCtl;
    wxDatePickerCtrl *datePickerCtl;
    wxCheckBox *pCheckBox;
    wxButton *pButton;
    myButton *pmyButton;
    wxListBox *pListBox;
    wxBoxSizer *pBoxSizer;
    MyTextCtrl *pMyTextCtrl;
    RecordSelector *pRecordSelector;

    int iTypeOfControl; //Used to identify the type of control we have, combo - checkbox. Identified by the enum above
    bool bFound; // A temp field used for searching items.

    wxBoxSizer *pSizer; // The controls will be stored in a box sizer
    wxString m_sIdentifier; // Used to identify this control in the list when we retrieve or set data
    wxString m_sData;//Used to store generic data for this field item.
    wxString m_sDescription; // A description associated with this control

    //Used in the run form
    wxString m_sTypeName;
    wxString m_sFormId;
    wxString m_sTypeId;
    wxString m_sControlId;
    wxString m_sLabel;
    //wxString m_sField;

    wxRect m_rect; // Used to determine the order in which to layout the controls or the exact position of the control

    ~FieldCtlItem()
    = default;

    FieldCtlItem(){
        fieldName="";
        fieldNull="";
        fieldExtra="";
        fieldKey="";
        fieldType="";
        fieldDefault="";
        Title="";
        Flags="";
        m_sData="";
        m_sDescription="";
        m_sIdentifier="";

        m_sTypeName="";
        m_sFormId="";
        m_sTypeId="";
        m_sControlId="";
        m_sLabel="";
        //m_sField=""; USE fieldName

        iTypeOfControl=0;
        bFound=false;
        m_rect.x=0;
        m_rect.y=0;
        m_rect.width=0;
        m_rect.height=0;

        TitleCtl= nullptr;
        SpacerCtl= nullptr;
        textCtl=nullptr;
        comCtl=nullptr;
        datePickerCtl=nullptr;
        linkCtl=nullptr;
        pSizer= nullptr;
        pCheckBox= nullptr;
        pButton = nullptr;
        pmyButton = nullptr;
        pListBox = nullptr;
        pMyTextCtrl = nullptr;
        pRecordSelector = nullptr;
        pBoxSizer = nullptr;
        iTypeOfControl=CTL_NULL; //Set the control type as a simple text as default
    };
    void HideCtrl(){

        if(TitleCtl != nullptr){
            TitleCtl->Hide();
            TitleCtl = nullptr;
        }


        if(SpacerCtl != nullptr){
            SpacerCtl->Hide();
            SpacerCtl = nullptr;
        }
        else if( textCtl !=nullptr){
            textCtl->Hide();
            textCtl = nullptr;
        }
        else if( comCtl !=nullptr){
            comCtl->Hide();
            comCtl = nullptr;
        }

            //else if( datePickerCtl != nullptr)
            //datePickerCtl->Destroy();
            // else if( linkCtl != nullptr)
            //linkCtl->Close(true);
        else if( pCheckBox !=  nullptr){
            pCheckBox->Hide();
            pCheckBox = nullptr;
        }
        else if( pButton  !=  nullptr){
            pButton->Hide();
            pButton = nullptr;
        }

            // else if( pmyButton  !=  nullptr)
            //pmyButton->Close(true);
        else if( pListBox  !=  nullptr){
            pListBox->Hide();
            pListBox = nullptr;
        }

        //else if( pMyTextCtrl  !=  nullptr)
        //pMyTextCtrl->CloseCtrl();
        // else if( pRecordSelector  != nullptr)
        //pRecordSelector->Close(true);


    };
    void DestroyCtrl(){

        if(TitleCtl != nullptr){
            TitleCtl->Destroy();
            TitleCtl = nullptr;
        }


        if(SpacerCtl != nullptr){
            SpacerCtl->Destroy();
            SpacerCtl = nullptr;
        }
        else if( textCtl !=nullptr){
            textCtl->Destroy();
            textCtl = nullptr;
        }
        else if( comCtl !=nullptr){
            comCtl->Destroy();
            comCtl = nullptr;
        }

        //else if( datePickerCtl != nullptr)
            //datePickerCtl->Destroy();
       // else if( linkCtl != nullptr)
            //linkCtl->Close(true);
        else if( pCheckBox !=  nullptr){
            pCheckBox->Destroy();
            pCheckBox = nullptr;
        }
        else if( pButton  !=  nullptr){
            pButton->Destroy();
            pButton = nullptr;
        }

       // else if( pmyButton  !=  nullptr)
            //pmyButton->Close(true);
        else if( pListBox  !=  nullptr){
            pListBox->Destroy();
            pListBox = nullptr;
        }

        //else if( pMyTextCtrl  !=  nullptr)
            //pMyTextCtrl->CloseCtrl();
       // else if( pRecordSelector  != nullptr)
            //pRecordSelector->Close(true);


    };

    //We need to set the type identifier when we set the type names.
    void SetTypeName(wxString sTypeName){
        m_sTypeName=sTypeName;
        if(m_sTypeName=="CTL_NULL")
            iTypeOfControl = CTL_NULL;
        else if(m_sTypeName=="CTL_SPACER")
            iTypeOfControl = CTL_SPACER;
        else if(m_sTypeName=="CTL_STATIC")
            iTypeOfControl = CTL_STATIC;
        else if(m_sTypeName=="CTL_TEXT")
            iTypeOfControl = CTL_TEXT;
        else if(m_sTypeName=="CTL_PASSWORD")
            iTypeOfControl = CTL_PASSWORD;
        else if(m_sTypeName=="CTL_MULTI_TEXT")
            iTypeOfControl = CTL_MULTI_TEXT;
        else if(m_sTypeName=="CTL_SELECTION_ADDITIVE")
            iTypeOfControl = CTL_SELECTION_ADDITIVE;
        else if(m_sTypeName=="CTL_SELECTION")
            iTypeOfControl = CTL_SELECTION;
        else if(m_sTypeName=="CTL_CHECKBOX")
            iTypeOfControl = CTL_CHECKBOX;
        else if(m_sTypeName=="CTL_DATE")
            iTypeOfControl = CTL_DATE;
        else if(m_sTypeName=="CTL_WEBLINK")
            iTypeOfControl = CTL_WEBLINK;
        else if(m_sTypeName=="CTL_BUTTON")
            iTypeOfControl = CTL_BUTTON;
        else if(m_sTypeName=="CTL_SELECTION_LOOKUP_NAME")
            iTypeOfControl = CTL_SELECTION_LOOKUP_NAME;
        else if(m_sTypeName=="CTL_SELECTION_LINKED_NAME")
            iTypeOfControl = CTL_SELECTION_LINKED_NAME;
        else if(m_sTypeName=="CTL_SELECTION_LOOKUP_ID")
            iTypeOfControl = CTL_SELECTION_LOOKUP_ID;
        else if(m_sTypeName=="CTL_SELECTION_LINKED_ID")
            iTypeOfControl = CTL_SELECTION_LINKED_ID;
        else if(m_sTypeName=="CTL_RECORD_SELECTOR")
            iTypeOfControl = CTL_RECORD_SELECTOR;
        else if(m_sTypeName=="CTL_LISTBOX")
            iTypeOfControl = CTL_LISTBOX;
        else if(m_sTypeName=="CTL_MYTEXTCTRL")
            iTypeOfControl = CTL_MYTEXTCTRL;
        else if(m_sTypeName=="CTL_TIME")
            iTypeOfControl = CTL_TIME;
    };
};

//This class is used to drag a field name in the relationship diagram
class FieldRect
{
public:
    wxRect m_Rect;
    wxString m_sFieldName;
    FieldRect(){
        m_Rect.x=0;
        m_Rect.y=0;
        m_Rect.width=0;
        m_Rect.height=0;
        m_sFieldName="";
    };

    //Draw this object to the screen while we drag it.
    void Draw(wxDC&  dc){
        wxFontInfo info;
        info.Italic(true);
        info.Family(wxFONTFAMILY_ROMAN );
        wxFont ftItalic(info);
        dc.SetFont(ftItalic);
        dc.SetPen( wxPen( wxColor(150,150,150), 1 ) );
        wxBrush brush(wxColor(150,150,150),wxBRUSHSTYLE_SOLID);
        dc.SetBrush(brush); // green filling
        dc.DrawText(m_sFieldName,m_Rect.x,m_Rect.y);
    };
    void SetRect(int x, int y){
        m_Rect.x=x;
        m_Rect.y=y;
    };
};




WX_DECLARE_OBJARRAY(FieldCtlItem, ArrayFieldItem); //Used in GenericItemForm class
WX_DECLARE_OBJARRAY(TableField, ArrayTableField); //Used in GenericTable class
WX_DECLARE_OBJARRAY(TableField, ArrayGridItem); //Used in DBGrid class
WX_DECLARE_OBJARRAY(FieldCtlItem,ArrayFieldDataCtls); // Used in DialogbaseClass
WX_DECLARE_OBJARRAY(TableLinkageLine, ArrayTableLinkageLine); //Used in GenericItemForm class
WX_DECLARE_OBJARRAY(ObTable, ArrayDrawObjects); //Used in ObTablePanel class
WX_DECLARE_OBJARRAY(TableField, ArrayQueryField); //Used in Generic Query Grid
WX_DECLARE_OBJARRAY(FieldRect, ArrayFieldRect); //Used in Generic Query Grid
WX_DECLARE_OBJARRAY(ObControl, ArrayDrawControls); //Used in DesignFormPanel
WX_DECLARE_OBJARRAY(ObForm, ArrayDrawForms); //Used in DesignPagePanel