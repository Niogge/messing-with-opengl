#include "FramebufferScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Window.h"
#include <string>
FramebufferScene::FramebufferScene()
:ModelShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/backpack.frag")),
ScreenShader(std::make_unique<Shader>("resources/shaders/framebufferQuad.vert", "resources/shaders/framebufferQuad.frag")),
r2d2(new Model("resources/models/r2d2/R2-D2.obj")),
box(new Model("resources/models/box/box.obj")),
accumulatorTime(0.0f)
{
  
}
void FramebufferScene::init(){
   	boxPositions.push_back(glm::vec3(-5,-1.4,0));
	boxPositions.push_back(glm::vec3(-5,0.2,4.5));
	boxPositions.push_back(glm::vec3(-2.5,-1.7,-3.2));
    // configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

    
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // screen quad VAO
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    ScreenShader.get()->use();
    ScreenShader.get()->setInt("screenTexture", 0);


    glGenFramebuffers(1,&frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //we need a rendebuffer object to do depth and stencil 
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); //8 bit for stencil, 24 for depth  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 


}

void FramebufferScene::draw(Camera& c, Window& win){

///-------------------- FIRST PASS, ON THE FRAME BUFFER
    //BIND THE FRAME BUFFER
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glEnable(GL_DEPTH_TEST);

    //clear color and depth
    glClearColor(0.2, 0.3, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    accumulatorTime+= win.getDeltaTime();
    ModelShader.get()->use();
    ModelShader.get()->setV3Float("dirLight.direction", -0.2f, -1.0f, -0.3f);
    ModelShader.get()->setV3Float("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    ModelShader.get()->setV3Float("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    ModelShader.get()->setV3Float("dirLight.specular", 1.0f,0.0f, 1.0f);
    
    ModelShader.get()->setV3Float("viewPos", c.Position); 
    //Projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(c.Zoom), (float)win.getW() / (float)win.getH(), 0.1f, 100.0f);
    ModelShader.get()->setMat4("projection", projection);
    // camera/view transformation
    glm::mat4 view = c.GetViewMatrix();
    ModelShader.get()->setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    
    model = glm::rotate(model, glm::radians(20.0f)*accumulatorTime*0.5f, glm::vec3(0, 1.0f, 0));
    
    ModelShader.get()->setMat4("model", model);
    r2d2.get()->Draw(*ModelShader.get());
    int i =1;
    for (auto x : boxPositions)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, x);
        float angle = 20.0f *i;
        model = glm::rotate(model, glm::radians(angle)*accumulatorTime*0.5f, glm::vec3(1.0f, 0.3f*i, 0.5f));
        ModelShader.get()->setMat4("model", model);
        box.get()->Draw(*ModelShader.get());
        i++;
    }

///-------------------- SECOND PASS, ON THE DEFAULT BUFFER
    // then we unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //we don't want depth test since we drawing only a quad 
    glDisable(GL_DEPTH_TEST); 
    //clear the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);
    //then as usual draw a quad passing the color texture from the frame buffer as a parameter
    //Inside framebufferQuad.frag you'll find some post processing functions
    ScreenShader.get()->use();
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}