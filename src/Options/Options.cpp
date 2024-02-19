#include "Options.h"

Options::Options()
{
    this->m_flags = ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoTitleBar;

    this->m_grid = &Grid::GetInstance();
}

Options::~Options()
{
}

Options &Options::GetInstance()
{
    static Options instance;
    return instance;
}

void Options::Update()
{
}

void Options::Draw()
{
    ImGui::SetNextWindowPos(this->m_position);
    ImGui::SetNextWindowSize(this->m_size);
    ImGui::Begin("Options", nullptr, this->m_flags);

    // add border to each item
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.2f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.2f));

    // draw the first column of the window
    static std::string algorithmName = "Select Algorithm";

    ImGui::PushItemWidth(265);
    // draw a combo box to select the type of algorithm
    if (ImGui::BeginCombo("###Algorithm", algorithmName.c_str()))
    {
        if (ImGui::Selectable("A*", false))
        {
            algorithmName = "A*";
            this->m_algorithm = std::bind(&Grid::RunAStar, this->m_grid);
        }
        if (ImGui::Selectable("Dijkstra", false))
        {
            algorithmName = "Dijkstra";
            this->m_algorithm = std::bind(&Grid::RunDijkstra, this->m_grid);
        }
        if (ImGui::Selectable("BFS", false))
        {
            algorithmName = "BFS";
            this->m_algorithm = std::bind(&Grid::RunBFS, this->m_grid);
        }
        if (ImGui::Selectable("DFS", false))
        {
            algorithmName = "DFS";
            this->m_algorithm = std::bind(&Grid::RunDFS, this->m_grid);
        }
        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();

    // put these buttons to the right of the window

    ImGui::NewLine();

    for (int i = 0; i < static_cast<int>(CellType::Count); i++)
    {
        ImGui::SameLine();

        CellType temp = static_cast<CellType>(i);
        if (temp == CellType::Empty ||
            temp == CellType::Current ||
            temp == CellType::Count ||
            temp == CellType::Visited ||
            temp == CellType::Path ||
            temp == CellType::Weight && algorithmName != "A*")
            continue;

        ImColor color = cellColors[static_cast<CellType>(i)];
        ImGui::PushStyleColor(ImGuiCol_Button, color.Value);
        ImGui::Button("##ColorButton", ImVec2(20, 20));
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(cellNames[static_cast<CellType>(i)].c_str());
            ImGui::EndTooltip();
        }
        if (ImGui::IsItemClicked())
        {
            this->m_selectedType = static_cast<CellType>(i);

            this->m_grid->SetSelectedCellType(this->m_selectedType);
        }
    }

    // put these buttons to the right of the window
    ImGui::SameLine(0, 20);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

    // draw a button to run the selected algorithm
    if (ImGui::Button("Run"))
    {
        if (!this->m_running && algorithmName != "Select Algorithm")
        {
            std::thread t(
                [this]()
                {
                    this->m_running = true;
                    this->m_grid->SetRunning(true);
                    this->m_algorithm();
                    this->m_running = false;
                    this->m_grid->SetRunning(false);
                });
            this->m_threadHandle = t.native_handle();
            t.detach();
        }
    }

    ImGui::SameLine();

    // draw a button to pause the algorithm
    std::string pauseButtonText = this->m_paused ? "Resume" : "Pause";
    if (ImGui::Button(pauseButtonText.c_str()))
    {
        this->m_paused = !this->m_paused;
        this->m_grid->SetPaused(this->m_paused);
    }

    ImGui::SameLine();

    // draw a button to clear the grid
    if (ImGui::Button("Clear"))
    {
        if (this->m_running)
            pthread_cancel(this->m_threadHandle);

        this->m_grid->Clear();
    }

    std::string selectedCellName = "Selected Cell: " + cellNames[this->m_selectedType];
    ImGui::Text("%s", selectedCellName.c_str());

    ImGui::SameLine(0, 30);
    if (this->m_running)
        ImGui::Text("%s", "Running");
    else if (this->m_paused)
        ImGui::Text("%s", "Paused");
    else
        ImGui::Text("%s", "Ended");

    ImGui::PopStyleVar();

    ImGui::PopStyleColor(2);

    ImGui::PopStyleVar(2);

    ImGui::End();
}

ImVec2 Options::GetPosition() const
{
    return this->m_position;
}

ImVec2 Options::GetSize() const
{
    return this->m_size;
}

void Options::SetPosition(ImVec2 position)
{
    this->m_position = position;
}

void Options::SetSize(ImVec2 size)
{
    this->m_size = size;
}
