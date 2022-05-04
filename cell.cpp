#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <memory>
#include <variant>
#include <algorithm>

using namespace std;

Cell::Cell() : impl_(make_unique<EmptyImpl>())
{}

void Cell::Set(std::string text, SheetInterface& sheet)
{
    unique_ptr<Impl> temp_impl;
    if (text.size() > 1 && text[0] == FORMULA_SIGN)
        temp_impl = make_unique<FormulaImpl>(text.substr(1), sheet); // В данной ячейке нужно сохранить формулу
    else
        temp_impl = make_unique<TextImpl>(move(text)); // В данной ячейке нужно сохранить строку
    impl_ = move(temp_impl);
}

void Cell::Clear()
{
    impl_.reset();
    impl_ = make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const
{
    if (cash_value_)
    {
        return cash_value_.value();
    }
    else
    {
        Value result = impl_->GetValue();
        if (holds_alternative<double>(result))
            cash_value_ = get<double>(result);
        return result;
    }
}

std::string Cell::GetText() const
{
    std::string result = impl_->GetText();
    if (dynamic_cast<FormulaImpl*>(impl_.get()))
        result.insert(result.begin(), FORMULA_SIGN);
    return result;
}

std::vector<Position> Cell::GetReferencedCells() const
{
    return impl_->GetReferencedCells();
}

vector<Position> Cell::GetUpDependencesCells() const
{
    vector<Position> result;
    // Множество cell_dependences_up_ - список "восходящих" зависимостей - клеток, которые зависят от нас
    for (Position cur_pos : cell_dependences_up_)
        result.push_back(cur_pos);
    sort(result.begin(), result.end());
    return result;
}

unique_ptr<CellInterface> CreateCell()
{
    return make_unique<Cell>();
}
