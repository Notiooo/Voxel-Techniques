#version 460 core

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform atomic_uint rayIterationCounter;

in vec2 v_TexCoord;
uniform ivec3 u_WorldSize;
uniform vec3 u_CameraPosition;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_VoxelWorldPosition = vec3(0,0,0);
uniform float u_NearPlane = 0.1f;
uniform float u_FarPlane = 10000.f;

uniform sampler2DArray u_TextureArray;

struct OctreeNode {
    int children[8];
    uvec4 data;
};

layout(std430, binding = 1) buffer OctreeBuffer {
    OctreeNode nodes[];
};

int decodeTextureID(int value, bool highPart)
{
    if(!highPart)
    {
        return value & 0x0F;
    }
    return (value >> 4) & 0x0F;
}

vec4 calculateColor(uvec4 voxelData, vec3 rayDirection, vec3 hitPoint, vec3 hitNormal)
{
    int side = -1;
    vec2 texCoords;

    if (hitNormal.x > 0.5) { side = 3; texCoords = hitPoint.zy; }
    else if (hitNormal.x < -0.5) { side = 2; texCoords = hitPoint.zy; }
    else if (hitNormal.y > 0.5) { side = 1; texCoords = hitPoint.xz; }
    else if (hitNormal.y < -0.5) { side = 0; texCoords = hitPoint.xz; }
    else if (hitNormal.z > 0.5) { side = 4; texCoords = hitPoint.xy; }
    else if (hitNormal.z < -0.5) { side = 5; texCoords = hitPoint.xy; }

    texCoords.y = 1.0 - texCoords.y;
    texCoords = fract(texCoords);
    int value = int(voxelData[side / 2]);
    int textureID = decodeTextureID(value, side % 2 == 0);
    return texture(u_TextureArray, vec3(texCoords, textureID));
}

OctreeNode getOctreeNode(int index) {
    return nodes[index];
}

bool intersectAABB(vec3 rayOrigin, vec3 rayDirection, vec3 m_min, vec3 m_max, out float tNear, out float tFar) {
    vec3 invDir = 1.0 / rayDirection;
    vec3 tMin = (m_min - rayOrigin) * invDir;
    vec3 tMax = (m_max - rayOrigin) * invDir;

    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    tNear = max(max(t1.x, t1.y), t1.z);
    tFar = min(min(t2.x, t2.y), t2.z);
    return tFar >= tNear && tFar >= 0.0;
}

vec3 calculateNormalForFixedStep(vec3 currentPos)
{
    vec3 offset = fract(currentPos) - 0.5;
    vec3 absOffset = abs(offset);
    if (absOffset.x > absOffset.y && absOffset.x > absOffset.z)
        return vec3(sign(offset.x), 0.0, 0.0);
    else if (absOffset.y > absOffset.x && absOffset.y > absOffset.z)
        return vec3(0.0, sign(offset.y), 0.0);
    else
        return vec3(0.0, 0.0, sign(offset.z));
}

bool traverseOctreeIterative(vec3 rayOrigin, vec3 rayDirection, out uvec4 voxelData, out float distance, out vec3 normal, out int numIterations) {
    struct StackEntry {
        vec3 min;
        vec3 max;
        int nodeIndex;
        float tMin;
    };

    const int MAX_STACK_SIZE = 64;
    StackEntry stack[MAX_STACK_SIZE];
    int stackSize = 0;

    vec3 nodeMin = vec3(0.0);
    vec3 nodeMax = vec3(u_WorldSize);
    float tMin, tMax;

    if (!intersectAABB(rayOrigin, rayDirection, nodeMin, nodeMax, tMin, tMax)) {
        return false;
    }

    tMin = max(0.0, tMin) + 0.001;

    stack[stackSize++] = StackEntry(nodeMin, nodeMax, 0, tMin);

    while (stackSize > 0) {
        StackEntry entry = stack[--stackSize];
        numIterations++;
        atomicCounterIncrement(rayIterationCounter);

        OctreeNode node = getOctreeNode(entry.nodeIndex);

        if (node.data.a != 0.0) {
            voxelData = node.data;
            vec3 hitPoint = rayOrigin + rayDirection * entry.tMin;
            distance = entry.tMin;
            normal = calculateNormalForFixedStep(hitPoint);
            return true;
        }

        vec3 nodeCenter = (entry.min + entry.max) * 0.5;

        struct ChildInfo {
            int index;
            float tMin;
        };

        ChildInfo childInfos[8];
        int childCount = 0;

        for (int i = 0; i < 8; ++i) {
            int x = (i & 1) == 0 ? 0 : 1;
            int y = (i & 2) == 0 ? 0 : 1;
            int z = (i & 4) == 0 ? 0 : 1;
            vec3 corner = vec3(float(x), float(y), float(z));
            vec3 childMin = mix(entry.min, nodeCenter, corner);
            vec3 childMax = mix(nodeCenter, entry.max, corner);

            if (intersectAABB(rayOrigin, rayDirection, childMin, childMax, tMin, tMax)) {
                tMin = max(0.0, tMin) + 0.001;
                childInfos[childCount++] = ChildInfo(i, tMin);
            }
        }

        for (int i = 0; i < childCount; ++i) {
            for (int j = i + 1; j < childCount; ++j) {
                if (childInfos[i].tMin > childInfos[j].tMin) {
                    ChildInfo temp = childInfos[i];
                    childInfos[i] = childInfos[j];
                    childInfos[j] = temp;
                }
            }
        }

        for (int i = childCount - 1; i >= 0; --i) {
            int childIndex = childInfos[i].index;
            if (node.children[childIndex] != -1 && stackSize < MAX_STACK_SIZE) {
                int x = (childIndex & 1) == 0 ? 0 : 1;
                int y = (childIndex & 2) == 0 ? 0 : 1;
                int z = (childIndex & 4) == 0 ? 0 : 1;
                vec3 corner = vec3(float(x), float(y), float(z));
                vec3 childMin = mix(entry.min, nodeCenter, corner);
                vec3 childMax = mix(nodeCenter, entry.max, corner);

                stack[stackSize++] = StackEntry(childMin, childMax, node.children[childIndex], childInfos[i].tMin);
            }
        }
    }

    return false;
}

vec4 castRay(vec3 rayOrigin, vec3 rayDirection, out float depth) {
    uvec4 voxelData;
    float distance;
    vec3 normal;
    int numIterations = 0;
    bool hasIntersection = traverseOctreeIterative(rayOrigin, rayDirection, voxelData, distance, normal, numIterations);
    if (!hasIntersection) {
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
    return normalize((inverse(u_View) * viewSpacePosition).xyz);
}

void main() {
    vec2 screenCoord = v_TexCoord * 2.0 - 1.0;
    vec3 rayDirection = calculateRayDirection(screenCoord);
    float depth;
    vec4 color = castRay(u_CameraPosition - u_VoxelWorldPosition, rayDirection, depth);
    gl_FragDepth = depth;
    FragColor = color;
}
