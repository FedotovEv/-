#pragma once

#include "common.h"
#include "formula.h"

#include <unordered_set>
#include <optional>

class Impl
{
public:
    virtual ~Impl() = default;
    virtual void Set(std::string text) = 0;
    virtual void Clear() = 0;
    virtual CellInterface::Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
    virtual std::vector<Position> GetReferencedCells() const = 0;
    virtual bool IsReferenced() const = 0;
};

class EmptyImpl : public Impl
{
public:
    EmptyImpl() = default;
    virtual ~EmptyImpl() = default;

    virtual void Set(std::string text) override
    {}

    virtual void Clear() override
    {}

    virtual CellInterface::Value GetValue() const override
    {
        return 0.0;
    }

    virtual std::string GetText() const override
    {
        return {};
    }

    virtual std::vector<Position> GetReferencedCells() const
    {
        return {};
    }

    virtual bool IsReferenced() const
    {
        return false;
    }
};

class TextImpl : public Impl
{
public:
    TextImpl() = default;
    ~TextImpl() = default;

    TextImpl(std::string text) : cell_text_(std::move(text))
    {}

    virtual void Set(std::string text) override
    {
        cell_text_ = std::move(text);
    }

    virtual void Clear() override
    {
        cell_text_.clear();
    }

    virtual CellInterface::Value GetValue() const override
    {
        if (cell_text_.size() && cell_text_[0] == ESCAPE_SIGN)
            return cell_text_.substr(1);
        else
            return cell_text_;
    }

    virtual std::string GetText() const override
    {
        return cell_text_;
    }

    std::vector<Position> GetReferencedCells() const
    {
        return {};
    }

    bool IsReferenced() const
    {
        return false;
    }

private:
    std::string cell_text_;
};

class FormulaImpl : public Impl
{
public:
    FormulaImpl(const SheetInterface& sheet) : sheet_(sheet)
    {}
    ~FormulaImpl() = default;

    FormulaImpl(std::string text, const SheetInterface& sheet) : sheet_(sheet)
    {
        formula_ptr_ = ParseFormula(std::move(text));
    }

    virtual void Set(std::string text) override
    {
        formula_ptr_ = ParseFormula(std::move(text));
    }

    virtual void Clear() override
    {
        formula_ptr_.reset();
    }

    virtual CellInterface::Value GetValue() const override
    {
        if (!formula_ptr_)
            return FormulaError(FormulaError::Category::Value);
        FormulaInterface::Value result = formula_ptr_->Evaluate(sheet_);
        if (std::holds_alternative<double>(result))
            return std::get<double>(result);
        else
            return std::get<FormulaError>(result);
    }

    virtual std::string GetText() const override
    {
        if (!formula_ptr_)
            return {};
        return formula_ptr_->GetExpression();
    }

    std::vector<Position> GetReferencedCells() const
    {
        return formula_ptr_->GetReferencedCells();
    }

    bool IsReferenced() const
    {
        return true;
    }

private:
    std::unique_ptr<FormulaInterface> formula_ptr_;
    const SheetInterface& sheet_;
};

class Cell : public CellInterface
{
public:
    friend class Sheet;

    Cell();
    ~Cell() = default;

    void Set(std::string text, SheetInterface& sheet);
    void Clear();

    CellInterface::Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;
    bool IsReferenced() const;

    std::vector<Position> GetUpDependencesCells() const;

private:

    std::unique_ptr<Impl> impl_;
    mutable std::optional<double> cash_value_;
    // Поле cell_dependences_up_ - список "восходящих" зависимостей - клеток, которые зависят от нас
    std::unordered_set<Position> cell_dependences_up_;
    // Поле cell_dependences_down_ - список "нисходящих" зависимостей - клеток, от которых зависим мы
    //std::unordered_set<Position> cell_dependences_down_;
};

std::unique_ptr<CellInterface> CreateCell();
