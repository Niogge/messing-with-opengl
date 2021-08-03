#pragma once 
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include <Memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class CubemapScene: public IScene{
public:
    CubemapScene();
    void init();
    void draw(Camera& c, Window& win);
private:
    std::unique_ptr<Shader> ModelShader;
    std::unique_ptr<Shader> SkyboxShader;
    std::unique_ptr<Model> r2d2;
    std::unique_ptr<Model> box; 
    std::vector<glm::vec3> boxPositions;
    float accumulatorTime;
    unsigned int cubemapID;
    unsigned int skyboxVAO, skyboxVBO;
};