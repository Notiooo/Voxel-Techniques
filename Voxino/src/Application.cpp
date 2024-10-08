#include "Application.h"
#include "pch.h"

#include "States/CustomStates/ExitApplicationState.h"
#include "States/CustomStates/Polygons/PolygonMultiChunkState.h"
#include "States/CustomStates/Polygons/PolygonSingleChunkState.h"
#include "States/CustomStates/Raycast/Multi/RaycastMultiChunk.h"
#include "States/CustomStates/Raycast/Single/RaycastSingleChunkTexturedBrickmapGpu.h"
#include "States/CustomStates/Raycast/Single/RaycastSingleChunkTexturedOctreeGpu.h"
#include "States/CustomStates/Raycast/Single/RaycastSingleChunkTexturedVoxels.h"
#include "States/CustomStates/Raycast/Single/RaycastSingleChunkTexturedVoxelsFixedStep.h"
#include "Utils/FpsCounter.h"
#include "Utils/Mouse.h"
#include "World/Polygons/Chunks/Types/ChunkBinaryGreedyMeshing.h"
#include "World/Polygons/Chunks/Types/ChunkCullingGpu.h"
#include "World/Polygons/Chunks/Types/ChunkGreedyMeshing.h"
#include "World/Polygons/Chunks/Types/ChunkNaive.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Voxino
{

/**
 * @brief I kindly ask that the following variable not be changed, as the
 * physics calculations have been adjusted under the following framerate.
 * Thank you.
 *
 * @warning If you do not comply, the user will move differently and jump lower or higher.
 */
constexpr int MINIMAL_FIXED_UPDATES_PER_FRAME = 60;

const sf::Time Application::TIME_PER_FIXED_UPDATE_CALLS =
    sf::seconds(1.f / MINIMAL_FIXED_UPDATES_PER_FRAME);
const int Application::SCREEN_WIDTH = 1920;
const int Application::SCREEN_HEIGHT = 1080;


void Application::configureImGuiSinks()
{
    auto imguiSink = std::make_shared<ImGuiLogSink<std::mutex>>(&mImguiLog);
    imguiSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    auto logger = std::make_shared<spdlog::logger>("multi_sink",
                                                   spdlog::sinks_init_list{imguiSink, consoleSink});
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}

void Application::configureImGui()
{
#ifndef DISABLE_IMGUI
    configureImGuiSinks();
    ImGui::SFML::Init(*mGameWindow);
    setupImGuiStyle();
    ImGuiIO& io = ImGui::GetIO();
    if (!(io.ConfigFlags & ImGuiConfigFlags_DockingEnable))
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
#endif
}

void Application::setupGlew()
{
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }
}

void Application::setupFlowStates()
{
    mAppStack.saveState<ExitApplicationState>(State_ID::ExitApplicationState);
    mAppStack.saveState<PolygonSingleChunkState<Polygons::ChunkCulling>>(
        State_ID::PolygonSingleChunkCullingState, *mGameWindow);
    mAppStack.saveState<PolygonSingleChunkState<Polygons::ChunkNaive>>(
        State_ID::PolygonSingleChunkNaiveState, *mGameWindow);
    mAppStack.saveState<PolygonSingleChunkState<Polygons::ChunkGreedyMeshing>>(
        State_ID::PolygonSingleChunkGreedyState, *mGameWindow);
    mAppStack.saveState<PolygonSingleChunkState<Polygons::ChunkBinaryGreedyMeshing>>(
        State_ID::PolygonSingleChunkBinaryGreedyState, *mGameWindow);
    mAppStack.saveState<PolygonSingleChunkState<Polygons::ChunkCullingGpu>>(
        State_ID::PolygonSingleChunkCullingGpuState, *mGameWindow, "ChunkCullingGpu");
    mAppStack.saveState<PolygonMultiChunkState<Polygons::ChunkCulling>>(
        State_ID::PolygonMultiChunkCullingState, *mGameWindow);
    mAppStack.saveState<PolygonMultiChunkState<Polygons::ChunkNaive>>(
        State_ID::PolygonMultiChunkNaiveState, *mGameWindow);
    mAppStack.saveState<PolygonMultiChunkState<Polygons::ChunkGreedyMeshing>>(
        State_ID::PolygonMultiChunkGreedyState, *mGameWindow);
    mAppStack.saveState<PolygonMultiChunkState<Polygons::ChunkBinaryGreedyMeshing>>(
        State_ID::PolygonMultiChunkBinaryGreedyState, *mGameWindow);
    mAppStack.saveState<PolygonMultiChunkState<Polygons::ChunkCullingGpu>>(
        State_ID::PolygonMultiChunkCullingGpuState, *mGameWindow, "ChunkCullingGpu");

    mAppStack.saveState<RaycastSingleChunkTexturedVoxels>(
        State_ID::RaycastSingleChunkTexturedVoxels, *mGameWindow);
    mAppStack.saveState<RaycastSingleChunkTexturedVoxelsFixedStep>(
        State_ID::RaycastSingleChunkTexturedVoxelsFixedStep, *mGameWindow);
    mAppStack.saveState<RaycastSingleChunkTexturedBrickmapGpu>(
        State_ID::RaycastSingleChunkTexturedBrickmapGpu, *mGameWindow);
    mAppStack.saveState<RaycastSingleChunkTexturedOctreeGpu>(
        State_ID::RaycastSingleChunkTexturedOctreeGpu, *mGameWindow);

    mAppStack.saveState<RaycastMultiChunk<Raycast::RaycastChunk>>(
        State_ID::RaycastMultiChunkTexturedVoxels, *mGameWindow,
        SHADER_FILE_NAME(ChunkTexturedVoxels));
    mAppStack.saveState<RaycastMultiChunk<Raycast::RaycastChunk>>(
        State_ID::RaycastMultiChunkTexturedVoxelsFixedStep, *mGameWindow,
        SHADER_FILE_NAME(ChunkTexturedVoxelsFixedStep));

    mAppStack.saveState<RaycastMultiChunk<Raycast::RaycastChunkBrickmapGpu>>(
        State_ID::RaycastMultiChunkTexturedBrickmapGpu, *mGameWindow,
        SHADER_FILE_NAME(ChunkTexturedVoxelsBrickmapGpu));

    mAppStack.saveState<RaycastMultiChunk<Raycast::RaycastChunkOctreeGpu>>(
        State_ID::RaycastMultiChunkTexturedOctreeGpu, *mGameWindow,
        SHADER_FILE_NAME(ChunkTexturedVoxelsOctreeGpu));
}

Application::Application()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.attributeFlags = sf::ContextSettings::Core;

    mGameWindow =
        std::make_unique<WindowToRender>(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Voxino",
                                         sf::Style::Titlebar | sf::Style::Close, settings);

    mGameWindow->setActive(true);
    loadResources();
    configureImGui();
    setupGlew();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    TracyGpuContext;
    initializeTracyScreenCapture();
    setupFlowStates();
    // mAppStack.push(State_ID::PolygonSingleChunkNaiveState);
    // mAppStack.push(State_ID::PolygonSingleChunkCullingState);
    // mAppStack.push(State_ID::PolygonSingleChunkCullingGpuState);
    // mAppStack.push(State_ID::PolygonSingleChunkGreedyState);
    // mAppStack.push(State_ID::PolygonSingleChunkBinaryGreedyState);

    // mAppStack.push(State_ID::PolygonMultiChunkNaiveState);
    // mAppStack.push(State_ID::PolygonMultiChunkCullingState);
    // mAppStack.push(State_ID::PolygonMultiChunkCullingGpuState);
    // mAppStack.push(State_ID::PolygonMultiChunkGreedyState);
    // mAppStack.push(State_ID::PolygonMultiChunkBinaryGreedyState);
    // mAppStack.push(State_ID::RaycastSingleChunkTexturedVoxels);
    // mAppStack.push(State_ID::RaycastSingleChunkTexturedVoxelsFixedStep);
    // mAppStack.push(State_ID::RaycastSingleChunkTexturedBrickmapGpu);
    mAppStack.push(State_ID::RaycastSingleChunkTexturedOctreeGpu);
    // mAppStack.push(State_ID::RaycastMultiChunkTexturedVoxels);
    // mAppStack.push(State_ID::RaycastMultiChunkTexturedVoxelsFixedStep);
    // mAppStack.push(State_ID::RaycastMultiChunkTexturedBrickmapGpu);
    // mAppStack.push(State_ID::RaycastMultiChunkTexturedOctreeGpu);
}

void Application::initializeTracyScreenCapture()
{
#ifdef TRACY_COLLECT_THUMBNAILS
    glGenTextures(4, m_fiTexture);
    glGenFramebuffers(4, m_fiFramebuffer);
    glGenBuffers(4, m_fiPbo);
    for (int i = 0; i < 4; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_fiTexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fiFramebuffer[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fiTexture[i],
                               0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_fiPbo[i]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 320 * 180 * 4, nullptr, GL_STREAM_READ);
    }
#endif
}

void Application::toggleImGuiDisplay()
{
    mIsImGuiDisplayDisabled = !mIsImGuiDisplayDisabled;
}

void Application::initializeMinitraceProfiler()
{
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
}

void Application::run()
{
    spdlog::info("Game starts, the resolution is {}x{}", mGameWindow->getSize().x,
                 mGameWindow->getSize().y);

    initializeMinitraceProfiler();
    FPSCounter::instance().disable();
    performApplicationLoop();
    FPSCounter::instance().printOverallAverageFPS();
    FPSCounter::instance().printFPSStandardDeviation();

    mGameWindow->close();
#ifndef DISABLE_IMGUI
    ImGui::SFML::Shutdown();
#endif
    mtr_flush();
    mtr_shutdown();
}

void Application::performApplicationLoop()
{
    sf::Clock clock;
    auto frameTimeElapsed = sf::Time::Zero;
    mFixedUpdateClock.restart();
    while (isGameRunning)
    {
        MEASURE_SCOPE_CUSTOM("ApplicationLoop");
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
    MEASURE_SCOPE;
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

void Application::updateImGuiLogger()
{
    if (ImGui::Begin("Log Console"))
    {
        if (ImGui::Button("Clear"))
        {
            mImguiLog.clear();
        }

        ImGui::BeginChild("scrolling");
        std::lock_guard lock(mImguiLog.mutex);
        auto scrollToBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
        for (const auto& [message, level]: mImguiLog.entry)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGuiLog::toColor(level));
            ImGui::TextUnformatted(message.c_str());
            ImGui::PopStyleColor();
        }
        if (scrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }
    ImGui::End();
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

void Application::updateImGuiSelectScene()
{
    ImGui::Begin("Scene Selector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    auto currentStateId = mAppStack.top();
    static auto scene = [this, &currentStateId](std::string name, State_ID state_id)
    {
        if (state_id == currentStateId)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
        if (ImGui::Button(name.c_str()))
        {
            mAppStack.clear();
            mAppStack.push(state_id);
        }
        if (state_id == currentStateId)
        {
            ImGui::PopStyleColor();
        }
    };

    static auto splitLineText = [this](std::string text)
    {
        float availableWidth = ImGui::GetContentRegionAvail().x;
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        float spaceForLines = availableWidth - textSize.x;
        float lineLength = spaceForLines / 2.0f - ImGui::GetStyle().ItemSpacing.x;

        ImGui::NewLine();

        // Start of custom drawing
        float currentPos = ImGui::GetCursorPosX();
        ImGui::SetCursorPosX(currentPos + lineLength);

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::TextUnformatted("");
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x - lineLength, p.y + textSize.y / 2),
                                            ImVec2(p.x, p.y + textSize.y / 2),
                                            IM_COL32(255, 255, 255, 255));

        ImGui::SameLine();
        ImGui::Text("%s", text.c_str());

        ImGui::SameLine();
        p = ImGui::GetCursorScreenPos();
        ImGui::TextUnformatted("");
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + textSize.y / 2),
                                            ImVec2(p.x + lineLength, p.y + textSize.y / 2),
                                            IM_COL32(255, 255, 255, 255));
    };

    splitLineText("Game related");
    scene("Exit application", State_ID::ExitApplicationState);

    splitLineText("Polygons");
    scene("Single Chunk Culling", State_ID::PolygonSingleChunkCullingState);
    scene("Single Chunk Naive", State_ID::PolygonSingleChunkNaiveState);
    scene("Single Chunk Greedy", State_ID::PolygonSingleChunkGreedyState);
    scene("Single Chunk Binary Greedy", State_ID::PolygonSingleChunkBinaryGreedyState);
    scene("Single Chunk Culling GPU", State_ID::PolygonSingleChunkCullingGpuState);
    scene("Multi Chunk Culling", State_ID::PolygonMultiChunkCullingState);
    scene("Multi Chunk Naive", State_ID::PolygonMultiChunkNaiveState);
    scene("Multi Chunk Greedy", State_ID::PolygonMultiChunkGreedyState);
    scene("Multi Chunk Binary Greedy", State_ID::PolygonMultiChunkBinaryGreedyState);
    scene("Multi Chunk Culling GPU", State_ID::PolygonMultiChunkCullingGpuState);

    splitLineText("Raycast");
    scene("Single Chunk Textured", State_ID::RaycastSingleChunkTexturedVoxels);
    scene("Single Chunk Textured Fixed Step", State_ID::RaycastSingleChunkTexturedVoxelsFixedStep);
    scene("Single Chunk Textured Brickmap Gpu", State_ID::RaycastSingleChunkTexturedBrickmapGpu);
    scene("Single Chunk Textured Octree Gpu", State_ID::RaycastSingleChunkTexturedOctreeGpu);
    scene("Multi Chunk Textured", State_ID::RaycastMultiChunkTexturedVoxels);
    scene("Multi Chunk Textured Fixed Step", State_ID::RaycastMultiChunkTexturedVoxelsFixedStep);
    scene("Multi Chunk Textured Brickmap Gpu", State_ID::RaycastMultiChunkTexturedBrickmapGpu);
    scene("Multi Chunk Textured Octree Gpu", State_ID::RaycastMultiChunkTexturedOctreeGpu);
    ImGui::End();
}

void Application::calculateAndSendFPSToTracy(double deltaTime)
{
#ifdef PLOT_AVERAGE_FPS
    static bool isFirstFrame = true;
    if (not isFirstFrame) [[likely]]
    {
        static std::deque<double> frameTimes;
        static const size_t maxSamples = 100;
        static double totalTime = 0.0;

        frameTimes.push_back(deltaTime);
        totalTime += deltaTime;

        if (frameTimes.size() > maxSamples)
        {
            totalTime -= frameTimes.front();
            frameTimes.pop_front();
        }

        // Calculate the average FPS
        auto averageFPS = static_cast<int64_t>(frameTimes.size() / totalTime);
        TracyPlot("Average FPS", averageFPS);
    }
    else
    {
        isFirstFrame = false;
    }
#endif
}

void Application::displayFPS(float deltaTime)
{
    static float fpsValues[100] = {};
    static int fpsIndex = 0;
    float fps = 1.0f / deltaTime;

    fpsValues[fpsIndex] = fps;
    fpsIndex = (fpsIndex + 1) % IM_ARRAYSIZE(fpsValues);

    ImGui::Begin("Performance");
    float contentWidth = ImGui::GetContentRegionAvail().x;
    float textWidth = ImGui::CalcTextSize("FPS: 100").x;
    float centerX = (contentWidth - textWidth) * 0.5f;
    ImGui::SetCursorPosX(centerX);
    ImGui::Text("FPS: %.1f", fps);
    ImVec2 plotSize = ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y);
    ImGui::PlotLines("##FPS", fpsValues, IM_ARRAYSIZE(fpsValues), fpsIndex, nullptr, 0.0f, 120.0f,
                     plotSize);
    ImGui::End();
}

void Application::updateImGui(const sf::Time& deltaTime)
{
#ifndef DISABLE_IMGUI

    ImGui::SFML::Update(*mGameWindow, deltaTime);
    if (not mIsImGuiDisplayDisabled)
    {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                                     ImGuiDockNodeFlags_PassthruCentralNode);
        updateImGuiLogger();
        updateImGuiMiniTrace();
        updateImGuiSelectScene();
        displayFPS(deltaTime.asSeconds());
        mAppStack.updateImGui(deltaTime.asSeconds());
    }
#endif
}

void Application::processEvents()
{
    MEASURE_SCOPE;
    sf::Event event{};
    while (mGameWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            isGameRunning = false;
        }

#ifndef DISABLE_IMGUI
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
        {
            toggleImGuiDisplay();
        }
        ImGui::SFML::ProcessEvent(event);
#endif
        if (not mGameWindow->hasFocus())
        {
            return;
        }
        mAppStack.handleEvent(event);
    }
}

void Application::fixedUpdate(const sf::Time& deltaTime)
{
    MEASURE_SCOPE;
    const auto deltaTimeInSeconds = deltaTime.asSeconds();
    mAppStack.fixedUpdate(deltaTimeInSeconds);
}

void Application::update(const sf::Time& deltaTime)
{
    MEASURE_SCOPE;
    calculateAndSendFPSToTracy(deltaTime.asSeconds());
    FPSCounter::instance().addFrame(deltaTime.asSeconds());
    const auto deltaTimeInSeconds = deltaTime.asSeconds();
    Mouse::update(deltaTimeInSeconds, *mGameWindow);

    updateImGui(deltaTime);

    mAppStack.update(deltaTimeInSeconds);

    if (mAppStack.top() == State_ID::ExitApplicationState)
    {
        isGameRunning = false;
    }
}

void Application::performScreenCaptureTracyProfiler()
{
#ifdef TRACY_COLLECT_THUMBNAILS
    while (!m_fiQueue.empty())
    {
        const auto fiIdx = m_fiQueue.front();
        if (glClientWaitSync(m_fiFence[fiIdx], 0, 0) == GL_TIMEOUT_EXPIRED)
        {
            break;
        }
        glDeleteSync(m_fiFence[fiIdx]);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_fiPbo[fiIdx]);
        auto ptr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, 320 * 180 * 4, GL_MAP_READ_BIT);
        FrameImage(ptr, 320, 180, m_fiQueue.size(), true);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        m_fiQueue.erase(m_fiQueue.begin());
    }

    assert(m_fiQueue.empty() || m_fiQueue.front() != m_fiIdx);// check for buffer overrun
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fiFramebuffer[m_fiIdx]);
    glBlitFramebuffer(0, 0, mGameWindow->getSize().x, mGameWindow->getSize().y, 0, 0, 320, 180,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fiFramebuffer[m_fiIdx]);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_fiPbo[m_fiIdx]);
    glReadPixels(0, 0, 320, 180, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    m_fiFence[m_fiIdx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    m_fiQueue.emplace_back(m_fiIdx);
    m_fiIdx = (m_fiIdx + 1) % 4;
#endif
}

void Application::render()
{
    MEASURE_SCOPE_WITH_GPU;
    glClearColor(0.f, 0.f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mAppStack.draw(*mGameWindow);

#ifndef DISABLE_IMGUI
    ImGui::SFML::Render();
#endif

    performScreenCaptureTracyProfiler();
    // display to the window
    mGameWindow->display();
    FRAME_MARKER;
    GPU_COLLECT;
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
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.8f};
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