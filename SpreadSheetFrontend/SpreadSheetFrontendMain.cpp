/***************************************************************
 * Name:      SpreadSheetFrontendMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-18
 * Copyright: Dichrograph ()
 * License:
 **************************************************************/

#include <string>
#include <optional>
#include <variant>
#include <fstream>
#include <set>
#include "wx_pch.h"
#include "SpreadSheetFrontendMain.h"
#include <wx/msgdlg.h>
#include "../common.h"
#include "../formula.h"
#include "../sheet.h"
#include "SpreadSheetFrontendApp.h"

using namespace std;

//(*InternalHeaders(SpreadSheetFrontendFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(SpreadSheetFrontendFrame)
const long SpreadSheetFrontendFrame::ID_GRID1 = wxNewId();
const long SpreadSheetFrontendFrame::idMenuNew = wxNewId();
const long SpreadSheetFrontendFrame::idMenuLoad = wxNewId();
const long SpreadSheetFrontendFrame::idMenuSave = wxNewId();
const long SpreadSheetFrontendFrame::idMenuSaveAs = wxNewId();
const long SpreadSheetFrontendFrame::idMenuQuit = wxNewId();
const long SpreadSheetFrontendFrame::idMenuAbout = wxNewId();
const long SpreadSheetFrontendFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SpreadSheetFrontendFrame,wxFrame)
    //(*EventTable(SpreadSheetFrontendFrame)
    //*)
END_EVENT_TABLE()

SpreadSheetFrontendFrame::SpreadSheetFrontendFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(SpreadSheetFrontendFrame)
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;

    Create(parent, wxID_ANY, _("Электронная таблица"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    Grid1 = new wxGrid(this, ID_GRID1, wxPoint(120,128), wxDefaultSize, 0, _T("ID_GRID1"));
    Grid1->CreateGrid(1000,702);
    Grid1->EnableEditing(true);
    Grid1->EnableGridLines(true);
    Grid1->SetDefaultCellFont( Grid1->GetFont() );
    Grid1->SetDefaultCellTextColour( Grid1->GetForegroundColour() );
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem7 = new wxMenuItem(Menu1, idMenuNew, _("Новая таблица\tCtrl-N"), _("Создание новой таблицы"), wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
    Menu1->AppendSeparator();
    MenuItem3 = new wxMenuItem(Menu1, idMenuLoad, _("Открыть\tCtrl-O"), _("Загрузка таблицы из файла"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem5 = new wxMenuItem(Menu1, idMenuSave, _("Сохранить\tCtrl-S"), _("Сохранение таблицы в файл"), wxITEM_NORMAL);
    Menu1->Append(MenuItem5);
    MenuItem4 = new wxMenuItem(Menu1, idMenuSaveAs, _("Сохранить как"), _("Сохранить таблицу в файл под другим именем"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    Menu1->AppendSeparator();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Выход\tAlt-F4"), _("Покинуть программу"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("Файл"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("О программе\tF1"), _("Информация о программе"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Помощь"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[3] = { -1, -1, 60 };
    int __wxStatusBarStyles_1[3] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(3,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(3,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(ID_GRID1,wxEVT_GRID_CELL_CHANGE,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnGrid1CellChange);
    Connect(ID_GRID1,wxEVT_GRID_SELECT_CELL,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnGrid1CellSelect);
    Connect(idMenuNew,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnMenuNewSelected);
    Connect(idMenuLoad,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnMenuLoadSelected);
    Connect(idMenuSave,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnMenuSaveSelected);
    Connect(idMenuSaveAs,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnMenuSaveAsSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpreadSheetFrontendFrame::OnAbout);
    //*)

    Connect(wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&SpreadSheetFrontendFrame::EventTestModified);
}

SpreadSheetFrontendFrame::~SpreadSheetFrontendFrame()
{
    //(*Destroy(SpreadSheetFrontendFrame)
    //*)
}

void SpreadSheetFrontendFrame::EventTestModified(wxCloseEvent& event)
{
    if (TestIsModified())
        event.Veto();
    else
        event.Skip();
    return;
}

void SpreadSheetFrontendFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void SpreadSheetFrontendFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = _("Демонстрационная оболочка электронной таблицы");
    wxMessageBox(msg, _("О программе"));
}

bool SpreadSheetFrontendFrame::TestIsModified()
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);

    if (sheet_app->is_modified)
        return wxNO == wxMessageBox(_("Электронная таблица изменена и не сохранялась. Продолжить?"),
                                    _("Внимание"), wxYES_NO|wxNO_DEFAULT);
    else
        return false;
}

string SpreadSheetFrontendFrame::CSVConvertString(const string& in_string)
{
    string out_string;
    bool quote_flag = false;
    for (char c : in_string)
        switch (c)
        {
            case '\"':
                out_string += c;
            case '\r':
            case '\n':
            case '\t':
            case ' ':
                quote_flag = true;
            default:
                out_string += c;
        }
    if (quote_flag)
        out_string = '\"'+ out_string + '\"';
    return out_string;
}

void SpreadSheetFrontendFrame::MyGetline(istream& in_str, string& next_str)
{
    enum class WhatFindEol
    {
        FIND_TEXT = 1,
        FIND_CARRIAGE_RETURN,
        FIND_NEW_LINE
    };

    int c;
    WhatFindEol find_mode;

    next_str.clear();
    find_mode = WhatFindEol::FIND_TEXT;
    while (in_str.good())
    {
        c = in_str.get();
        if (!in_str || c < 0 || c > 255)
            break;
        switch (find_mode)
        {
            case WhatFindEol::FIND_TEXT:
                if (c == '\r')
                    find_mode = WhatFindEol::FIND_NEW_LINE;
                else if (c == '\n')
                    find_mode = WhatFindEol::FIND_CARRIAGE_RETURN;
                else
                    next_str += c;
                break;
            case WhatFindEol::FIND_CARRIAGE_RETURN:
                if (c != '\r')
                    in_str.unget();
                goto ImmExitGetLine;
            case WhatFindEol::FIND_NEW_LINE:
                if (c != '\n')
                    in_str.unget();
                goto ImmExitGetLine;
        }
    }

ImmExitGetLine:
    if (!next_str.size())
        in_str.setstate(ios_base::failbit);
    return;
}

pair<vector<string>, bool> SpreadSheetFrontendFrame::CSVSplitString(const string& in_string)
{
    enum class WhatFind
    {
        FIND_BEGIN_PARAM = 1,
        FIND_IN_PARAM,
        FIND_END_PARAM
    };

    vector<string> result;
    string next_str;
    static string spec_symb("\"\r\n\t ");

    WhatFind what_find_mode = WhatFind::FIND_BEGIN_PARAM;
    bool is_quote_param;
    for (int i = 0; i < static_cast<int>(in_string.size()); ++i)
    {
        char c = in_string[i];
        char nc = 0;
        if (i < static_cast<int>(in_string.size()) - 1)
            nc = in_string[i + 1];
        switch (what_find_mode)
        {
            case WhatFind::FIND_BEGIN_PARAM:
                if (c == '\"')
                {
                    is_quote_param = true;
                    next_str.clear();
                }
                else
                {
                    is_quote_param = false;
                    next_str.clear();
                    if (c == ',')
                    {
                        result.push_back(move(next_str));
                        next_str.clear();
                        break;
                    }
                    else
                    {
                        next_str += c;
                    }
                }
                what_find_mode = WhatFind::FIND_IN_PARAM;
                break;
            case WhatFind::FIND_IN_PARAM:
                if (is_quote_param)
                {
                    if (c == '\"' )
                    {
                        if (nc == '\"')
                        {
                            next_str += '\"';
                            ++i;
                        }
                        else
                        {
                            what_find_mode = WhatFind::FIND_END_PARAM;
                        }
                    }
                    else
                    {
                        next_str += c;
                    }
                }
                else
                {
                    if (c == ',')
                    {
                        result.push_back(move(next_str));
                        next_str.clear();
                        what_find_mode = WhatFind::FIND_BEGIN_PARAM;
                    }
                    else if (spec_symb.find_first_of(c) != string::npos)
                    {
                        return {result, false};
                    }
                    else
                    {
                        next_str += c;
                    }
                }
                break;
            case WhatFind::FIND_END_PARAM:
                if (c == ',')
                {
                    result.push_back(move(next_str));
                    next_str.clear();
                    what_find_mode = WhatFind::FIND_BEGIN_PARAM;
                }
                else
                {
                    return {result, false};
                }
                break;
        }
    }
    result.push_back(move(next_str));
    return {result, true};
}

void SpreadSheetFrontendFrame::SetRealCellValue(Position grid_pos)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    CellInterface* cell_ptr = sheet_app->my_sheet.GetCell(grid_pos);
    wxGridCellCoords grid_coords(grid_pos.row, grid_pos.col);
    string cell_text_content, cell_value_content;
    wxString wx_cell_text_content, wx_cell_value_content;

    if (cell_ptr)
    {
        cell_text_content = cell_ptr->GetText();
        CellInterface::Value cell_value = cell_ptr->GetValue();
        if (holds_alternative<string>(cell_value))
            cell_value_content = get<string>(cell_value);
        else if (holds_alternative<double>(cell_value))
            cell_value_content = to_string(get<double>(cell_value));
        else
            cell_value_content = get<FormulaError>(cell_value).ToString();
        wx_cell_text_content = wxString::From8BitData(cell_text_content.data(), cell_text_content.size());
        wx_cell_value_content = wxString::From8BitData(cell_value_content.data(), cell_value_content.size());
    }

    if (current_grid_pos.has_value() && grid_pos == current_grid_pos.value())
    { // Устанавливаем текстовое отображение для текущей ячейки - таковым будет её истинный текст
        Grid1->SetCellValue(grid_coords, wx_cell_text_content);
        StatusBar1->SetStatusText(wx_cell_value_content, 0);
    }
    else
    { // Устаналиваем текст для нетекущей ячейки - выводим в такую ячейку её вычисленное значение
        Grid1->SetCellValue(grid_coords, wx_cell_value_content);
        StatusBar1->SetStatusText(wx_cell_text_content, 0);
    }
}

void SpreadSheetFrontendFrame::SetModifiedStatus(bool new_modified_value)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    sheet_app->is_modified = new_modified_value;
    if (sheet_app->is_modified)
        StatusBar1->SetStatusText(_("Изменена"), 2);
    else
        StatusBar1->SetStatusText(_(""), 2);
}

void SpreadSheetFrontendFrame::UpdateCellTree(Position root_pos)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    Cell* cell_ptr = static_cast<Cell*>(sheet_app->my_sheet.GetCell(root_pos));
    if (!cell_ptr)
        return;
    auto ref_cell_vector = cell_ptr->GetUpDependencesCells();
    for (Position leaf_pos : ref_cell_vector)
        SetRealCellValue(leaf_pos);
    for (Position leaf_pos : ref_cell_vector)
        UpdateCellTree(leaf_pos);
}

void SpreadSheetFrontendFrame::OnGrid1CellChange(wxGridEvent& event)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    Position pos{event.GetRow(), event.GetCol()};
    wxGridCellCoords grid_coords(pos.row, pos.col);
    wxString cell_content = Grid1->GetCellValue(grid_coords);
    string sheet_cell_content = string(cell_content.To8BitData(), cell_content.size());
    if (sheet_cell_content[0] == '=') // Если это формула, переводим её в верхний регистр
        for (char& c : sheet_cell_content) c = toupper(c);

    wxString err_caption = _("Error in cell content");
    try
    {
        sheet_app->my_sheet.SetCell(pos, sheet_cell_content);
    }
    catch (InvalidPositionException& ipe)
    {
        wxMessageBox(_("Недопустимая ссылка на ячейку"), err_caption);
    }
    catch (FormulaException& fe)
    {
        wxMessageBox(_("Синтаксическая ошибка в формуле"), err_caption);
    }
    catch (CircularDependencyException& cde)
    {
        wxMessageBox(_("Формула с циклическими ссылками на ячейки"), err_caption);
    }
    SetRealCellValue(pos);
    UpdateCellTree(pos);
    SetModifiedStatus(true);
}

void SpreadSheetFrontendFrame::OnGrid1CellSelect(wxGridEvent& event)
{
    Position new_pos{event.GetRow(), event.GetCol()};

    if (current_grid_pos)
    {
        Position previous_grid_pos = current_grid_pos.value();
        current_grid_pos = new_pos;
        SetRealCellValue(previous_grid_pos);
    }
    else
    {
        current_grid_pos = new_pos;
    }
    SetRealCellValue(new_pos);
    event.Skip();
}

void SpreadSheetFrontendFrame::OnMenuSaveSelected(wxCommandEvent& event)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    if (!sheet_app->sheet_file_name.size())
        OnMenuSaveAsSelected(event);
    if (!sheet_app->sheet_file_name.size())
        return;
    ofstream out_file(sheet_app->sheet_file_name.mb_str(), ios::binary);
    Size print_size = sheet_app->my_sheet.GetPrintableSize();
    for (int i = 0; i < print_size.rows; ++i)
    {
        string summ_row;
        for (int j = 0; j < print_size.cols; ++j)
        {
            CellInterface* cell_ptr = sheet_app->my_sheet.GetCell(Position{i, j});
            if (cell_ptr)
                summ_row += CSVConvertString(cell_ptr->GetText());
            if (j < print_size.cols - 1)
                summ_row += ',';
        }
        out_file << summ_row << "\r\n";
    }
    SetModifiedStatus(false);
}

void SpreadSheetFrontendFrame::OnMenuLoadSelected(wxCommandEvent& event)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    if (TestIsModified())
        return;

    wxFileDialog load_file_dialog(this, _("Открыть файл электронной таблицы"), _(""), _(""),
                    _("CSV файлы (*.csv)|*.csv"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (load_file_dialog.ShowModal() == wxID_CANCEL)
        return;
    sheet_app->sheet_file_name = load_file_dialog.GetPath();
    sheet_app->my_sheet.Clear();
    Grid1->ClearGrid();
    Grid1->SetGridCursor(0, 0);
    SetModifiedStatus(false);
    StatusBar1->SetStatusText(sheet_app->sheet_file_name, 1);
    current_grid_pos = Position{0, 0};
    ifstream in_file(sheet_app->sheet_file_name.mb_str(), ios::binary);
    int row_num = 0, col_count = -1;

    while (in_file)
    {
        string next_str;
        MyGetline(in_file, next_str);
        if (!in_file)
            break;
        pair<vector<string>, bool> split_result = CSVSplitString(next_str);
        if (!split_result.second)
        {
            wxMessageBox(_("Неверный формат файла.\nНеверный формат строки."),
                         _("Ошибка в загружаемом файле"));
            return;
        }
        vector<string> col_params = move(split_result.first);
        if (col_count < 0)
        {
            col_count = col_params.size();
        }
        else
        {
            if (col_count != static_cast<int>(col_params.size()))
            {
                wxMessageBox(_("Неверный формат файла.\nРазное количество колонок в строках."),
                             _("Ошибка в загружаемом файле"));
                return;
            }
        }
        for(int col_num = 0; col_num < static_cast<int>(col_params.size()); ++col_num)
        {
            Position pos{row_num, col_num};
            sheet_app->my_sheet.SetCell(pos, col_params[col_num]);
            SetRealCellValue(pos);
        }
    ++row_num;
    }
}

void SpreadSheetFrontendFrame::OnMenuSaveAsSelected(wxCommandEvent& event)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    wxFileDialog save_file_dialog(this, _("Сохранить электронную таблицу в файл"), _(""), _(""),
                    _("CSV файлы (*.csv)|*.csv"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return;
    sheet_app->sheet_file_name = save_file_dialog.GetPath();
    StatusBar1->SetStatusText(sheet_app->sheet_file_name, 1);
    OnMenuSaveSelected(event);
}

void SpreadSheetFrontendFrame::OnMenuNewSelected(wxCommandEvent& event)
{
    SpreadSheetFrontendApp* sheet_app = static_cast<SpreadSheetFrontendApp*>(wxTheApp);
    if (TestIsModified())
        return;

    sheet_app->sheet_file_name.clear();
    StatusBar1->SetStatusText(sheet_app->sheet_file_name, 1);
    sheet_app->my_sheet.Clear();
    Grid1->ClearGrid();
    Grid1->SetGridCursor(0, 0);
    SetModifiedStatus(false);
}
