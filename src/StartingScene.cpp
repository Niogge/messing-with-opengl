#include "StartingScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Window.h"
#include <string>
StartingScene::StartingScene()
:ModelShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/backpack.frag")),
r2d2(new Model("resources/models/r2d2/R2-D2.obj")),
box(new Model("resources/models/box/box.obj")),
accumulatorTime(0.0f)
{
  
}
void StartingScene::init(){
   	boxPositions.push_back(glm::vec3(-5,-1.4,0));
	boxPositions.push_back(glm::vec3(-5,0.2,4.5));
	boxPositions.push_back(glm::vec3(-2.5,-1.7,-3.2));
}

void StartingScene::draw(Camera& c, Window& win){
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
}