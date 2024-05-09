#pragma once
#include <cmath>
#include <numeric>
#include <spdlog/spdlog.h>
#include <vector>

class RayCounter
{
public:
    RayCounter()
        : raysToDiscard(5)
    {
    }

    void addMeasurement(unsigned long long rays)
    {
        rayCounts.push_back(rays);
    }

    void printOverallAverageIterations(std::string name = "") const
    {
        auto validRayCounts = getValidRayCounts();
        if (!validRayCounts.empty())
        {
            unsigned long long totalRays =
                std::accumulate(validRayCounts.begin(), validRayCounts.end(), 0ULL);
            auto overallAverageRays = totalRays / validRayCounts.size();
            spdlog::info("[{}] Overall Average Ray Iterations: {}", name, overallAverageRays);
        }
        else
        {
            spdlog::info("[{}] No measurements were recorded.", name);
        }
    }

    void printRayStandardDeviation(std::string name = "") const
    {
        auto validRayCounts = getValidRayCounts();
        if (validRayCounts.size() > 1)
        {
            unsigned long long totalRays =
                std::accumulate(validRayCounts.begin(), validRayCounts.end(), 0ULL);
            double mean = totalRays / static_cast<double>(validRayCounts.size());
            double variance = 0.0;

            for (const auto& rays: validRayCounts)
            {
                variance += (rays - mean) * (rays - mean);
            }
            variance /= (validRayCounts.size() - 1);// Używamy N-1 dla nieskorygowanego oszacowania

            double stdDeviation = std::sqrt(variance);
            spdlog::info("[{}] Ray Iterations Standard Deviation: {}", name, stdDeviation);
        }
        else
        {
            spdlog::info("[{}] Not enough measurements to calculate standard deviation.", name);
        }
    }

private:
    std::vector<unsigned long long> getValidRayCounts() const
    {
        if (rayCounts.size() <= raysToDiscard)
        {
            return {};
        }
        return std::vector<unsigned long long>(rayCounts.begin() + raysToDiscard, rayCounts.end());
    }


    const size_t raysToDiscard;
    std::vector<unsigned long long> rayCounts;
};
