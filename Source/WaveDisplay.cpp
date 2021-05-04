/*
  ==============================================================================

    WaveDisplay.cpp
    Created: 4 May 2021 1:31:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WaveDisplay.h"
WaveGraphOpenGL::WaveGraphOpenGL(std::vector<std::vector<float>>& graphData, SynthParam* p) :
rawData(graphData),
positionParam(p)
{
    openGLContext.setOpenGLVersionRequired (juce::OpenGLContext::OpenGLVersion::openGL3_2);
    
    openGLContext.setRenderer (this);
    openGLContext.attachTo (*this);
    openGLContext.setContinuousRepainting (true); // Enable rendering
}

WaveGraphOpenGL::~WaveGraphOpenGL()
{
    openGLContext.setContinuousRepainting (false);
    openGLContext.detach();
}

std::vector<juce::Vector3D<GLfloat>> WaveGraphOpenGL::generateFromGraph()
{
    std::vector<juce::Vector3D<GLfloat>> vec;
    int idx = 0;
    auto length = (int)rawData.size();
    int dZ = 1.0f / length;
    for(auto frame : rawData)
    {
        auto dX = 1.0f / frame.size();
        auto currentZ = (dZ * idx) - 0.5f;
        vec.push_back(juce::Vector3D<GLfloat>(-0.5f, -0.5f, currentZ));
        int pointIdx = 0;
        for(auto point : frame)
        {
            auto x = pointIdx * dX - 0.5f;
            auto y = point / 2.0f;
            vec.push_back(juce::Vector3D<GLfloat>(x, y, currentZ));
            ++pointIdx;
        }
        ++idx;
        vec.push_back(juce::Vector3D<GLfloat>(0.5f, -0.5f, currentZ));
        vec.push_back(juce::Vector3D<GLfloat>(-0.5f, -0.5f, currentZ));
    }
    return vec;
}

//====================================== GL callbacks ===========================
void WaveGraphOpenGL::newOpenGLContextCreated()
{
    compileOpenGLShaderProgram();
    
    vertices = generateFromGraph();
    
    // Generate opengl vertex objects ==========================================
    openGLContext.extensions.glGenVertexArrays(1, &VAO); // Vertex Array Object
    openGLContext.extensions.glGenBuffers (1, &VBO);     // Vertex Buffer Object
    
    openGLContext.extensions.glBindVertexArray (VAO);
    
    // Fill VBO buffer with vertices array
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, VBO);
    openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                           sizeof (GLfloat) * vertices.size() * 3,
                                           vertices.data(),
                                           GL_STATIC_DRAW);


    // Define that our vertices are laid out as groups of 3 GLfloats
    openGLContext.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,
                                                    3 * sizeof (GLfloat), NULL);
    openGLContext.extensions.glEnableVertexAttribArray (0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show wireframe
}
void WaveGraphOpenGL::openGLContextClosing()
{
    
}

void WaveGraphOpenGL::renderOpenGL()
{
    jassert (juce::OpenGLHelpers::isContextActive());
    
    // Scale viewport
    const float renderingScale = (float) openGLContext.getRenderingScale();
    glViewport (0, 0, (int)renderingScale * getWidth(), (int)renderingScale * getHeight());

    // Set background color
    juce::OpenGLHelpers::clear(UXPalette::darkBkgnd);
    // Select shader program
    shaderProgram->use();
    // Setup the Uniforms for use in the Shader
    if (projectionMatrix)
        projectionMatrix->setMatrix4 (calculateProjectionMatrix().mat, 1, false);
    if (viewMatrix)
        viewMatrix->setMatrix4 (calculateViewMatrix().mat, 1, false);
    
    // Draw Vertices
    openGLContext.extensions.glBindVertexArray (VAO);
    glDrawArrays (GL_TRIANGLES, 0, (int) vertices.size());
    openGLContext.extensions.glBindVertexArray (0);
}

void WaveGraphOpenGL::compileOpenGLShaderProgram()
{
    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgramAttempt
        = std::make_unique<juce::OpenGLShaderProgram> (openGLContext);

    // Attempt to compile the program
    if (shaderProgramAttempt->addVertexShader ({ BinaryData::BasicVertex_glsl })
        && shaderProgramAttempt->addFragmentShader ({ BinaryData::BasicFragment_glsl })
        && shaderProgramAttempt->link())
    {
        projectionMatrix.disconnectFromShaderProgram();
        viewMatrix.disconnectFromShaderProgram();
        
        shaderProgram.reset (shaderProgramAttempt.release());
        
        projectionMatrix.connectToShaderProgram (openGLContext, *shaderProgram);
        viewMatrix.connectToShaderProgram (openGLContext, *shaderProgram);
        
    }
}

juce::Matrix3D<GLfloat> WaveGraphOpenGL::calculateProjectionMatrix() const
{
    float w = 1.0f / 0.6f;
    float h = w * getLocalBounds().toFloat().getAspectRatio (false);
    
    return juce::Matrix3D<GLfloat>::fromFrustum(-w, w, -h, h, 4.0f, 20.0f);
}


juce::Matrix3D<GLfloat> WaveGraphOpenGL::calculateViewMatrix() const
{
    float scaleFactor = 2.0f;
    auto scale = juce::Matrix3D<GLfloat>(juce::AffineTransform::scale(scaleFactor * 3.0f, scaleFactor));
    auto angleX = juce::MathConstants<float>::pi * 0.0f;
    auto angleY = juce::MathConstants<float>::pi * 0.0f;
    auto angleZ = juce::MathConstants<float>::pi * 0.0f;
    auto rotation = juce::Matrix3D<GLfloat>::rotation(juce::Vector3D<GLfloat>(angleX / scaleFactor,
                                                                              angleY / scaleFactor,
                                                                              angleZ / scaleFactor));
    juce::Matrix3D<GLfloat> translate (juce::Vector3D<GLfloat> (0.0f, 0.0f, -10.0f));
    auto out = scale * rotation * translate;
    printf("\nNEW MATRIX\n");
    for(int i = 0; i < 16; ++i)
    {
        printf("Matrix value %d: %f\n", i, out.mat[i]);
    }
    return out;
}

//===================================== Component stuff ============================
void WaveGraphOpenGL::resized()
{
    
}

void WaveGraphOpenGL::paint(juce::Graphics &g)
{
    
}

void WaveGraphOpenGL::handleAsyncUpdate()
{
    
}

