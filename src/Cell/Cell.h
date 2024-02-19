#pragma once

#include <map>
#include <string>
#include <vector>

#include <imgui.h>

typedef struct AStarData
{
    float f = 0;
    float g = 0;
    float h = 0;
} AStarData;

typedef struct CellData
{
    AStarData aStarData;
    float distance = 0;
    bool visited = false;
} CellData;

enum class CellType
{
    Empty,
    Wall,
    Weight,
    Start,
    End,
    Path,
    Visited,
    Current,
    Count
};

static std::map<CellType, ImColor> cellColors = {
    {CellType::Empty, ImColor(255, 255, 255, 255)},
    {CellType::Wall, ImColor(0, 0, 0, 255)},
    {CellType::Weight, ImColor(255, 215, 0, 255)},
    {CellType::Start, ImColor(0, 255, 0, 255)},
    {CellType::End, ImColor(255, 0, 0, 255)},
    {CellType::Path, ImColor(247, 39, 152, 255)},
    {CellType::Visited, ImColor(60, 90, 160, 255)},
    {CellType::Current, ImColor(235, 244, 0, 255)}};

static std::map<CellType, std::string> cellNames = {
    {CellType::Empty, "Empty"},
    {CellType::Wall, "Wall"},
    {CellType::Weight, "Weight"},
    {CellType::Start, "Start"},
    {CellType::End, "End"},
    {CellType::Path, "Path"},
    {CellType::Visited, "Visited"},
    {CellType::Current, "Current"}};

class Cell
{
public:
    Cell(ImVec2 position, ImVec2 size);
    ~Cell();

    void Update();
    void Draw();

    void SetType(CellType type, int degree = 1);
    CellType GetType() const;

    ImVec2 GetPosition() const;
    ImVec2 GetSize() const;

    bool IsMouseOver(ImVec2) const;

    void AddNeighbor(Cell *cell);
    std::vector<Cell *> GetNeighbors() const;

    void SetData(CellData data);
    CellData GetData() const;

    int GetDistance(Cell *cell) const;

    void SetParent(Cell *parent);
    Cell *GetParent() const;

    void SetWeight(std::string weight);

private:
    ImVec2 m_position = {0, 0};
    ImVec2 m_size = {0, 0};

    CellType m_type = CellType::Empty;
    ImColor m_color = cellColors[CellType::Empty];

    std::vector<Cell *> m_neighbors;
    CellData m_data;
    Cell *m_parent = nullptr;
    float m_animationTimer = 1.0f;
    bool m_animated = true;
    std::string m_weight = "0";
};
