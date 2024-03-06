#pragma once

inline void initializeOpenGL()
{
    static sf::Context context;

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }
}