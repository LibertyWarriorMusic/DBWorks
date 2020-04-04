class wxDatePickerCtrl;
class wxHyperlinkCtrl;

#define FLAG_OPTIONS "SELECTION{MULTILINE;READONLY;HIDE;PASSWORD;WEBLINK;DOCUMENT;SELECTION_ADDITIVE;SELECTION{val1;val2;etc;};SELECTION_LOOKUP_ID{TableID;ColumnNumber;};SELECTION_LOOKUP_NAME{TableName;FieldName;};SELECTION_LINKED_ID{TableID;ColumnNumber;};SELECTION_LINKED_NAME{TableName;FieldName;}}" //The different type of flags you can create with this database.
#define MYSQL_TYPE_OPTIONS "SELECTION{VARCHAR(255)     [String(0-255 Variable String)];CHAR(255)            [String(0-255) Padded String];TINYTEXT              [String(0-255)];TEXT                      [String(0-65535)];MEDIUMTEXT        [String(0-16777215)];LONGTEXT             [String(0-4294967295)];BINARY                  [0-255];TINYBLOB              [0-255];BLOB                     [0-65535];MEDIUMBLOB         [0-16777215];LONGBLOB          [0-4294967295];TINYINT           [-128 to 127];SMALLINT     [-32768 to 32767];MEDIUMINT     [-8388608 to 8388607];INT                  [-2147483648 to 2147483647];BIGINT            [-9223372036854775808 to 9223372036854775807];FLOAT(p)         [Where p is the precision];DOUBLE(m,d)     [m:total digits d:digits after the decimal];DECIMAL;DATE;TIME;DATETIME;TIMESTAMP;YEAR;ENUM;BOOLEAN;}"
#define SYS_TABLES "sys_tables" //This is the name of the table in the information database that holds all user tables.
#define SYS_FIELDS "sys_fields"//This is the name of the table in the information database that holds all user fields linked to sys_table.
#define SYS_DOCS "sys_docs" //This is the name of the table in the information database that holds system documents.
#define MYSQLRESERVEDWORDS "KEY TABLE DATABASE CHARACTER COLUMN CURRENT_TIME CHECK COLUMNS CROSS CURRENT_TIMESTAMP COLLATE CONSTRAINT CURRENT_DATE ADD ALL ALTER ANALYZE AND AS ASC AUTO_INCREMENT BDB BERKELEYDB BETWEEN BIGINT BINARY BLOB BOTH BTREE BY CASCADE CASE CHANGE CHAR CHARACTER CHECK" // This is all the mysql reserved words.
//Place all global settings here, can be use anywhere in the project.
//Thse setting are only set from settings.ini file.
struct  GlobalSettings {
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
    bool bImportPrimaryKey=true;
    bool bImportCreateTables=false;
    bool bImportData=false;
    bool bAutoCheckDefinitions=false;
    bool bShowGridColumnFields=false;

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


#define TITLE_WIDTH 100
#define BUTTON_WIDTH 100
#define TEXTCTL_WIDTH 500
#define CTRL_HEIGHT 20
#define TEXTCTLMULTI_HEIGHT 60
#define BORDER_WIDTH 2
#define ALLOW_TO_GROW 1 // 1, the control will grow with the sizer
#define DONT_ALLOW_TO_GROW 0
#define WINDOW_WIDTH 600

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
    CTL_STATIC = wxID_HIGHEST + 1700,
    CTL_TEXT,
    CTL_MULTI_TEXT,
    CTL_COMBO_LOOKUP_NAME,
    CTL_COMBO_ADDITIVE,
    CTL_COMBO,
    CTL_CHECKBOX,
    CTL_DATE,
    CTL_WEBLINK
};


//This class is used to generate fields for either database forms or dialog based forms
class FieldCtlItem  : public TableField
{
public:
    //Pointers to controls used in our forms. One of these will be active at any one time this this class represents only one single field of information.

    //Add more control pointers here when you need to use them in the forms.
    wxStaticText *TitleCtl;
    wxTextCtrl *textCtl;
    wxComboBox *comCtl;
    wxHyperlinkCtrl *linkCtl;
    wxDatePickerCtrl *datePickerCtl;
    wxCheckBox *pCheckBox;

    int iTypeOfControl; //Used to identify the type of control we have, combo - checkbox. Identified by the enum above
    wxBoxSizer *pSizer; // The controls will be stored in a box sizer
    wxString sIdentifier; // Used to identify this control in the list when we retrieve or set data
    wxString sData;//Used to store generic data for this field item.
    wxString sDescription; // A description associated with this control

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
        sData="";
        sDescription="";
        sIdentifier="";
        textCtl=nullptr;
        comCtl=nullptr;
        datePickerCtl=nullptr;
        linkCtl=nullptr;
        pSizer= nullptr;
        pCheckBox= nullptr;
        iTypeOfControl=CTL_STATIC; //Set the control type as a simple text as default
    };
};


WX_DECLARE_OBJARRAY(FieldCtlItem, ArrayFieldItem); //Used in GenericItemForm class
WX_DECLARE_OBJARRAY(TableField, ArrayTableField); //Used in GenericTable class
WX_DECLARE_OBJARRAY(TableField, ArrayGridItem); //Used in DBGrid class
WX_DECLARE_OBJARRAY(FieldCtlItem,ArrayFieldDataCtls); // Used in DialogbaseClass
