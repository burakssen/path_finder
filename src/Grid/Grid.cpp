#include "Grid.h"

Grid::Grid()
{
    this->m_flags = ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoTitleBar;
}

Grid::~Grid()
{
}

Grid &Grid::GetInstance()
{
    static Grid instance;
    return instance;
}

void Grid::Update()
{

    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !this->m_running)
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        for (auto &row : this->m_cells)
        {
            for (auto &cell : row)
            {
                if (cell->IsMouseOver(mousePos))
                {
                    if (this->m_selectedType == CellType::Weight)
                    {
                        if (std::find(this->m_weightCells.begin(), this->m_weightCells.end(), cell.get()) != this->m_weightCells.end())
                        {
                            cell->SetType(CellType::Empty);
                            this->m_weightCells.erase(std::remove(this->m_weightCells.begin(), this->m_weightCells.end(), cell.get()), this->m_weightCells.end());
                        }

                        for (auto &weight : this->m_weightCells)
                            weight->SetWeight(std::to_string(std::distance(this->m_weightCells.begin(), std::find(this->m_weightCells.begin(), this->m_weightCells.end(), weight)) + 1));
                    }

                    if (this->m_selectedType == CellType::Start)
                    {
                        if (this->m_startCell != nullptr)
                        {
                            this->m_startCell->SetType(CellType::Empty);
                        }
                        this->m_startCell = cell.get();
                    }

                    if (this->m_selectedType == CellType::End)
                    {
                        if (this->m_endCell != nullptr)
                        {
                            this->m_endCell->SetType(CellType::Empty);
                        }
                        this->m_endCell = cell.get();
                    }

                    if (this->m_selectedType == CellType::Weight)
                    {
                        if (std::find(this->m_weightCells.begin(), this->m_weightCells.end(), cell.get()) == this->m_weightCells.end())
                        {
                            cell->SetWeight(std::to_string(this->m_weightCells.size() + 1));
                            this->m_weightCells.push_back(cell.get());
                        }
                    }

                    cell->SetType(this->m_selectedType);
                }
            }
        }
    }

    if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        for (auto &row : this->m_cells)
        {
            for (auto &cell : row)
            {
                if (cell->IsMouseOver(mousePos))
                {
                    cell->SetType(CellType::Empty);
                    if (std::find(this->m_weightCells.begin(), this->m_weightCells.end(), cell.get()) != this->m_weightCells.end())
                    {
                        this->m_weightCells.erase(std::remove(this->m_weightCells.begin(), this->m_weightCells.end(), cell.get()), this->m_weightCells.end());
                    }

                    for (auto &weight : this->m_weightCells)
                        weight->SetWeight(std::to_string(std::distance(this->m_weightCells.begin(), std::find(this->m_weightCells.begin(), this->m_weightCells.end(), weight)) + 1));
                }
            }
        }
    }

    for (auto &row : this->m_cells)
    {
        for (auto &cell : row)
        {
            cell->Update();
        }
    }
}

void Grid::Draw()
{
    ImGui::SetNextWindowPos(this->m_position);
    ImGui::SetNextWindowSize(this->m_size);
    ImGui::Begin("Grid", nullptr, this->m_flags);

    for (auto &row : this->m_cells)
    {
        for (auto &cell : row)
        {
            cell->Draw();
        }
    }

    ImGui::End();
}

void Grid::SetPosition(ImVec2 position)
{
    this->m_position = position;
}

void Grid::SetSize(ImVec2 size)
{
    this->m_size = size;

    ImVec2 gridAreaSize = {this->m_size.x - 24.0f, this->m_size.y - 41.2f};

    this->m_cellSize = {20, 20};

    this->m_rows = gridAreaSize.y / this->m_cellSize.y;
    this->m_columns = gridAreaSize.x / this->m_cellSize.x;

    this->m_cells.clear();
    this->m_cells.resize(this->m_rows);

    for (int i = 0; i < this->m_rows; i++)
    {
        this->m_cells[i].resize(this->m_columns);
    }

    for (int i = 0; i < this->m_rows; i++)
    {
        for (int j = 0; j < this->m_columns; j++)
        {
            ImVec2 position = {this->m_position.x + 10 + j * this->m_cellSize.x, this->m_position.y + 10 + i * this->m_cellSize.y};
            this->m_cells[i][j] = std::make_shared<Cell>(position, this->m_cellSize);
        }
    }

    for (int i = 0; i < this->m_rows; i++)
    {
        for (int j = 0; j < this->m_columns; j++)
        {
            if (i > 0)
            {
                this->m_cells[i][j]->AddNeighbor(this->m_cells[i - 1][j].get());
            }

            if (i < this->m_rows - 1)
            {
                this->m_cells[i][j]->AddNeighbor(this->m_cells[i + 1][j].get());
            }

            if (j > 0)
            {
                this->m_cells[i][j]->AddNeighbor(this->m_cells[i][j - 1].get());
            }

            if (j < this->m_columns - 1)
            {
                this->m_cells[i][j]->AddNeighbor(this->m_cells[i][j + 1].get());
            }
        }
    }
}

void Grid::SetSelectedCellType(CellType type)
{
    this->m_selectedType = type;
}

void Grid::SetPaused(bool paused)
{
    this->m_paused = paused;
}

void Grid::SetRunning(bool running)
{
    this->m_running = running;
}

void Grid::RunAStar()
{
    if (this->m_startCell == nullptr || this->m_endCell == nullptr)
    {
        std::cout << "Start or end cell not set" << std::endl;
        return;
    }

    std::vector<Cell *> openSet;
    std::vector<Cell *> closedSet;

    openSet.push_back(this->m_startCell);
    int weightedCellIndex = 0;

    while (!openSet.empty())
    {
        if (this->m_paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        Cell *current = openSet[0];

        for (int i = 1; i < openSet.size(); i++)
        {
            CellData currentData = current->GetData();
            CellData openSetData = openSet[i]->GetData();

            if (openSetData.aStarData.f < currentData.aStarData.f || (openSetData.aStarData.f == currentData.aStarData.f && openSetData.aStarData.h < currentData.aStarData.h))
            {
                current = openSet[i];
            }
        }

        openSet.erase(std::remove(openSet.begin(), openSet.end(), current), openSet.end());
        closedSet.push_back(current);

        Cell *follow = nullptr;
        if (weightedCellIndex < this->m_weightCells.size())
            follow = this->m_weightCells[weightedCellIndex];
        else
            follow = this->m_endCell;

        if (current == follow)
        {
            Cell *temp = current;
            while (temp->GetParent() != nullptr)
            {
                if (temp->GetType() != CellType::Start && temp->GetType() != CellType::End && temp->GetType() != CellType::Weight)
                {
                    temp->SetType(CellType::Path, weightedCellIndex + 1);
                }

                temp = temp->GetParent();

                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }

            if (follow == this->m_endCell)
            {
                return;
            }

            openSet.clear();
            closedSet.clear();

            this->m_weightCells[weightedCellIndex]->SetParent(nullptr);

            openSet.push_back(this->m_weightCells[weightedCellIndex]);

            weightedCellIndex++;

            continue;
        }

        if (current == this->m_endCell)
        {
            Cell *temp = current;
            while (temp->GetParent() != nullptr)
            {
                if (temp->GetType() != CellType::Start && temp->GetType() != CellType::End && temp->GetType() != CellType::Weight && temp->GetType() != CellType::Path)
                    temp->SetType(CellType::Path);
                temp = temp->GetParent();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            this->m_startCell->SetType(CellType::Start);
            this->m_endCell->SetType(CellType::End);
            return;
        }

        for (auto &neighbor : current->GetNeighbors())
        {
            if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end() || neighbor->GetType() == CellType::Wall)
            {
                continue;
            }

            CellData neighborData = neighbor->GetData();
            CellData currentData = current->GetData();
            int newMovementCostToNeighbor = currentData.aStarData.g + current->GetDistance(neighbor);
            if (newMovementCostToNeighbor < neighborData.aStarData.g || std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end())
            {
                CellData newData = neighbor->GetData();
                newData.aStarData.g = newMovementCostToNeighbor;
                newData.aStarData.h = neighbor->GetDistance(follow);

                neighbor->SetData(newData);
                neighbor->SetParent(current);

                if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end())
                {
                    openSet.push_back(neighbor);
                }
            }
        }

        for (auto &cell : openSet)
        {
            if (cell->GetType() != CellType::Start && cell->GetType() != CellType::End && cell->GetType() != CellType::Weight && cell->GetType() != CellType::Path)
                cell->SetType(CellType::Current);
        }

        for (auto &cell : closedSet)
        {
            if (cell->GetType() != CellType::Start && cell->GetType() != CellType::End && cell->GetType() != CellType::Weight && cell->GetType() != CellType::Path)
                cell->SetType(CellType::Visited);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

struct DijkstraComparator
{
    bool operator()(const std::pair<float, Cell *> &a, const std::pair<float, Cell *> &b)
    {
        return a.first > b.first;
    }

    bool operator()(const Cell *a, const Cell *b)
    {
        return a->GetData().distance > b->GetData().distance;
    }

    bool operator()(const Cell &a, const Cell &b)
    {
        return a.GetData().distance > b.GetData().distance;
    }
};

void Grid::RunDijkstra()
{
    if (this->m_startCell == nullptr || this->m_endCell == nullptr)
    {
        std::cout << "Start or end cell not set" << std::endl;
        return;
    }

    // create a priority queue to store cells
    std::priority_queue<Cell *, std::vector<Cell *>, DijkstraComparator> openSet;

    // set the start cell distance to 0
    CellData startData = this->m_startCell->GetData();
    startData.distance = 0;
    this->m_startCell->SetData(startData);

    // push the start cell to the priority queue
    openSet.push(this->m_startCell);

    // for each cell, set the distance to infinity and mark it as not visited
    for (auto &row : this->m_cells)
    {
        for (auto &cell : row)
        {
            if (cell.get() != this->m_startCell)
            {
                CellData data = cell->GetData();
                data.distance = std::numeric_limits<float>::infinity();
                data.visited = false;
                cell->SetData(data);
            }
        }
    }

    // loop until the priority queue is empty
    while (!openSet.empty())
    {
        if (this->m_paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        // get the cell with the smallest distance
        Cell *current = openSet.top();
        openSet.pop();

        // if the current cell is the end cell, we have found the path
        if (current == this->m_endCell)
        {

            // loop through the path and set the cell type to path
            Cell *temp = current;
            while (temp->GetParent() != nullptr)
            {
                temp->SetType(CellType::Path);
                temp = temp->GetParent();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }

            // set the end cell type to end
            this->m_endCell->SetType(CellType::End);

            // set the start cell type to start
            this->m_startCell->SetType(CellType::Start);

            // print path found

            // return from the function
            return;
        }

        // loop through the current cell's neighbors
        for (auto &neighbor : current->GetNeighbors())
        {
            // get the neighbor's data
            CellData neighborData = neighbor->GetData();

            // if the neighbor is a wall or is in the closed set, skip it
            if (neighbor->GetType() == CellType::Wall || neighborData.visited)
            {
                continue;
            }

            // calculate the new distance to the neighbor
            int newDistance = current->GetData().distance + current->GetDistance(neighbor);

            // if the new distance is less than the neighbor's distance, update the neighbor's distance
            if (newDistance < neighborData.distance)
            {
                neighborData.distance = newDistance;
                neighbor->SetData(neighborData);
                neighbor->SetParent(current);
                openSet.push(neighbor);
            }
            neighbor->SetType(CellType::Current);
        }

        // mark the current cell as visited
        CellData currentData = current->GetData();
        currentData.visited = true;
        current->SetData(currentData);

        // set the current cell type to visited
        current->SetType(CellType::Visited);

        // sleep for a short time
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void Grid::RunBFS()
{
    std::deque<Cell *> Q;
    CellData data = this->m_startCell->GetData();
    data.visited = true;
    this->m_startCell->SetData(data);
    // enqueue the start cell
    Q.push_back(this->m_startCell);
    while (!Q.empty())
    {
        if (this->m_paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        // dequeue a cell from the queue
        Cell *current = Q.front();
        Q.pop_front();

        current->SetType(CellType::Visited);

        if (current == this->m_endCell)
        {
            Cell *temp = current;
            while (temp->GetParent() != nullptr)
            {
                temp->SetType(CellType::Path);
                temp = temp->GetParent();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            this->m_startCell->SetType(CellType::Start);
            this->m_endCell->SetType(CellType::End);
            return;
        }

        for (auto &neighbor : current->GetNeighbors())
        {
            if (!neighbor->GetData().visited && neighbor->GetType() != CellType::Wall)
            {
                CellData neighborData = neighbor->GetData();
                neighborData.visited = true;
                neighbor->SetData(neighborData);
                neighbor->SetParent(current);
                neighbor->SetType(CellType::Current);
                Q.push_back(neighbor);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void Grid::RunDFS()
{
    this->m_running = true;
    std::deque<Cell *> Q;
    CellData data = this->m_startCell->GetData();
    data.visited = true;
    this->m_startCell->SetData(data);
    // enqueue the start cell
    Q.push_back(this->m_startCell);
    while (!Q.empty())
    {
        if (this->m_paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        // dequeue a cell from the queue
        Cell *current = Q.back();
        Q.pop_back();

        current->SetType(CellType::Visited);

        if (current == this->m_endCell)
        {
            Cell *temp = current;
            while (temp->GetParent() != nullptr)
            {
                temp->SetType(CellType::Path);
                temp = temp->GetParent();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            this->m_startCell->SetType(CellType::Start);
            this->m_endCell->SetType(CellType::End);
            return;
        }

        for (auto &neighbor : current->GetNeighbors())
        {
            if (!neighbor->GetData().visited && neighbor->GetType() != CellType::Wall)
            {
                CellData neighborData = neighbor->GetData();
                neighborData.visited = true;
                neighbor->SetData(neighborData);
                neighbor->SetParent(current);
                neighbor->SetType(CellType::Current);
                Q.push_back(neighbor);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void Grid::Clear()
{
    for (auto &row : this->m_cells)
    {
        for (auto &cell : row)
        {
            cell->SetType(CellType::Empty);
            cell->SetData(CellData());
            cell->SetParent(nullptr);
        }
    }

    for (auto &cell : this->m_weightCells)
    {
        cell->SetType(CellType::Empty);
        cell->SetData(CellData());
        cell->SetParent(nullptr);
    }

    this->m_weightCells.clear();

    this->m_startCell = nullptr;
    this->m_endCell = nullptr;
}