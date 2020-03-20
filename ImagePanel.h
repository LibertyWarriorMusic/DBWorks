
#pragma once

#include <wx/wx.h>
//#include <wx/sizer.h>

class wxImagePanel : public wxPanel
{
private:
    wxBitmap *image;
    bool bFileLoaded;

    
public:
    wxImagePanel(wxFrame* parent, const wxString& file, wxBitmapType format);
    ~wxImagePanel() override;
    
    void Rescale(int width, int height);
    void paintEvent(wxPaintEvent & evt);
    
    void render(wxDC& dc);
    
    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */
    
    DECLARE_EVENT_TABLE()
};
