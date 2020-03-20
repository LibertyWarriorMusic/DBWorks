#include "ImagePanel.h"



BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
// some useful events
/*
 EVT_MOTION(wxImagePanel::mouseMoved)
 EVT_LEFT_DOWN(wxImagePanel::mouseDown)
 EVT_LEFT_UP(wxImagePanel::mouseReleased)
 EVT_RIGHT_DOWN(wxImagePanel::rightClick)
 EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
 EVT_KEY_DOWN(wxImagePanel::keyPressed)
 EVT_KEY_UP(wxImagePanel::keyReleased)
 EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
 */

// catch paint events
EVT_PAINT(wxImagePanel::paintEvent)

END_EVENT_TABLE()


// some useful events
/*
 void wxImagePanel::mouseMoved(wxMouseEvent& event) {}
 void wxImagePanel::mouseDown(wxMouseEvent& event) {}
 void wxImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
 void wxImagePanel::mouseReleased(wxMouseEvent& event) {}
 void wxImagePanel::rightClick(wxMouseEvent& event) {}
 void wxImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
 void wxImagePanel::keyPressed(wxKeyEvent& event) {}
 void wxImagePanel::keyReleased(wxKeyEvent& event) {}
 */

wxImagePanel::wxImagePanel(wxFrame* parent, const wxString& file, wxBitmapType format) :
wxPanel(parent)
{
    image = new wxBitmap();
   // bFileLoaded = image.SetBitmap(wxBitmap(wxImage(file)));
    // load the file... ideally add a check to see if loading was successful
    wxInitAllImageHandlers();
    bFileLoaded = image->LoadFile(file, format);
    
}

wxImagePanel::~wxImagePanel()
{
    delete image;
}
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void wxImagePanel::Rescale(int width, int height)
{
    wxImage img=image->ConvertToImage();
    img.Rescale(width,height);
    
    
    wxBitmap * temp = image;
    image = new wxBitmap(img);
    Refresh();
    delete temp;
    
}
void wxImagePanel::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
/*
void wxImagePanel::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}
*/
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void wxImagePanel::render(wxDC&  dc)
{
    if(bFileLoaded)
        dc.DrawBitmap( *image, 0, 0, false );
    
    
   // wxPaintDC dc(this);
    //render(dc);
 //  dc.SetBrush(*wxGREEN_BRUSH); // green filling
 //  dc.SetPen( wxPen( wxColor(255,0,0), 1 ) ); // 5-pixels-thick red outline
           //dc.DrawCircle( wxPoint(50,50), 25 /* radius */ );
  // dc.DrawLine(0,0,100,100);
    
    

}

