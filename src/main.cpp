#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <vector>
#include <filesystem>
#include <string>
#include "Model.h"
#include "Window.h"
#include "Scene.h"
#include "StartingScene.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(Window& window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
	Window win = Window(SCR_WIDTH, SCR_HEIGHT,"OpenGL Sandbox", false);

	glfwSetFramebufferSizeCallback(win.getWindow(), framebuffer_size_callback); //resize callback
	glfwSetCursorPosCallback(win.getWindow(), mouse_callback);
    glfwSetScrollCallback(win.getWindow(), scroll_callback);
 	// tell GLFW to capture our mouse
    glfwSetInputMode(win.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Error on Glad Init" << std::endl;
		return -1;
	}


	//stbi_set_flip_vertically_on_load(true);
	
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader 
	StartingScene scene;
	scene.init();
	while (win.isOpened()) {

		
		//input
		processInput(win);

		//rendering
		//------
		//clear color and depth
		glClearColor(0.2, 0.3, 0.3, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ourShader.use();
		// ourShader.setV3Float("dirLight.direction", -0.2f, -1.0f, -0.3f);
		// ourShader.setV3Float("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		// ourShader.setV3Float("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
		// ourShader.setV3Float("dirLight.specular", 1.0f,0.0f, 1.0f);
		
		// ourShader.setV3Float("viewPos", camera.Position); 
		// //Projection matrix
 		// glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // ourShader.setMat4("projection", projection);
		// // camera/view transformation
        // glm::mat4 view = camera.GetViewMatrix();
        // ourShader.setMat4("view", view);

		// glm::mat4 model = glm::mat4(1.0f);
		// model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		
		// model = glm::rotate(model, glm::radians(20.0f)*win.getDeltaTime()*0.5f, glm::vec3(0, 1.0f, 0));
		
        // ourShader.setMat4("model", model);
        // backpack.Draw(ourShader);
		// int i =1;
		// for (auto x : boxPositions)
		// {
		// 	model = glm::mat4(1.0f);
		// 	model = glm::translate(model, x);
		// 	float angle = 20.0f *i;
		// 	model = glm::rotate(model, glm::radians(angle)*(float)glfwGetTime()*0.5f, glm::vec3(1.0f, 0.3f*i, 0.5f));
  		// 	ourShader.setMat4("model", model);
		// 	box.Draw(ourShader);
		// 	i++;
		// }
		scene.draw(camera, win);

     
		win.update();

	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(Window& window) {
	if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window.getWindow(), true);
	}
	const float cameraSpeed = 0.05f; // adjust accordingly

 	if (glfwGetKey(window.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, window.getDeltaTime());
    if (glfwGetKey(window.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, window.getDeltaTime());
    if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, window.getDeltaTime());
    if (glfwGetKey(window.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, window.getDeltaTime());
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
