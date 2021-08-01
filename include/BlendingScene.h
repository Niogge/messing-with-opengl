#pragma once 
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include <Memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class BlendingScene: public IScene{
public:
    BlendingScene();
    void init();
    void draw(Camera& c, Window& win);
private:
    std::unique_ptr<Shader> ModelShader;
    std::unique_ptr<Shader> TransparentShader;
    std::unique_ptr<Shader> BlendShader;
    std::unique_ptr<Model> r2d2;
    std::unique_ptr<Model> box; 
    std::vector<glm::vec3> boxPositions;
    unsigned int transparentVAO, transparentVBO;
    unsigned int transparentTexture;
    unsigned int blendTexture;
    float accumulatorTime;
    std::vector<glm::vec3> vegetation;
};