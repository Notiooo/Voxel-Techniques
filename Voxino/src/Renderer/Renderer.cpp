#include "Renderer/Renderer.h"
#include "World/Camera.h"
#include "pch.h"

namespace Voxino
{

Renderer::Renderer(sf::Window& window)
    : mWindow(window)
{
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

    const auto view = camera.view();
    const auto projection = camera.projection();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);
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

    const auto view = camera.view();
    const auto projection = camera.projection();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);
    shader.setUniform("cameraPosition", camera.cameraPosition());
    GLCall(glDrawArrays(toOpenGl(drawMode), 0, numberOfVertices));

#ifdef _DEBUG
    shader.unbind();
    va.unbind();
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

}// namespace Voxino