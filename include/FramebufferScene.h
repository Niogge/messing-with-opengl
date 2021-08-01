#pragma once 
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include <Memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class FramebufferScene: public IScene{
public:
    FramebufferScene();
    void init();
    void draw(Camera& c, Window& win);
private:
    std::unique_ptr<Shader> ModelShader;
    std::unique_ptr<Shader> ScreenShader;
    std::unique_ptr<Model> r2d2;
    std::unique_ptr<Model> box; 
    std::vector<glm::vec3> boxPositions;
    float accumulatorTime;
    unsigned int texColorBuffer;
    unsigned int frameBuffer;
    unsigned int quadVAO, quadVBO;
};