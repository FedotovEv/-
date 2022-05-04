/***************************************************************
 * Name:      SpreadSheetFrontendApp.h
 * Purpose:   Defines Application Class
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-18
 * Copyright: Dichrograph ()
 * License:
 **************************************************************/

#ifndef SPREADSHEETFRONTENDAPP_H
#define SPREADSHEETFRONTENDAPP_H

#include <wx/app.h>
#include <wx/grid.h>
#include "../sheet.h"

class SpreadSheetFrontendApp : public wxApp
{
    public:
        Sheet my_sheet;
        wxString sheet_file_name;
        bool is_modified = false;
        virtual bool OnInit();
};

#endif // SPREADSHEETFRONTENDAPP_H
