#include "FrameBuffer.h"


FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) : m_width(width), m_height(height)
{
    m_shader = new Shader("src/shader/framebuffers/shader.vert", "src/shader/framebuffers/shader.frag",nullptr);
    m_shader->setInt("screenTexture", 0);
    m_shader->use();

    //Generate VAO
    float screenSquareVertices[] = {
        // positions          // texture coords
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f, // 0 top right
            1.0f, -1.0f,0.0f,   1.0f, 0.0f, // 1 bottom right
        -1.0f, -1.0f,0.0f,   0.0f, 0.0f, // 2 bottom left
        -1.0f, 1.0f, 0.0f,   0.0f, 1.0f //  3 top left
    };
    unsigned int indices[] =
    {
        0, 3, 2, // first triangle
        2, 1, 0 // second triangle
    };
    glGenVertexArrays(1, &m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    glGenBuffers(1, &m_screenEBO);
    glBindVertexArray(m_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenSquareVertices), screenSquareVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenEBO);
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_DEPTH_TEST);

    //Generate Texture Color Buffer
    glGenTextures(1, &m_texColBuff);
    glBindTexture(GL_TEXTURE_2D, m_texColBuff);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texColBuff, 0);

    //Generate Texture Depth Buffer    
    glGenTextures(1, &m_depthBuffer);
    glBindTexture(GL_TEXTURE_2D, m_depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer, 0);

    ////Generate Texture Stencil Buffer
    //glGenTextures(1, &m_texRenderBuff);
    //glBindTexture(GL_TEXTURE_2D, m_texRenderBuff);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, m_width, m_height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_INT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_texRenderBuff, 0);
   
  

    //Generate Texture Render Buffer (DEPTH + STENCIL)
   /* glGenRenderbuffers(1, &m_texRenderBuff);
    glBindRenderbuffer(GL_RENDERBUFFER, m_texRenderBuff);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL, m_width, m_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_texRenderBuff);*/

        
    //Verify that the frame buffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
       std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer()
{
    delete m_shader;
}
void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);
}
void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
}
void FrameBuffer::resize(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    //Generate Texture Color Buffer
    glBindTexture(GL_TEXTURE_2D, m_texColBuff);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texColBuff, 0);


    //Generate Texture Render Buffer (DEPTH + STENCIL)
    glBindRenderbuffer(GL_RENDERBUFFER, m_texRenderBuff);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_texRenderBuff);
}
unsigned int FrameBuffer::getVAO()
{
    return m_screenVAO;
}
unsigned int FrameBuffer::getFBO()
{
    return m_fbo;
}
unsigned int FrameBuffer::getRenderBuffer()
{
    return m_texRenderBuff;
}
unsigned int FrameBuffer::getColorBuffer()
{
    return m_texColBuff;
}
unsigned int FrameBuffer::getDepthBuffer()
{
    return m_depthBuffer;
}
void FrameBuffer::useShader()
{
    m_shader->use();
}

