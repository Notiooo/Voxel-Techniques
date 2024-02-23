#include "Application.h"
#include "States/CustomStates/ExitApplicationState.h"
#include "States/CustomStates/GameState.h"
#include "States/CustomStates/LogoState.h"
#include "Utils/Mouse.h"
#include "constants.h"
#include "pch.h"

namespace Voxino
{

constexpr int FRAMES_PER_SECOND = 120;

/**
 * @brief I kindly ask that the following variable not be changed, as the
 * physics calculations have been adjusted under the following framerate.
 * Thank you.
 *
 * @warning If you do not comply, the user will move differently and jump lower or higher.
 */
constexpr int MINIMAL_FIXED_UPDATES_PER_FRAME = 120;

const sf::Time Application::TIME_PER_FIXED_UPDATE_CALLS =
    sf::seconds(1.f / MINIMAL_FIXED_UPDATES_PER_FRAME);
const int Application::SCREEN_WIDTH = 1280;
const int Application::SCREEN_HEIGHT = 720;

Application::Application()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 24;
    settings.stencilBits = 8;
#ifndef _DEBUG
    settings.attributeFlags = sf::ContextSettings::Core;
#endif

    // settings.attributeFlags = sf::ContextSettings::Core;

    // If Core should be used, then there is
    // a need to change the way ImGui is plugged in. For the moment ImGui-SFML uses the SFML
    // graphics module - so ImGui will not draw on Core setting. This project is developed without
    // the SFML graphics module and works without it. In the future, it would be useful to plug in
    // ImGui in such a way that it no longer relies on the SFML graphics module.

    mGameWindow =
        std::make_unique<WindowToRender>(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Voxino",
                                         sf::Style::Titlebar | sf::Style::Close, settings);

    mGameWindow->setFramerateLimit(FRAMES_PER_SECOND);
    mGameWindow->setActive(true);
    loadResources();

#ifndef DISABLE_IMGUI
    ImGui::SFML::Init(*mGameWindow, sf::Vector2f(mGameWindow->getSize()), true);
    setupImGuiStyle();
#endif

    // GLEW setup
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup all application-flow states
    mAppStack.saveState<LogoState>(State_ID::LogoState, *mGameWindow);
    mAppStack.saveState<ExitApplicationState>(State_ID::ExitApplicationState);
    mAppStack.saveState<GameState>(State_ID::GameState, *mGameWindow);

    // Initial state of the statestack is TitleState
    mAppStack.push(State_ID::LogoState);
}

void Application::run()
{
    spdlog::info("Game starts, the resolution is {}x{}", mGameWindow->getSize().x,
                 mGameWindow->getSize().y);

    mtr_init("chrome-trace.json");
    if constexpr (not IS_MINITRACE_COLLECTING_AT_START)
    {
        mtr_stop();
    }
    else
    {
        spdlog::info("Minitrace started collecting data");
    }
    MTR_META_PROCESS_NAME("Game");
    MTR_META_THREAD_NAME("main thread");

    performApplicationLoop();

    mGameWindow->close();
#ifndef DISABLE_IMGUI
    ImGui::SFML::Shutdown();
#endif
    mtr_flush();
    mtr_shutdown();
}

void Application::performApplicationLoop()
{
    MINITRACE_COLLECT_FUNCTION();
    sf::Clock clock;
    auto frameTimeElapsed = sf::Time::Zero;
    mFixedUpdateClock.restart();
    while (isGameRunning)
    {
        MINITRACE_COLLECT_FUNCTION_CUSTOM("ApplicationLoop");
        frameTimeElapsed = clock.restart();
        update(frameTimeElapsed);
        fixedUpdateAtEqualIntervals();
        processEvents();

        render();

        mtr_flush();
    }
    mAppStack.forceInstantClear();
}

void Application::fixedUpdateAtEqualIntervals()
{
    MINITRACE_COLLECT_FUNCTION();
    mTimeSinceLastFixedUpdate += mFixedUpdateClock.restart();
    if (mTimeSinceLastFixedUpdate > TIME_PER_FIXED_UPDATE_CALLS)
    {
        do
        {
            mTimeSinceLastFixedUpdate -= TIME_PER_FIXED_UPDATE_CALLS;
            fixedUpdate(TIME_PER_FIXED_UPDATE_CALLS);
        }
        while (mTimeSinceLastFixedUpdate > TIME_PER_FIXED_UPDATE_CALLS);
    }
}

void Application::updateImGuiMiniTrace()
{
#ifdef MTR_ENABLED
    static bool isMinitraceCollecting = IS_MINITRACE_COLLECTING_AT_START;
    ImGui::Begin("MiniTrace", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (isMinitraceCollecting)
    {
        ImGui::Text("Collecting minitrace...");
    }
    else
    {
        ImGui::Text("Idle...");
    }
    if (ImGui::Button("Start collecting trace"))
    {
        mtr_start();
        isMinitraceCollecting = true;
        spdlog::info("Minitrace started collecting data");
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop collecting trace"))
    {
        mtr_stop();
        isMinitraceCollecting = false;
        spdlog::info("Minitrace stopped collecting data");
    }
    ImGui::End();
#endif
}

void Application::updateImGui(const sf::Time& deltaTime)
{
#ifndef DISABLE_IMGUI

    ImGui::SFML::Update(sf::Mouse::getPosition(*mGameWindow), sf::Vector2f(mGameWindow->getSize()),
                        deltaTime);

    updateImGuiMiniTrace();
    mAppStack.updateImGui(deltaTime.asSeconds());
#endif
}

void Application::processEvents()
{
    MINITRACE_COLLECT_FUNCTION();
    sf::Event event{};
    while (mGameWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            isGameRunning = false;
        }

#ifndef DISABLE_IMGUI
        ImGui::SFML::ProcessEvent(event);
#endif

        mAppStack.handleEvent(event);
    }
}

void Application::fixedUpdate(const sf::Time& deltaTime)
{
    MINITRACE_COLLECT_FUNCTION();
    const auto deltaTimeInSeconds = deltaTime.asSeconds();
    mAppStack.fixedUpdate(deltaTimeInSeconds);
}

void Application::update(const sf::Time& deltaTime)
{
    MINITRACE_COLLECT_FUNCTION();
    const auto deltaTimeInSeconds = deltaTime.asSeconds();
    Mouse::update(deltaTimeInSeconds, *mGameWindow);

    updateImGui(deltaTime);

    mAppStack.update(deltaTimeInSeconds);

    if (mAppStack.top() == State_ID::ExitApplicationState)
    {
        isGameRunning = false;
    }
}

void Application::render() const
{
    MINITRACE_COLLECT_FUNCTION();
    glClearColor(0.f, 0.f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the application
    mAppStack.draw(*mGameWindow);

#ifndef DISABLE_IMGUI
    mGameWindow->pushGLStates();
    ImGui::SFML::Render();
    mGameWindow->popGLStates();
#endif

    // display to the window
    mGameWindow->display();
}


void Application::loadResources()
{
    // Empty
}
void Application::setupImGuiStyle()
{
    // Thank you Trippasch
    // https://github.com/ocornut/imgui/issues/707
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Border
    colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

    // Text
    colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
    colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
    colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

    auto& style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;
}

}// namespace Voxino