#include "CubemapScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Window.h"
#include <string>
CubemapScene::CubemapScene()
:ModelShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/backpack.frag")),
SkyboxShader(std::make_unique<Shader>("resources/shaders/skybox.vert", "resources/shaders/skybox.frag")),
r2d2(new Model("resources/models/r2d2/R2-D2.obj")),
box(new Model("resources/models/box/box.obj")),
accumulatorTime(0.0f)
{
  
}

unsigned int loadCubemap(std::vector<std::string>& faces){
    unsigned int texId;
    glGenTextures(1,&texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

    int width, height, nrChannels;
    for(unsigned int i =0; i<faces.size(); i++){
        unsigned char*data = stbi_load(faces[i].c_str(), &width, &height,&nrChannels,STBI_rgb_alpha);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return texId;
}
void CubemapScene::init(){
   	boxPositions.push_back(glm::vec3(-5,-1.4,0));
	boxPositions.push_back(glm::vec3(-5,0.2,4.5));
	boxPositions.push_back(glm::vec3(-2.5,-1.7,-3.2));
    // configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

    std::vector<std::string> faces
    {
        "resources/textures/skyboxSpace/skybox_right.png",
        "resources/textures/skyboxSpace/skybox_left.png",
        "resources/textures/skyboxSpace/skybox_up.png",
        "resources/textures/skyboxSpace/skybox_down.png",
        "resources/textures/skyboxSpace/skybox_front.png",
        "resources/textures/skyboxSpace/skybox_back.png"
    };
    stbi_set_flip_vertically_on_load(true); 
    cubemapID = loadCubemap(faces);
    stbi_set_flip_vertically_on_load(false);
    SkyboxShader.get()->use();
    SkyboxShader.get()->setInt("skybox", 0);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void CubemapScene::draw(Camera& c, Window& win){
    //clear color and depth
    glClearColor(0.2, 0.3, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(c.Zoom), (float)win.getW() / (float)win.getH(), 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(glm::mat3(c.GetViewMatrix()));  //remove translation
    //draw skybox without depth (it's behind everything)
    glDepthMask(GL_FALSE);
    SkyboxShader.get()->use();
    SkyboxShader.get()->setMat4("projection", projection);
    SkyboxShader.get()->setMat4("view", view);
    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
    glDrawArrays(GL_TRIANGLES,0,36);
    //then set depth as usual
    glDepthMask(GL_TRUE);
    view = c.GetViewMatrix();
    accumulatorTime+= win.getDeltaTime();
    ModelShader.get()->use();
    
    //hardcoded value based on skybox light source 
    //found using the universal italian technique "A Occhio"
    ModelShader.get()->setV3Float("dirLight.direction", -0.5f, -0.2f, -0.85f); 

    ModelShader.get()->setV3Float("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    ModelShader.get()->setV3Float("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    ModelShader.get()->setV3Float("dirLight.specular", 1.0f,0.7f, 0.2f);
    ModelShader.get()->setV3Float("viewPos", c.Position); 
    //Projection matrix
   
    ModelShader.get()->setMat4("projection", projection);
    // camera/view transformation
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