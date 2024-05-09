#version 430 core

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform atomic_uint rayIterationCounter;

in vec2 v_TexCoord;

uniform sampler3D u_VoxelWorld;
uniform ivec3 u_WorldSize;
uniform vec3 u_CameraPosition;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_VoxelWorldPosition = vec3(0,0,0);
uniform float u_NearPlane = 0.1f;
uniform float u_FarPlane = 10000.f;
uniform sampler2DArray u_TextureArray;


int decodeTextureID(float channel, bool highPart)
{
    int value = int(channel * 255.0);
    if (highPart)
    {
        return value / 16;
    }
    else
    {
        return value % 16;
    }
}

bool isInWorldBounds(vec3 coord)
{
    return coord.x >= 0. && coord.x < u_WorldSize.x &&
           coord.y >= 0. && coord.y < u_WorldSize.y &&
           coord.z >= 0. && coord.z < u_WorldSize.z;
}

vec3 mapToTexCoord(vec3 worldCoord)
{
    return (worldCoord + 0.5f) / u_WorldSize;
}

bool sampleWorld(vec3 worldCoord, out vec4 worldData)
{
    vec3 texCoord = mapToTexCoord(worldCoord);
    worldData = texture(u_VoxelWorld, texCoord);
    return true;
}

bool intersectsWorld(vec3 rayOrigin, vec3 rayDirectionInv, out float tNear, out float tFar, out int outerSideNormalAxis)
{
    vec3 tMin = -rayOrigin                * rayDirectionInv;
    vec3 tMax = (u_WorldSize - rayOrigin) * rayDirectionInv;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    tNear = max(max(t1.x, t1.y), t1.z);
    tFar = min(min(t2.x, t2.y), t2.z);

    outerSideNormalAxis = 2;
    if (tNear == t1.x) outerSideNormalAxis = 0;
    if (tNear == t1.y) outerSideNormalAxis = 1;

    return tFar >= 0 && tFar >= tNear;
}

vec3 calculateNormal(int normalAxis, vec3 step) {
    if (normalAxis == 0)
    {
        return vec3(-step.x, 0., 0.);
    }
    if (normalAxis == 1)
    {
        return vec3(0., -step.y, 0.);
    }
    return vec3(0., 0., -step.z);
}

bool traverseWorld(vec3 rayOrigin, vec3 rayDirection, float tNear, float tFar, out vec4 color, int outerSideNormalAxis, out vec3 normal, out float distance, out int numIterations)
{
    tNear = max(0, tNear) + 0.001;
    vec3 rayStart = rayOrigin + tNear * rayDirection;
    vec3 currentVoxel = floor(rayStart);

    vec3 step = vec3(
        rayDirection.x >= 0.0 ? 1.0 : -1.0,
        rayDirection.y >= 0.0 ? 1.0 : -1.0,
        rayDirection.z >= 0.0 ? 1.0 : -1.0
    );

    vec3 distanceToNext = vec3(
        step.x > 0 ? (currentVoxel.x + 1.) - rayStart.x : rayStart.x - currentVoxel.x,
        step.y > 0 ? (currentVoxel.y + 1.) - rayStart.y : rayStart.y - currentVoxel.y,
        step.z > 0 ? (currentVoxel.z + 1.) - rayStart.z : rayStart.z - currentVoxel.z
    );

    vec3 stepDelta = abs(vec3(
        rayDirection.x != 0.0 ? 1. / rayDirection.x : tFar,
        rayDirection.y != 0.0 ? 1. / rayDirection.y : tFar,
        rayDirection.z != 0.0 ? 1. / rayDirection.z : tFar
    ));

    vec3 nearestVoxelBoundary = vec3(
        rayDirection.x != 0.0 ? stepDelta.x * distanceToNext.x : tFar,
        rayDirection.y != 0.0 ? stepDelta.y * distanceToNext.y : tFar,
        rayDirection.z != 0.0 ? stepDelta.z * distanceToNext.z : tFar
    );

    distance = 0.0;

    int normalAxis = outerSideNormalAxis;
    while (isInWorldBounds(currentVoxel))
    {
        ++numIterations;
        atomicCounterIncrement(rayIterationCounter);

        vec4 voxelData;
        if (sampleWorld(currentVoxel, voxelData))
        {
            if (voxelData.w != 0)
            {
                distance = tNear + distance;
                color = voxelData;
                normal = calculateNormal(normalAxis, step);
                return true;
            }
        }

        if (nearestVoxelBoundary.x < nearestVoxelBoundary.y && nearestVoxelBoundary.x < nearestVoxelBoundary.z)
        {
            distance = nearestVoxelBoundary.x;

            currentVoxel.x += step.x;
            nearestVoxelBoundary.x += stepDelta.x;
            normalAxis = 0;
        }
        else if (nearestVoxelBoundary.y < nearestVoxelBoundary.z)
        {
            distance = nearestVoxelBoundary.y;

            currentVoxel.y += step.y;
            nearestVoxelBoundary.y += stepDelta.y;
            normalAxis = 1;
        }
        else
        {
            distance = nearestVoxelBoundary.z;

            currentVoxel.z += step.z;
            nearestVoxelBoundary.z += stepDelta.z;
            normalAxis = 2;
        }
    }
    return false;
}

bool intersectWorld(vec3 rayOrigin, vec3 rayDirection, out vec4 voxelData, out float distance, out vec3 normal, out int numIterations)
{
    float tNear, tFar;
    int outerSideNormalAxis;
    if (!intersectsWorld(rayOrigin, 1. / rayDirection, tNear, tFar, outerSideNormalAxis))
    {
        return false;
    }

    return traverseWorld(rayOrigin, rayDirection, tNear, tFar, voxelData, outerSideNormalAxis, normal, distance, numIterations);
}

vec4 calculateColor(vec4 voxelData, vec3 rayDirection, vec3 hitPoint, vec3 hitNormal)
{
    int side = -1;
    vec2 texCoords;

    if (hitNormal.x > 0.5)
    {
        side = 3; // Right
        texCoords = hitPoint.zy;
    }
    else if (hitNormal.x < -0.5)
    {
        side = 2; // Left
        texCoords = hitPoint.zy;
    }
    else if (hitNormal.y > 0.5)
    {
        side = 1; // Top
        texCoords = hitPoint.xz;
    }
    else if (hitNormal.y < -0.5)
    {
        side = 0; // Bottom
        texCoords = hitPoint.xz;
    }
    else if (hitNormal.z > 0.5)
    {
        side = 4; // Front
        texCoords = hitPoint.xy;
    }
    else if (hitNormal.z < -0.5)
    {
        side = 5; // Back
        texCoords = hitPoint.xy;
    }
    texCoords.y = 1.0 - texCoords.y;
    texCoords = fract(texCoords);

    int textureID = decodeTextureID(voxelData[side / 2], side % 2 == 0);
    vec4 texColor = texture(u_TextureArray, vec3(texCoords, textureID));

    return texColor;
}

vec4 castRay(vec3 rayOrigin, vec3 rayDirection, out float depth)
{
    vec4 voxelData;
    float distance;
    vec3 normal;
    int numIterations = 0;
    bool hasIntersection = intersectWorld(rayOrigin, rayDirection, voxelData, distance, normal, numIterations);
    if (!hasIntersection)
    {
        discard;
    }

    vec3 hitPoint = rayOrigin + rayDirection * (distance - 0.001);

    depth = (distance - u_NearPlane) / (u_FarPlane - u_NearPlane);
    depth = clamp(depth, 0.0, 1.0);
    return calculateColor(voxelData, rayDirection, hitPoint, normal);
}

vec3 calculateRayDirection(vec2 screenCoord)
{
    vec4 clipSpacePosition = vec4(screenCoord, -1.0, 1.0);
    vec4 viewSpacePosition = inverse(u_Projection) * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverse(u_View) * viewSpacePosition;
    vec3 rayDirection = normalize(worldSpacePosition.xyz);

    return rayDirection;
}

void main() {
    vec2 screenCoord = v_TexCoord * 2.0 - 1.0;
    vec3 rayDirection = calculateRayDirection(screenCoord);
    float depth;
    vec4 color = castRay(u_CameraPosition - u_VoxelWorldPosition, rayDirection, depth);
    gl_FragDepth = depth;
    FragColor = color;
}