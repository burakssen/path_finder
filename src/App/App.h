#pragma once

#include <string>

#include <raylib.h>
#include <imgui.h>

#include <rlImGui.h>

#include "Options/Options.h"
#include "Grid/Grid.h"

class App
{
    App();

public:
    ~App();
    App(const App &) = delete;
    App &operator=(const App &) = delete;
    static App &GetInstance();
    void Run();

private:
    void Update();
    void Draw();

    void SetupTheme();

private:
    float m_width = 1024;
    float m_height = 768;

    std::string m_title = "Pathfinding Visualiser 2.0";

    Grid *m_grid = nullptr;
    Options *m_options = nullptr;
};
