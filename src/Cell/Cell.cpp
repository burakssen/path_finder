#include "Cell.h"

#include <raylib.h>

Cell::Cell(ImVec2 position, ImVec2 size)
    : m_position(position), m_size(size)
{
}

Cell::~Cell()
{
}

void Cell::Update()
{
}

void Cell::Draw()
{

    ImVec2 size = ImVec2(this->m_size.x / 2, this->m_size.y / 2);
    ImVec2 position = ImVec2(this->m_position.x, this->m_position.y);

    if (this->m_animationTimer < 1)
    {
        if (m_animated)
        {
            this->m_animationTimer += 0.05;
            size.x *= this->m_animationTimer;
            size.y *= this->m_animationTimer;

            position.x += (this->m_size.x - size.x) * this->m_animationTimer;
            position.y += (this->m_size.y - size.y) * this->m_animationTimer;
        }
    }
    else
    {
        size.x = this->m_size.x;
        size.y = this->m_size.y;
        this->m_animated = false;
    }

    ImGui::GetWindowDrawList()->AddRectFilled(
        this->m_position,
        ImVec2(position.x + size.x, position.y + size.y),
        this->m_color);

    if (this->m_type == CellType::Weight)
    {
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(this->m_position.x + 5, this->m_position.y + 5),
            ImColor(0, 0, 0, 255),
            this->m_weight.c_str());
    }

    ImGui::GetWindowDrawList()->AddRect(
        this->m_position,
        ImVec2(this->m_position.x + this->m_size.x, this->m_position.y + this->m_size.y),
        ImColor(0, 0, 0, 255));
}

void Cell::SetType(CellType type, int degree)
{
    if (this->m_type == type)
    {
        return;
    }

    this->m_animated = true;
    this->m_animationTimer = 0;

    this->m_type = type;
    if (type == CellType::Path)
        this->m_color = cellColors[type] * degree;
    else
        this->m_color = cellColors[type];

    this->m_animationTimer = 0;
}

CellType Cell::GetType() const
{
    return this->m_type;
}

ImVec2 Cell::GetPosition() const
{
    return this->m_position;
}

ImVec2 Cell::GetSize() const
{
    return this->m_size;
}

bool Cell::IsMouseOver(ImVec2 mousePos) const
{
    return mousePos.x >= this->m_position.x && mousePos.x <= this->m_position.x + this->m_size.x &&
           mousePos.y >= this->m_position.y && mousePos.y <= this->m_position.y + this->m_size.y;
}

void Cell::AddNeighbor(Cell *cell)
{
    this->m_neighbors.push_back(cell);
}

std::vector<Cell *> Cell::GetNeighbors() const
{
    return this->m_neighbors;
}

void Cell::SetData(CellData data)
{
    this->m_data = data;
}

CellData Cell::GetData() const
{
    return this->m_data;
}

int Cell::GetDistance(Cell *cell) const
{
    return abs(this->m_position.x - cell->GetPosition().x) + abs(this->m_position.y - cell->GetPosition().y);
}

void Cell::SetParent(Cell *parent)
{
    this->m_parent = parent;
}

Cell *Cell::GetParent() const
{
    return this->m_parent;
}

void Cell::SetWeight(std::string weight)
{
    this->m_weight = weight;
}