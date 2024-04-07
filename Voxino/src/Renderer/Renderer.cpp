#include "Renderer/Renderer.h"
#include "Core/Buffers/BufferLayout.h"
#include "World/Camera.h"
#include "pch.h"

namespace Voxino
{

Renderer::Renderer(sf::Window& window)
    : mWindow(window)
{
    initializeRaycastBuffers();
}

void Renderer::draw2D(const VertexArray& va, const IndexBuffer& ib, const Shader& shader,
                      const DrawMode& drawMode) const
{
    shader.bind();
    va.bind();
    ib.bind();

    const auto windowOrthoProjection = glm::ortho(0.f, static_cast<float>(mWindow.getSize().x), 0.f,
                                                  static_cast<float>(mWindow.getSize().y));
    shader.setUniform("windowOrthoProjection", windowOrthoProjection);

    glDisable(GL_DEPTH_TEST);
    GLCall(glDrawElements(toOpenGl(drawMode), ib.size(), GL_UNSIGNED_INT, nullptr));
    glEnable(GL_DEPTH_TEST);

#ifdef _DEBUG
    shader.unbind();
    va.unbind();
    ib.unbind();
#endif
}

void Renderer::draw3D(const VertexArray& va, const IndexBuffer& ib, const Shader& shader,
                      const Camera& camera, const DrawMode& drawMode)
{
    shader.bind();
    va.bind();
    ib.bind();

    shader.setUniform("view", camera.view());
    shader.setUniform("projection", camera.projection());
    shader.setUniform("cameraPosition", camera.cameraPosition());
    GLCall(glDrawElements(toOpenGl(drawMode), ib.size(), GL_UNSIGNED_INT, nullptr));

#ifdef _DEBUG
    shader.unbind();
    va.unbind();
    ib.unbind();
#endif
}

void Renderer::draw3D(const VertexArray& va, int numberOfVertices, const Shader& shader,
                      const Camera& camera, const DrawMode& drawMode)
{
    shader.bind();
    va.bind();

    shader.setUniform("view", camera.view());
    shader.setUniform("projection", camera.projection());
    shader.setUniform("cameraPosition", camera.cameraPosition());
    GLCall(glDrawArrays(toOpenGl(drawMode), 0, numberOfVertices));

#ifdef _DEBUG
    shader.unbind();
    va.unbind();
#endif
}


void Renderer::drawRaycast(const Shader& shader, const Camera& camera) const
{
    shader.bind();
    mRaycastVAO.bind();
    mRaycastEBO.bind();

    shader.setUniform("u_View", glm::mat4(glm::mat3(camera.view())));// eliminates translation
    shader.setUniform("u_Projection", camera.projection());
    shader.setUniform("u_CameraPosition", camera.cameraPosition());

    GLCall(glDrawElements(toOpenGl(DrawMode::Triangles), mRaycastEBO.size(), GL_UNSIGNED_INT,
                          nullptr));

#ifdef _DEBUG
    shader.unbind();
    mRaycastVAO.unbind();
    mRaycastEBO.unbind();
#endif
}

unsigned Renderer::toOpenGl(const Renderer::DrawMode& drawMode)
{
    switch (drawMode)
    {
        case DrawMode::Points: return GL_POINTS;
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::Quads: return GL_QUADS;
        default: throw std::runtime_error("This DrawMode is not supported!");
    }
}

void Renderer::initializeRaycastBuffers()
{
    float vertices[] = {
        // vertex coords   / texture coords
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,//
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,//
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,//
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f //
    };

    unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    mRaycastVBO.setBuffer(vertices, sizeof(vertices));
    mRaycastEBO.setBuffer(indices, sizeof(indices));

    mRaycastBufferLayout.push<float>(3);
    mRaycastBufferLayout.push<float>(2);

    mRaycastVAO.setBuffer(mRaycastVBO, mRaycastBufferLayout);
    mRaycastVAO.unbind();
}
}// namespace Voxino