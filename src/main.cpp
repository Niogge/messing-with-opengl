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
#include "StencilScene.h"
#include "BlendingScene.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(Window& window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// camera
Camera camera(glm::vec3(10.0f, 0.0f, 13.0f));
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


	// build and compile our shader 
	//StartingScene scene;
	//StencilScene scene;
	BlendingScene scene;
	scene.init();
	while (win.isOpened()) {

		
		//input
		processInput(win);

		//rendering
		//------
		

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
