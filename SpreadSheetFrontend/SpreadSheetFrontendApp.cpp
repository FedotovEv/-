/***************************************************************
 * Name:      SpreadSheetFrontendApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-18
 * Copyright: Dichrograph ()
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "SpreadSheetFrontendApp.h"

//(*AppHeaders
#include "SpreadSheetFrontendMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(SpreadSheetFrontendApp);

bool SpreadSheetFrontendApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	SpreadSheetFrontendFrame* Frame = new SpreadSheetFrontendFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
