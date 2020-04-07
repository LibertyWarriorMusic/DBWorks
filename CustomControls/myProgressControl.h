//
// Created by Nicholas Zounis on 26/3/20.
//

#ifndef DBWORKS_MYPROGRESSCONTROL_H
#define DBWORKS_MYPROGRESSCONTROL_H


class myProgressControl : public wxGauge
{
public:
    DECLARE_DYNAMIC_CLASS(myProgressControl);

    wxGauge * m_ProgressGauge;

    myProgressControl (){};
    myProgressControl (wxWindow* parent, wxWindowID id, int range)
            : wxGauge (parent, id, range) { Init(); }

    void Init();
    wxSize DoGetBestSize() const;
    void OnPaint(wxPaintEvent& event);
    void SetLabel( const wxString& label );
    void SetValue( int val );
    DECLARE_EVENT_TABLE();
};

#endif //DBWORKS_MYPROGRESSCONTROL_H
