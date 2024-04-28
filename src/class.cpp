#include "../include/wx-3.1/wx/wx.h"


class MyFrame : public wxFrame
{
public:
    MyFrame(wxWindow* parent, wxWindowID id, const wxString& title)
        : wxFrame(parent, id, title)
    {
        Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);
    }

private:
    void OnClose(wxCloseEvent& event)
    {
        Destroy();
    }
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame(NULL, wxID_ANY, "TeleAddress");
    frame->Show(true);

    return true;
}

wxIMPLEMENT_APP(MyApp);