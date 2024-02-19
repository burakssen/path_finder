#pragma once

#include <vector>
#include <memory>
#include <numeric>
#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <deque>

#include <imgui.h>

#include "Cell/Cell.h"

class Grid
{
    Grid();

public:
    ~Grid();
    Grid(const Grid &) = delete;
    Grid &operator=(const Grid &) = delete;
    static Grid &GetInstance();

    void Update();
    void Draw();

    void SetPosition(ImVec2 position);
    void SetSize(ImVec2 size);

    void SetSelectedCellType(CellType type);

    void SetPaused(bool paused);
    void SetRunning(bool running);
    void RunAStar();
    void RunDijkstra();
    void RunBFS();
    void RunDFS();
    void Clear();

private:
    ImVec2 m_position = {0, 0};
    ImVec2 m_size = {0, 0};
    ImVec2 m_cellSize = {0, 0};

    int m_rows = 0;
    int m_columns = 0;

    std::vector<std::vector<std::shared_ptr<Cell>>> m_cells;

    ImGuiWindowFlags m_flags;

    CellType m_selectedType = CellType::Empty;

    Cell *m_startCell = nullptr;
    Cell *m_endCell = nullptr;
    std::vector<Cell *> m_weightCells;

    bool m_paused = false;
    bool m_running = false;
};