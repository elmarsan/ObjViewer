#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>

#include "math.h"
#include "shader.h"
#include "types.h"
#include "wavefront.h"
#include "camera.h"

const int screenWidth = 800;
const int screenHeight = 600;

// Camera globals
Camera camera{ vec3{0, 1.0f, 5.0f} };

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// Timing globals
float deltaTime;
float lastFrame;

// GLFW callbacks and input handling
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void handleInput(GLFWwindow* window);

int main(int argc, char* argv[])
{

	if (argc != 2 || argv[1] == "")
	{
		std::cerr << "Error: Missing wavefront file" << std::endl;
		return 1;
	}	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto* window = glfwCreateWindow(screenWidth, screenHeight, "Wavefront viewer", nullptr, nullptr);
	assert(window != nullptr && "Failed to create GLFW Window");
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) && "Failed to initialize GLAD");

	// const std::string filename = "cow.obj";	
	std::ifstream fstream(argv[1]);
	if (!fstream.is_open())
	{
		std::cerr << "Error: Could not open file " << argv[1] << std::endl;
		return 1;
	}

	if (fstream.bad())
	{
		std::cerr << "Error: I/O error while reading file " << argv[1] << std::endl;
	}

	Wavefront obj;
	obj.load(fstream);
	fstream.close();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Shader shader{ "mvp.vs", "color.fs" };

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handleInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 view = camera.viewMatrix();
		mat4 projection = perspective(radians(camera.Zoom), float(screenWidth) / float(screenHeight), 0.1f, 100.0f);
		mat4 model{ 1.0f };

		shader.use();
		shader.setVec3("Color", vec3{ 1 });
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		obj.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.setZoom(static_cast<float>(yoffset));
}

void handleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.move(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.move(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.move(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.move(RIGHT, deltaTime);
	}
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.07f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.setEulerAngles(xoffset, yoffset);
}