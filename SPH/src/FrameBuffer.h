#include "Shader.h"


class FrameBuffer {
public:
    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer();
    void bind();
    void unbind();
    void resize(unsigned int width, unsigned int height);
    unsigned int getVAO();
    unsigned int getFBO();
    unsigned int getRenderBuffer();
    unsigned int getColorBuffer();
    unsigned int getDepthBuffer();
    void useShader();
private:
    unsigned int m_screenVAO,m_screenVBO,m_screenEBO;
    unsigned int m_fbo;
    unsigned int m_height, m_width;
    unsigned int m_texColBuff, m_texRenderBuff, m_depthBuffer,m_texDepthBuff;
    Shader* m_shader;
};