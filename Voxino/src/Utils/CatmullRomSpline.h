#pragma once

namespace Voxino
{


inline glm::vec3 catmullRomSpline(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
                                  const glm::vec3& p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

}// namespace Voxino