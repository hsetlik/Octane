/*
  ==============================================================================

    WaveDisplay.h
    Created: 4 May 2021 1:31:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ModulationSystem.h"
#include "RgbColor.h"
#include "OpenGLUtil/OpenGLUtil.hpp"

class WaveGraphOpenGL :
public juce::Component,
public juce::OpenGLRenderer,
private juce::AsyncUpdater
{
public:
    //GL callbacks
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;
private:
    //GL vars
    /** Attempts to compile the OpenGL program at runtime and setup OpenGL variables. */
    void compileOpenGLShaderProgram();
    
    juce::Matrix3D<GLfloat> calculateProjectionMatrix() const;
    juce::Matrix3D<GLfloat> calculateViewMatrix() const;

    // OpenGL Variables
    juce::OpenGLContext openGLContext;
    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;
    
    OpenGLUtil::UniformWrapper projectionMatrix { "projectionMatrix" };
    OpenGLUtil::UniformWrapper viewMatrix {"viewMatrix" };
    
    GLuint VAO, VBO;
    std::vector<juce::Vector3D<GLfloat>> vertices;
    
    
};
