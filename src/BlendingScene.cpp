#include "BlendingScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Window.h"
#include <string>
BlendingScene::BlendingScene()
:ModelShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/backpack.frag")),
TransparentShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/alpha.frag")),
BlendShader(std::make_unique<Shader>("resources/shaders/backpack.vert", "resources/shaders/alphablending.frag")),
r2d2(new Model("resources/models/r2d2/R2-D2.obj")),
box(new Model("resources/models/box/box.obj")),
accumulatorTime(0.0f)
{
  
}
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
void BlendingScene::init(){
   	boxPositions.push_back(glm::vec3(-5,-1.4,0));
	boxPositions.push_back(glm::vec3(-5,0.2,4.5));
	boxPositions.push_back(glm::vec3(-2.5,-1.7,-3.2));
    // configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
    // Enable blending 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  

    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);//IMPORTANT. This is a 2 because location 1 on backpack.vert is for the normal vector.
                                // TexCoords are at location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    transparentTexture = loadTexture("resources/textures/grass.png");
    blendTexture = loadTexture("resources/textures/blending_transparent_window.png");

    TransparentShader.get()->use();
    TransparentShader.get()->setInt("texture1", 0);
    BlendShader.get()->use();
    BlendShader.get()->setInt("texture1", 0);
    
    vegetation.push_back(glm::vec3(-1.5f, 0.0f, 3.48f));
    vegetation.push_back(glm::vec3(1.5f, 0.0f, 4.51f));
    vegetation.push_back(glm::vec3(0.0f, 0.0f, 6.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, 5.3f));
    vegetation.push_back(glm::vec3(0.5f, 0.0f, 3.6f));
}


void BlendingScene::draw(Camera& c, Window& win){
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
    //to see leaves just uncomment this 
    // TransparentShader.get()->use();
    // TransparentShader.get()->setMat4("view", view);
    // TransparentShader.get()->setMat4("projection", projection);
    // glBindVertexArray(transparentVAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, transparentTexture);
    // for (unsigned int i = 0; i < vegetation.size(); i++)
    // {
    //     model = glm::mat4(1.0f);
    //     model = glm::translate(model, vegetation[i]);
    //     TransparentShader.get()->setMat4("model", model);
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    // }
    BlendShader.get()->use();
    BlendShader.get()->setMat4("view", view);
    BlendShader.get()->setMat4("projection", projection);
    glBindVertexArray(transparentVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blendTexture);
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < vegetation.size(); i++) 
    {
        //it's important to sort them, so put them in a map with distance from camera as the key
        
        float distance = glm::length(c.Position - vegetation[i]);
        sorted[distance] = vegetation[i];
    }
    //then use the iterator. the reverse one on the std::map 
    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, it->second);
        BlendShader.get()->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}