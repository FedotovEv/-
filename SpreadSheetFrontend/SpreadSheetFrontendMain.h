/***************************************************************
 * Name:      SpreadSheetFrontendMain.h
 * Purpose:   Defines Application Frame
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-18
 * Copyright: Dichrograph ()
 * License:
 **************************************************************/

#ifndef SPREADSHEETFRONTENDMAIN_H
#define SPREADSHEETFRONTENDMAIN_H

#include <optional>
#include <tuple>
#include "../common.h"

//(*Headers(SpreadSheetFrontendFrame)
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
//*)

struct CellSetElem
{
    int cell_row;
    int cell_col;
    bool operator==(CellSetElem other)
    {
        return cell_row == other.cell_row && cell_col == other.cell_col;
    }

    bool operator<(CellSetElem other)
    {
        return std::tuple(cell_row, cell_col) < std::tuple(other.cell_row, other.cell_col);
    }
};

class SpreadSheetFrontendFrame: public wxFrame
{
    public:

        SpreadSheetFrontendFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~SpreadSheetFrontendFrame();

    private:
        static std::string CSVConvertString(const std::string& in_string);
        static std::pair<std::vector<std::string>, bool> CSVSplitString(const std::string& in_string);
        static void MyGetline(std::istream& in_str, std::string& next_str);
        void SetRealCellValue(Position grid_pos);
        void UpdateCellTree(Position root_pos);
        void SetModifiedStatus(bool new_modified_value);
        bool TestIsModified();
        void EventTestModified(wxCloseEvent& event);

        std::optional<Position> current_grid_pos;

        //(*Handlers(SpreadSheetFrontendFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnGrid1CellChange(wxGridEvent& event);
        void OnGrid1CellSelect(wxGridEvent& event);
        void OnMenuSaveSelected(wxCommandEvent& event);
        void OnMenuLoadSelected(wxCommandEvent& event);
        void OnMenuSaveAsSelected(wxCommandEvent& event);
        void OnMenuNewSelected(wxCommandEvent& event);
        //*)

        //(*Identifiers(SpreadSheetFrontendFrame)
        static const long ID_GRID1;
        static const long idMenuNew;
        static const long idMenuLoad;
        static const long idMenuSave;
        static const long idMenuSaveAs;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(SpreadSheetFrontendFrame)
        wxGrid* Grid1;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem5;
        wxMenuItem* MenuItem7;
        wxStatusBar* StatusBar1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // SPREADSHEETFRONTENDMAIN_H
