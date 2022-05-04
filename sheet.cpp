#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <unordered_set>

using namespace std::literals;

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value)
{
      std::visit(
          [&](const auto& x) {
              output << x;
          },
          value);
      return output;
}

void Sheet::Clear()
{
    sheet_closure_.clear();
    minimal_printed_area_.rows = 0;
    minimal_printed_area_.cols = 0;
    cells_in_row.clear();
    cells_in_col.clear();
}

void Sheet::ClearDependences(Position del_pos)
{ // Функция удаляет узел del_pos из графа зависимостей
    if (!sheet_closure_.count(del_pos))
        return;
    Cell& del_cell = static_cast<Cell&>(*sheet_closure_.at(del_pos));
    // Перебираем все клетки. от которых мы зависим, и удаляем там восходящие связи, указывающие на нас
    for (Position current_cell_pos : del_cell.GetReferencedCells())
    {
        Cell& current_cell = static_cast<Cell&>(*sheet_closure_.at(current_cell_pos));
        current_cell.cell_dependences_up_.erase(del_pos);
    }
    // Уничтожаем список нисходящих зависимостей клетки del_pos.
    //del_cell.cell_dependences_down_.clear();
}

void Sheet::RestoreDependences(Position restore_pos)
{
    if (!sheet_closure_.count(restore_pos))
        return;
    // Вносим поправки в списки восходящих зависимостей.
    for (const Position current_cell_pos : sheet_closure_.at(restore_pos)->GetReferencedCells())
    {
        if (!sheet_closure_.count(current_cell_pos))
            InsertEmptyCell(current_cell_pos);
        Cell& current_cell = static_cast<Cell&>(*sheet_closure_.at(current_cell_pos));
        current_cell.cell_dependences_up_.insert(restore_pos);
    }
}

void Sheet::ResetCache(Position reset_pos)
{
    if (!sheet_closure_.count(reset_pos))
        return;
    Cell& reset_cell = static_cast<Cell&>(*sheet_closure_.at(reset_pos));
    reset_cell.cash_value_.reset();
    for (Position current_cell_pos : reset_cell.cell_dependences_up_)
    {
        if (sheet_closure_.count(current_cell_pos))
        {
            Cell& current_cell = static_cast<Cell&>(*sheet_closure_.at(current_cell_pos));
            if (current_cell.cash_value_)
                ResetCache(current_cell_pos);
        }
    }
}

bool Sheet::CheckCircularDependences(Position check_pos)
{
    cell_open_status_.clear();
    bool result = CheckCircularDependencesRec(check_pos, check_pos);
    cell_open_status_.clear();
    return result;
}

bool Sheet::CheckCircularDependencesRec(Position check_pos, Position start_pos)
{ // Обходим весь граф зависимостей поиском в глубину, начиная с текущего узла (текущей вершины) start_pos по "нисходящим" связям,
  // пытаясь обнаружить указатель на сам текущий узел. Если такой узел найден, в графе зависимостей имеется цикл, что недопустимо
  // и вызовет возврат из функции с ложным результатом. Если такого указателя нет, циклов, включающих текущую вершину. в графе
  // нет, и результатом функции будет истина.
    if (!sheet_closure_.count(check_pos))
        return true;
    Cell& check_cell = static_cast<Cell&>(*sheet_closure_.at(check_pos));
    cell_open_status_.insert(check_pos); // Достижение - вершина открыта!
    for (Position current_cell_pos : check_cell.GetReferencedCells())
    {
        if (current_cell_pos == start_pos)
            return false;
        if (!cell_open_status_.count(current_cell_pos)) // Вершина ещё не открыта
        {
            if (!CheckCircularDependencesRec(current_cell_pos, start_pos))
                return false;
        }
    }
    return true;
}

void Sheet::InsertEmptyCell(Position pos)
{
    if (!pos.IsValid())
        throw InvalidPositionException("SetCell - Invalid position");
    if (!sheet_closure_.count(pos))
    {
        cells_in_row[pos.row]++;
        cells_in_col[pos.col]++;
    }
    sheet_closure_[pos] = CreateCell();
    if ((pos.row + 1) > minimal_printed_area_.rows)
        minimal_printed_area_.rows = pos.row + 1;
    if ((pos.col + 1) > minimal_printed_area_.cols)
        minimal_printed_area_.cols = pos.col + 1;
}

void Sheet::SetCell(Position pos, std::string text)
{
    if (!pos.IsValid())
        throw InvalidPositionException("SetCell - Invalid position");
    std::unique_ptr<CellInterface> cell_interface_ptr = CreateCell();
    Cell* cell_ptr = static_cast<Cell*>(cell_interface_ptr.get());
    cell_ptr->Set(std::move(text), *this);
    // Создаём новый список нисходящих зависимостей.
    //for (const Position current_cell_pos : cell_ptr->GetReferencedCells())
    //    cell_ptr->cell_dependences_down_.insert(current_cell_pos);
    // Сохраним в поле old_cell_interface_ptr старое значение ячейки pos, если она существует.
    std::unique_ptr<CellInterface> old_cell_interface_ptr;
    if (sheet_closure_.count(pos))
    {
        ClearDependences(pos);
        old_cell_interface_ptr = std::move(sheet_closure_[pos]);
        Cell* old_cell_ptr = static_cast<Cell*>(old_cell_interface_ptr.get());
        cell_ptr->cell_dependences_up_ = old_cell_ptr->cell_dependences_up_;
    }
    // Включаем вновь сформированную ячейку в символьный массив таблицы
    sheet_closure_[pos] = std::move(cell_interface_ptr);
    // Проверим новую формулу на наличие циркулярных связей
    if (!CheckCircularDependences(pos))
    {   // Круговые взаимозависимости найдены - швыряем исключение
        // Восстановим исходное состояние символьного массива таблицы
        if (old_cell_interface_ptr)
        {
            sheet_closure_[pos] = std::move(old_cell_interface_ptr);
            RestoreDependences(pos);
        }
        else
        {
            sheet_closure_.erase(pos);
        }
        throw CircularDependencyException("#CIRC!");
    }
    RestoreDependences(pos);
    ResetCache(pos);
    // Ну и, наконец, внесём изменения в массивы слежения cells_in_row и cells_in_col
    // и исправим размер минимальной печатной области.
    if (!old_cell_interface_ptr)
    {
        cells_in_row[pos.row]++;
        cells_in_col[pos.col]++;
    }
    if ((pos.row + 1) > minimal_printed_area_.rows)
        minimal_printed_area_.rows = pos.row + 1;
    if ((pos.col + 1) > minimal_printed_area_.cols)
        minimal_printed_area_.cols = pos.col + 1;
}

const CellInterface* Sheet::GetCell(Position pos) const
{
    return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos)
{
    if (!pos.IsValid())
        throw InvalidPositionException("GetCell - Invalid position");
    if (sheet_closure_.count(pos))
        return sheet_closure_.at(pos).get();
    else
        return nullptr;
}

void Sheet::ClearCell(Position pos)
{
    if (!pos.IsValid())
        throw InvalidPositionException("ClearCell - Invalid position");
    if (sheet_closure_.count(pos))
    {
        ClearDependences(pos);
        ResetCache(pos);
        sheet_closure_.erase(pos);
        if (!(--cells_in_row[pos.row]))
            cells_in_row.erase(pos.row);
        if (!(--cells_in_col[pos.col]))
            cells_in_col.erase(pos.col);
        if ((pos.row + 1) >= minimal_printed_area_.rows)
        { // Удалена ячейка на нижней крайней линии печатаемой области.
          // Возможно, её нужно уменьшить снизу, чем и займёмся.
            while (minimal_printed_area_.rows &&
                  !cells_in_row.count(minimal_printed_area_.rows - 1))
                --minimal_printed_area_.rows;
        }
        if ((pos.col + 1) >= minimal_printed_area_.cols)
        { // Удалена ячейка на правой крайней линии печатаемой области.
          // Возможно, её нужно уменьшить справа.
            while (minimal_printed_area_.cols &&
                  !cells_in_col.count(minimal_printed_area_.cols - 1))
                --minimal_printed_area_.cols;
        }
    }
}

Size Sheet::GetPrintableSize() const
{
    return minimal_printed_area_;
}

void Sheet::PrintValues(std::ostream& output) const
{
    for (int i = 0; i < minimal_printed_area_.rows; ++i)
    {
        for (int j = 0; j < minimal_printed_area_.cols; ++j)
        {
            Position print_pos{i, j};
            if (sheet_closure_.count(print_pos))
                output << sheet_closure_.at(print_pos)->GetValue();
            if (j < minimal_printed_area_.cols - 1)
                output << '\t';
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const
{
    for (int i = 0; i < minimal_printed_area_.rows; ++i)
    {
        for (int j = 0; j < minimal_printed_area_.cols; ++j)
        {
            Position print_pos{i, j};
            if (sheet_closure_.count(print_pos))
                output << sheet_closure_.at(print_pos)->GetText();
            if (j < minimal_printed_area_.cols - 1)
                output << '\t';
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}
