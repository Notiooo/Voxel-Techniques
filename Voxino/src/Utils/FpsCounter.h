#pragma once
#include <cmath>
#include <numeric>
#include <spdlog/spdlog.h>
#include <vector>

class FPSCounter
{
public:
    /**
     * Returns an instance of the FPSCounter
     * @return Instance of the FPSCounter
     */
    static FPSCounter& instance()
    {
        static FPSCounter instance;
        return instance;
    }

    /**
     * Adds a frame time sample if the counter is enabled.
     * @param deltaTime Time for the current frame
     */
    void addFrame(double deltaTime)
    {
        if (!enabled)
        {
            return;
        }

        totalFrames++;
        frameTimes.push_back(deltaTime);
    }

    /**
     * Enables the FPS counter
     */
    void enable()
    {
        enabled = true;
    }

    /**
     * Disables the FPS counter
     */
    void disable()
    {
        enabled = false;
    }

    /**
     * Prints the overall average FPS
     */
    void printOverallAverageFPS() const
    {
        if (totalFrames > framesToDiscard + framesToDiscardEnd)
        {
            auto validFrameTimes = getValidFrameTimes();
            double sumOfFrameTimes =
                std::accumulate(validFrameTimes.begin(), validFrameTimes.end(), 0.0);
            double overallAverageFPS = validFrameTimes.size() / sumOfFrameTimes;
            spdlog::info("Overall Average FPS: {}", overallAverageFPS);
        }
        else
        {
            spdlog::info("Not enough frames to calculate average FPS.");
        }
    }

    /**
     * Prints the standard deviation of the FPS
     */
    void printFPSStandardDeviation() const
    {
        if (totalFrames > framesToDiscard + framesToDiscardEnd + 1)
        {
            auto validFrameTimes = getValidFrameTimes();
            std::vector<double> fpsValues;
            fpsValues.reserve(validFrameTimes.size());
            for (double frameTime: validFrameTimes)
            {
                fpsValues.push_back(1.0 / frameTime);
            }

            double meanFPS =
                std::accumulate(fpsValues.begin(), fpsValues.end(), 0.0) / fpsValues.size();

            double variance = 0.0;
            for (double fps: fpsValues)
            {
                variance += (fps - meanFPS) * (fps - meanFPS);
            }
            variance /= (fpsValues.size() - 1);// Use N-1 for an unbiased estimate

            double stdDeviationFPS = std::sqrt(variance);
            spdlog::info("FPS Standard Deviation: {}", stdDeviationFPS);
        }
        else
        {
            spdlog::info("Not enough frames to calculate standard deviation.");
        }
    }

private:
    FPSCounter()
        : totalFrames(0)
        , enabled(true)
        , framesToDiscard(40)
        , framesToDiscardEnd(20)
    {
    }
    ~FPSCounter() = default;

    // Deleting copy constructor and assignment operator to enforce singleton pattern
    FPSCounter(const FPSCounter&) = delete;
    FPSCounter& operator=(const FPSCounter&) = delete;

    std::vector<double> getValidFrameTimes() const
    {
        if (frameTimes.size() <= framesToDiscard + framesToDiscardEnd)
        {
            return {};
        }
        return std::vector<double>(frameTimes.begin() + framesToDiscard,
                                   frameTimes.end() - framesToDiscardEnd);
    }

    int64_t totalFrames;
    bool enabled;
    const int64_t framesToDiscard;
    const int64_t framesToDiscardEnd;
    std::vector<double> frameTimes;
};
