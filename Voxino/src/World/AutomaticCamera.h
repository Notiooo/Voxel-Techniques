#pragma once

#include "Camera.h"
#include "Chunks/ChunkBlocks.h"
#include "Utils/Mouse.h"
#include <Utils/CatmullRomSpline.h>
#include <numbers>
#include <vector>

namespace Voxino
{
class AutomaticCamera : public Camera
{
public:
    AutomaticCamera(const WindowToRender& target)
        : Camera(target)
    {
    }

    void setRotationPath(float radius, glm::vec3 center,
                         glm::vec3 cameraOffset = glm::vec3(0.0f, 8.0f, 0.0f),
                         float heightVariation = 0.0f, float numberOfPoints = 10)
    {
        MEASURE_SCOPE;
        mPathPoints.clear();
        mLookAtPoints.clear();
        for (int i = 0; i < numberOfPoints; ++i)
        {
            float angle = 2 * std::numbers::pi * i / numberOfPoints;
            float x = center.x + cameraOffset.x + radius * std::cos(angle);
            float z = center.z + cameraOffset.z + radius * std::sin(angle);
            float y = center.y + cameraOffset.y +
                      heightVariation * std::sin(angle * 2);// Adding some height variation

            mPathPoints.push_back(glm::vec3(x, y, z));
        }
        mLookAtPoints = {center};
        extendPathForLooping();
    }

    void setAutomatic(bool isAutomatic)
    {
        mIsAutomatic = isAutomatic;
    }

    void setStaticPoint(glm::vec3 position, glm::vec3 center)
    {
        mIsStatic = true;
        mStaticPosition = position;
        mStaticLookAt = center;
    }

    void setStaticPointWithAngle(glm::vec3 center, float radius, glm::vec3 cameraOffset,
                                 float angleInDegrees)
    {
        float angleInRadians = glm::radians(angleInDegrees);
        glm::vec3 position;
        position.x = center.x + radius * std::cos(angleInRadians);
        position.z = center.z + radius * std::sin(angleInRadians);
        position.y = center.y + cameraOffset.y;

        mIsStatic = true;
        mStaticPosition = position;
        mStaticLookAt = center;
    }

    void update(const float& deltaTime)
    {
        MEASURE_SCOPE;
        if (mIsStatic)
        {
            mCameraPosition = mStaticPosition;
            mCameraFront = glm::normalize(mStaticLookAt - mStaticPosition);
            mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mCameraUp);
        }
        else if (mIsAutomatic)
        {
            updateAutomaticMovement(deltaTime);
        }
        else
        {
            Camera::update(deltaTime);
        }
    }

    void updateImGui()
    {
        ImGui::Begin("Camera");
        if (ImGui::Button(mIsAutomatic ? "Switch to Manual" : "Switch to Automatic"))
        {
            setAutomatic(!mIsAutomatic);
        }

        if (ImGui::Button(mIsStatic ? "Switch to Dynamic" : "Switch to Static"))
        {
            mIsStatic = !mIsStatic;
        }

        ImGui::End();
    }

private:
    std::vector<glm::vec3> mPathPoints;
    std::vector<glm::vec3> mLookAtPoints;

    std::vector<glm::vec3> mExtendedPathPoints;
    std::vector<glm::vec3> mExtendedLookAtPoints;

    bool mIsAutomatic = false;
    bool mIsStatic = false;
    size_t mCurrentPathPointIndex = 0;
    size_t mCurrentLookAtPointIndex = 0;
    float mTimeSinceLastPathPoint = 0.0f;
    float mTimeSinceLastLookAtPoint = 0.0f;
    float mPathTransitionDuration = 2.0f;  // Time to move from one point to another
    float mLookAtTransitionDuration = 2.0f;// Time to look at the next point

    glm::vec3 mStaticPosition;
    glm::vec3 mStaticLookAt;

    void extendLookAtPointsForMinimumSize()
    {
        while (mLookAtPoints.size() < 3)
        {
            if (mLookAtPoints.empty())
            {
                // If no points are available, add a default point (0, 0, 0)
                mLookAtPoints.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            }
            else
            {
                // If points are available, repeat the last point
                mLookAtPoints.push_back(mLookAtPoints.back());
            }
        }
    }

    void extendPathForLooping()
    {
        extendLookAtPointsForMinimumSize();
        mExtendedPathPoints = mPathPoints;
        mExtendedLookAtPoints = mLookAtPoints;

        // Add the first three points to the end of the list for smooth looping
        for (int i = 0; i < 3; ++i)
        {
            mExtendedPathPoints.push_back(mPathPoints[i]);
            mExtendedLookAtPoints.push_back(mLookAtPoints[i]);
        }
    }

    void updateAutomaticMovement(const float& deltaTime)
    {
        if (mExtendedPathPoints.size() < 4 || mExtendedLookAtPoints.size() < 4)
        {
            return;
        }

        // Update path movement
        mTimeSinceLastPathPoint += deltaTime;

        if (mTimeSinceLastPathPoint > mPathTransitionDuration)
        {
            mTimeSinceLastPathPoint = 0.0f;
            mCurrentPathPointIndex =
                (mCurrentPathPointIndex + 1) % (mExtendedPathPoints.size() - 3);
        }

        float tPath = mTimeSinceLastPathPoint / mPathTransitionDuration;
        glm::vec3 newPosition =
            catmullRomSpline(mExtendedPathPoints[mCurrentPathPointIndex],
                             mExtendedPathPoints[mCurrentPathPointIndex + 1],
                             mExtendedPathPoints[mCurrentPathPointIndex + 2],
                             mExtendedPathPoints[mCurrentPathPointIndex + 3], tPath);

        // Update look-at point
        mTimeSinceLastLookAtPoint += deltaTime;

        if (mTimeSinceLastLookAtPoint > mLookAtTransitionDuration)
        {
            mTimeSinceLastLookAtPoint = 0.0f;
            mCurrentLookAtPointIndex =
                (mCurrentLookAtPointIndex + 1) % (mExtendedLookAtPoints.size() - 3);
        }

        float tLookAt = mTimeSinceLastLookAtPoint / mLookAtTransitionDuration;
        glm::vec3 newLookAt =
            catmullRomSpline(mExtendedLookAtPoints[mCurrentLookAtPointIndex],
                             mExtendedLookAtPoints[mCurrentLookAtPointIndex + 1],
                             mExtendedLookAtPoints[mCurrentLookAtPointIndex + 2],
                             mExtendedLookAtPoints[mCurrentLookAtPointIndex + 3], tLookAt);

        mCameraPosition = newPosition;
        mCameraFront = glm::normalize(newLookAt - newPosition);
        mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mCameraUp);
    }
};

}// namespace Voxino
