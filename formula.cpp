#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
class Formula : public FormulaInterface
{
public:

    explicit Formula(std::string expression) try : ast_(ParseFormulaAST(expression))
    {}
    catch (const std::exception& re)
    {
        throw FormulaException(re.what());
    }

    Value Evaluate(const SheetInterface& sheet) const override
    {
        auto get_cell_ptr = [&sheet](const Position& pos) -> CellInterface*
                            {
                                return const_cast<CellInterface*>(sheet.GetCell(pos));
                            };
        try
        {
            return ast_.Execute(get_cell_ptr);
        }
        catch (const FormulaError& fe)
        {
            return fe;
        }
    }
    
    std::string GetExpression() const override
    {
        std::ostringstream ostr;
        ast_.PrintFormula(ostr);
        return ostr.str();
    }

    std::vector<Position> GetReferencedCells() const
    {
        const std::forward_list<Position>& formula_cells(ast_.GetCells());
        if (formula_cells.empty())
        {
            return {};
        }
        else
        {
            std::vector<Position> result{ formula_cells.begin(), formula_cells.end() };
            return { result.begin(), unique(result.begin(), result.end()) };
        }
    }

private:
    FormulaAST ast_;
};

}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression)
{
    return std::make_unique<Formula>(std::move(expression));
}
