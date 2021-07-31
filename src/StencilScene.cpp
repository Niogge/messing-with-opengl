#include "StencilScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Window.h"
#include <string>
StencilScene::StencilScene()
:ModelShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/backpack.frag")),
OutlineShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/outline.frag")),
r2d2(new Model("resources/models/r2d2/R2-D2.obj")),
box(new Model("resources/models/box/box.obj")),
accumulatorTime(0.0f)
{
  
}
void StencilScene::init(){
   	boxPositions.push_back(glm::vec3(-5,-1.4,0));
	boxPositions.push_back(glm::vec3(-5,0.2,4.5));
	boxPositions.push_back(glm::vec3(-2.5,-1.7,-3.2));
    	
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

}

void StencilScene::draw(Camera& c, Window& win){
    //clear color and depth
    glClearColor(0.2, 0.3, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    

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
   

  
    int i =1;
    std::vector<glm::mat4> boxModelMatrices;


    glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
    glStencilMask(0xFF); // stencil for boxes
    for (auto x : boxPositions)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, x);
        float angle = 20.0f *i;
        model = glm::rotate(model, glm::radians(angle)*accumulatorTime*0.5f, glm::vec3(1.0f, 0.3f*i, 0.5f));
        boxModelMatrices.push_back(model);
        ModelShader.get()->setMat4("model", model);
        box.get()->Draw(*ModelShader.get());
        i++;
    }
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); 
    glDisable(GL_DEPTH_TEST);
    OutlineShader.get()->use(); 
    OutlineShader.get()->setMat4("projection", projection);
    OutlineShader.get()->setMat4("view", view);
    for (auto m : boxModelMatrices)
    {
        m =glm::scale(m, glm::vec3(1.2f, 1.2f, 1.2f)); //should reposition the model, or use a box with centered pivot
        OutlineShader.get()->setMat4("model", m);
        box.get()->Draw(*OutlineShader.get());
    }
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);   
    glEnable(GL_DEPTH_TEST);  
    ModelShader.get()->use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    
    model = glm::rotate(model, glm::radians(20.0f)*accumulatorTime*0.5f, glm::vec3(0, 1.0f, 0));
    
    ModelShader.get()->setMat4("model", model);
    r2d2.get()->Draw(*ModelShader.get());
}