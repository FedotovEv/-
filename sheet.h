#pragma once

#include "cell.h"
#include "common.h"

#include <map>
#include <functional>

class Sheet : public SheetInterface
{
public:
    void Clear() override;
    void SetCell(Position pos, std::string text) override;
    void InsertEmptyCell(Position pos) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:

    std::unordered_map<Position, std::unique_ptr<CellInterface>> sheet_closure_;
    Size minimal_printed_area_;
    std::unordered_map<int, int> cells_in_row;
    std::unordered_map<int, int> cells_in_col;
    // ‘лаги посещени€ (открыти€) вершин графа зависимости при выполнении поиска в глубину.
    std::unordered_set<Position> cell_open_status_;

    // ѕриватные методы класса
    void ClearDependences(Position del_pos);
    void RestoreDependences(Position pos);
    void ResetCache(Position reset_pos);
    bool CheckCircularDependences(Position check_pos);
    bool CheckCircularDependencesRec(Position check_pos, Position start_pos);
};
