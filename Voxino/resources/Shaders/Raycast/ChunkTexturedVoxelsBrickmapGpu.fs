#version 430 core

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform atomic_uint rayIterationCounter;

in vec2 v_TexCoord;

uniform vec3 u_CameraPosition;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float u_CameraRadius = 5.f;
uniform bool u_VisibilitySphereEnabled = true;
uniform bool u_DrawRayIterations = false;
uniform float u_IntensityMultiplier = 0.01;
uniform int u_BrickSize = 8;
uniform int u_GridSize = 2;

uniform sampler2DArray u_TextureArray;
uniform sampler3D u_Brickmaps;
uniform isamplerBuffer u_BrickmapOccupancy;
uniform ivec3 u_WorldSize;

bool isBrickmapOccupied(int brickIndex)
{
//     uint byteIndex = uint(brickIndex) / 8u;
//     uint bitIndex = uint(brickIndex) % 8u;
//     uint value = texelFetch(u_BrickmapOccupancy, int(byteIndex)).r;
//     return (value & (1u << bitIndex)) != 0u;
       return texelFetch(u_BrickmapOccupancy, brickIndex).r != 0;
}

///////// Constants //////////
const vec3 LIGHT_POSITION = vec3(-100, 200, 8);
const float SHADOW_SHADE = 0.3;

vec3 calculateGlobalTexCoord(vec3 worldCoord)
{
    // Calculate normalized coordinates in the entire texture
    return vec3(worldCoord) / float(u_BrickSize * u_GridSize);
}

int calculateBrickIndexFromWorldCoord(vec3 worldCoord)
{
    ivec3 gridCoord = ivec3(worldCoord) / u_BrickSize;
    return gridCoord.x * u_GridSize * u_GridSize + gridCoord.y * u_GridSize + gridCoord.z;
}

vec3 calculateLocalCoord(vec3 worldCoord)
{
    ivec3 localCoord = ivec3(worldCoord) % u_BrickSize;
    return vec3(localCoord) / float(u_BrickSize);  // Normalize local coordinates
}

int decodeTextureID(float channel, bool highPart)
{
    int value = int(channel * 255.0); // Scale channel value to 8 bits
    if (highPart)
    {
        return value / 16; // Get the high part
    }
    else
    {
        return value % 16; // Get the low part
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
    // Derived from rescale:
    // worldCoord                     = coord * worldSize - 0.5
    // worldCoord + 0.5               = coord * worldSize
    // (worldCoord + 0.5) / worldSize = coord
    return (worldCoord + 0.5f) / u_WorldSize;
}

bool sampleWorld(vec3 worldCoord, out vec4 worldData)
{
    if (!isInWorldBounds(worldCoord))
    {
        return false;
    }

    vec3 globalTexCoord = mapToTexCoord(worldCoord);
    worldData = texture(u_Brickmaps, globalTexCoord);
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

void calculateRegionExit(inout vec3 currentVoxel, vec3 rayStart, vec3 rayDirection, inout vec3 nearestVoxelBoundary, vec3 step, vec3 stepDelta, inout float distance, out vec3 exitPoint)
{
    // Calculate the bounds of the region based on entry voxel and region size
    vec3 regionSize = vec3(u_BrickSize, u_BrickSize, u_BrickSize); // Assuming the bigger region is 8x8x8
    vec3 regionStart = floor(currentVoxel / regionSize) * regionSize;
    vec3 regionEnd = regionStart + regionSize;

    // Calculate the exit point for each axis
    vec3 exitT;
    exitT.x = step.x > 0.0 ? (regionEnd.x - rayStart.x) * stepDelta.x : (regionStart.x - rayStart.x) * stepDelta.x;
    exitT.y = step.y > 0.0 ? (regionEnd.y - rayStart.y) * stepDelta.y : (regionStart.y - rayStart.y) * stepDelta.y;
    exitT.z = step.z > 0.0 ? (regionEnd.z - rayStart.z) * stepDelta.z : (regionStart.z - rayStart.z) * stepDelta.z;


    // Find the smallest t value to exit the region
    float tExit = min(exitT.x, min(exitT.y, exitT.z));
    exitPoint = rayStart + rayDirection * tExit;
    currentVoxel = floor(exitPoint);
    distance += tExit; // Update distance to include traversal through the larger region

    // Update the nearestVoxelBoundary for continuing traversal
    nearestVoxelBoundary = stepDelta * (ceil(exitPoint) - exitPoint) * step;
}

void updateTraversal(inout vec3 currentVoxel, inout vec3 nearestVoxelBoundary, inout float distance, inout int normalAxis, vec3 step, vec3 stepDelta)
{
    if (nearestVoxelBoundary.x < nearestVoxelBoundary.y && nearestVoxelBoundary.x < nearestVoxelBoundary.z)
    {
        distance = nearestVoxelBoundary.x;

        // X-axis traversal
        currentVoxel.x += step.x;
        nearestVoxelBoundary.x += stepDelta.x;
        normalAxis = 0;
    }
    else if (nearestVoxelBoundary.y < nearestVoxelBoundary.z)
    {
        distance = nearestVoxelBoundary.y;

        // Y-axis traversal
        currentVoxel.y += step.y;
        nearestVoxelBoundary.y += stepDelta.y;
        normalAxis = 1;
    }
    else
    {
        distance = nearestVoxelBoundary.z;

        // Z-axis traversal
        currentVoxel.z += step.z;
        nearestVoxelBoundary.z += stepDelta.z;
        normalAxis = 2;
    }
}

// Implementation based on "A Fast Voxel Traversal Algorithm for Ray Tracing"
// by John Amanatides and Andrew Woo.
// Paper: http://www.cse.yorku.ca/~amana/research/grid.pdf
bool traverseWorld(vec3 rayOrigin, vec3 rayDirection, float tNear, float tFar, out vec4 color, int outerSideNormalAxis, out vec3 normal, out float distance, out int numIterations)
{
    // Increase tNear by small amount to start *inside* the world
    // If we are inside the bounding box of the world, start at the ray origin
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

    // How far along the ray we must travel for such movement to equal the cell size (1)
    vec3 stepDelta = abs(vec3(
        rayDirection.x != 0.0 ? 1. / rayDirection.x : tFar,
        rayDirection.y != 0.0 ? 1. / rayDirection.y : tFar,
        rayDirection.z != 0.0 ? 1. / rayDirection.z : tFar
    ));

    // How far along the ray we must travel to cross the x, y or z grid line
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

        int brickIndex = calculateBrickIndexFromWorldCoord(currentVoxel);
        if (!isBrickmapOccupied(brickIndex))
        {
           //skipEmptyBrick(currentVoxel, step, nearestVoxelBoundary, stepDelta, distance);
           while(isInWorldBounds(currentVoxel) && !isBrickmapOccupied(brickIndex))
           {
              updateTraversal(currentVoxel, nearestVoxelBoundary, distance, normalAxis, step, stepDelta);
              brickIndex = calculateBrickIndexFromWorldCoord(currentVoxel);
           }
           continue;
        }
//         if (!isBrickmapOccupied(brickIndex))
//         {
//             vec3 exitPoint;
//             calculateRegionExit(currentVoxel, rayStart, rayDirection, nearestVoxelBoundary, step, stepDelta, distance, exitPoint);
//             currentVoxel = exitPoint;
//             continue; // Skip the traversal inside this region
//         }

        // Sample world and return if data.z == 0
        vec4 voxelData;
        if (sampleWorld(currentVoxel, voxelData))
        {
            if(u_VisibilitySphereEnabled)
            {
                const float voxelBoundingRadius = sqrt(3.0) * 0.5;
                vec3 voxelCenter = currentVoxel + vec3(0.5, 0.5, 0.5);
                vec3 centerDirection = voxelCenter - rayOrigin;
                float centerDistance = length(centerDirection);

                if ((centerDistance + voxelBoundingRadius) > u_CameraRadius && voxelData.w != 0)
                {
                    distance = tNear + distance;
                    color = voxelData;
                    normal = calculateNormal(normalAxis, step);
                    return true;
                }
            }
            else if (voxelData.w != 0)
            {
                distance = tNear + distance;
                color = voxelData;
                normal = calculateNormal(normalAxis, step);
                return true;
            }
        }
        updateTraversal(currentVoxel, nearestVoxelBoundary, distance, normalAxis, step, stepDelta);
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

vec4 calculateDiffuseMultiplier(vec3 lightVector, vec3 normal)
{
    float lightInclination = dot(normal, lightVector);

    float shadeFactor = max(SHADOW_SHADE, lightInclination);
    return vec4(shadeFactor, shadeFactor, shadeFactor, 1.);
}

vec4 calculateShadingMultiplier(vec3 hitPoint, vec3 normal, vec3 rayDirection) {
    vec3 lightVector = normalize(LIGHT_POSITION - hitPoint);

    int numIterations = 0;
    vec4 voxelData2;
    float distance2;
    vec3 normal2;
    bool hasIntersection2 = intersectWorld(hitPoint, lightVector, voxelData2, distance2, normal2, numIterations);
    if (hasIntersection2)
    {
        return vec4(SHADOW_SHADE, SHADOW_SHADE, SHADOW_SHADE, 1.);
    }
    return calculateDiffuseMultiplier(lightVector, normal);
}

vec4 calculateColor(vec4 voxelData, vec3 rayDirection, vec3 hitPoint, vec3 hitNormal)
{
    vec4 shadingMultiplier = calculateShadingMultiplier(hitPoint, hitNormal, rayDirection);
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
    texCoords.y = 1.0 - texCoords.y; // flips upside down
    texCoords = fract(texCoords);

    int textureID = decodeTextureID(voxelData[side / 2], side % 2 == 0);
    vec4 texColor = texture(u_TextureArray, vec3(texCoords, textureID));

    return shadingMultiplier * texColor;
}

vec4 castRay(vec3 rayOrigin, vec3 rayDirection)
{
    vec4 voxelData;
    float distance;
    vec3 normal;
    int numIterations = 0;
    bool hasIntersection = intersectWorld(rayOrigin, rayDirection, voxelData, distance, normal, numIterations);
    if (u_DrawRayIterations)
    {
        float normalizedIterations = clamp(float(numIterations) / 50.0, 0.0, 1.0);
        vec3 colorGradient = mix(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), normalizedIterations);
        return vec4(colorGradient, 1.0);
    }
    else if (!hasIntersection)
    {
        discard;
    }

    vec3 hitPoint = rayOrigin + rayDirection * (distance - 0.001);
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
    vec4 color = castRay(u_CameraPosition, rayDirection);
    FragColor = color;
}