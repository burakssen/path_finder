#pragma once

#include <imgui.h>
#include <thread>
#include <functional>
#include <pthread.h>

#include "Grid/Grid.h"
#include "Cell/Cell.h"

class Options
{
    Options();

public:
    ~Options();
    Options(const Options &) = delete;
    Options &operator=(const Options &) = delete;
    static Options &GetInstance();

    void Update();
    void Draw();

    ImVec2 GetPosition() const;
    ImVec2 GetSize() const;

    void SetPosition(ImVec2 position);
    void SetSize(ImVec2 size);

private:
    ImVec2 m_position = {0, 0};
    ImVec2 m_size = {0, 0};

    ImGuiWindowFlags m_flags;

    CellType m_selectedType = CellType::Empty;

    Grid *m_grid = nullptr;

    std::function<void()> m_algorithm;

    std::thread::native_handle_type m_threadHandle;
    bool m_running = false;
    bool m_paused = false;
};