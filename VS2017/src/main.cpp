#include <iostream>
#include <cmath>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "ColoredVertex.h"
#include "Mesh.h"
#include "objMesh.h"
#include "objModel.h"
#include "Texture.h"
#include "Grid.h"

#include <stdlib.h>
#include <time.h>

#include "Heights.h"

#ifndef GLEW_STATIC
#define GLEW_STATIC 1
#endif


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//For error checking for error checking
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GlClearError();\
	x;\
	ASSERT(glError(#x, __FILE__, __LINE__))

static void GlClearError() {
	while (glGetError() != 0);
}

static bool glError(const char* funct, const char* file, int line) {
	while (GLenum err = glGetError()) {
		std::cout << "[OpenGL Error]: " << err << funct << " " << file << " : " << line << ";" << std::endl;
		return false;
	}
	return true;
}


Camera cam;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main() {

	unsigned int WIDTH = 1024, HEIGHT = 768;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creating GLFW window
	GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "COMP371 - Project", NULL, NULL);
	if (win == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	
	// VSYNC - Sync framerate with screen refresh rate - 60Hz
	glfwSwapInterval(1);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// OpenGL Version Check
	//std::cout << glGetString(GL_VERSION) << std::endl;

	glfwSetScrollCallback(win, scroll_callback);

	// Shader Creation
	Shader sh("assets/shaders/vertexShader.glsl", "assets/shaders/fragShader.glsl");
	Shader depthShader("assets/shaders/shadowVert.glsl", "assets/shaders/shadowFrag.glsl");


	// Arrays for the shapes used in rendering
	coloredVertex line[] = {
		coloredVertex(glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
		coloredVertex(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	};

	Grid* gr = new Grid();
	std::vector<float> Xgrid = gr->getGridX();
	std::vector<float> Zgrid = gr->getGridZ();
	std::vector<objMesh*> fuck;
	// Creating meshes
	// creating line
	Mesh _line(line, sizeof(line), glm::vec3(1.0f, 1.0f, 0.0f));

	std::string buildingOptions[6] = {
		"assets/models/b1.ob",
		"assets/models/b2.ob",
		"assets/models/b3.ob",
		"assets/models/b4.ob",
		"assets/models/b5.ob",
		"assets/models/b6.ob"
	};

	std::vector<objMesh*> renderObjects;

	objMesh plane("assets/models/plane.ob", glm::vec3(1.f), glm::vec3(0.f), glm::vec3(50.f, 1.f, 50.f));

	int seed = 0;
	auto buildings = gr->getBuildingPos();
	for (auto i : buildings)
	{
		srand(seed);
		int buildingChoice = rand() % 6;
		float size = randomizeHeight(.1f, .3f);
		//std::cout << buildingChoice << std::endl;
		//std::cout << size << std::endl;
		fuck.push_back(new objMesh(buildingOptions[buildingChoice], glm::vec3(1.f, 1.f, 1.f), glm::vec3(i.first * 15.f, 0.f, i.second * 15.f), glm::vec3(.07f, size, .07f)));
		seed++;
	}
	auto roads = gr->getRoadPos();
	for (auto i : roads)
	{
		fuck.push_back(new objMesh("assets/models/sphere.ob", glm::vec3(1.f, 0.f, 0.f), glm::vec3(i.first * 15.f, 0.f, i.second * 15.f), glm::vec3(.5f)));
	}


	//Textures!!1

	Texture snow("assets/textures/snow.jpg", GL_TEXTURE_2D);
	Texture asphalt("assets/textures/asphalt.jpg", GL_TEXTURE_2D);
	plane.setTexture(&asphalt);


	//configuring depth map
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//initial shader config
	sh.use();
	sh.setInt("diffuseTexture", 0);
	sh.setInt("shadowMap", 1);


	//glm::vec3 lightPos = glm::vec3(0.f, 30.f, 0.f);
	//glm::vec3 lightPos = glm::vec3(-1.f, 4.f, -2.f);

	glm::vec3 lightPos = glm::vec3(0.f, 10.f, -1.f);


	
	// Setting up Camera with starting point
	float spd = 1.0f;
	
	glm::vec3 Eye = glm::vec3(0.0f, 25.0f, 10.0f);
	//glm::vec3 Eye = lightPos;
	glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);


	cam = Camera(&Eye, &Center, &Up, &spd, PERSPECTIVE, win);



	float lastFrameTime = glfwGetTime();

	// Disabling mouse cursor
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	bool hasRandomized = false;
	float n = 1.f;
	bool shadows = true;
	bool hasTurned = false;

	

	//std::cout << Xgrid[24] <<" "<< Zgrid[24] <<std::endl;

	while (!glfwWindowShouldClose(win))
	{
		// calculating deltatime
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

		// Background Color
		glClearColor(0.11f, 0.44f, 0.68f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//first pass, render shadow map

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = -1.f, far_plane = 40.f;

		//lightProjection = glm::perspective(glm::radians(45.f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
		lightSpaceMatrix = lightProjection * lightView;

		//render from ligth's pov
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		plane.draw(&depthShader);

		for (auto i : fuck)
			i->draw(&depthShader);

		//b.draw(&depthShader);



		//olaf.draw(&depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		//reset
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/////////////////////////////////////////////////////////////////////////////////////////////////
		//second pass, render scene
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sh.use();

		// camera's input processing
		cam.processMovement(win, dt);

		// Performing view and projection transformations for camera
		cam.updateView(sh, win, dt);

		
		//Light
		sh.setVec3("light.position", lightPos);
		sh.setVec3("light.intensities", 1.f, 1.f, 1.f);
		sh.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		sh.setBool("shadows", shadows);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		//drawing
		//Drawing snowman at origin
		glLineWidth(1);
		glPointSize(10);

		
		plane.draw(&sh);
		//b.draw(&sh);

		//olaf.draw(&sh);
		//id.draw(&sh);
		for (auto i : fuck)
			i->draw(&sh);

		// Rendering
		glm::mat4 scalingMatrix;
		glm::mat4 translationMatrix;
		glm::mat4 worldMatrix;
		glm::mat4 rotation;

		// Coordinate Axis Lines
		int scale = 5; // 5 Unit length
		glLineWidth(5);

		//X
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale * 1.0f, 1.1f, 1.1f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(scale * 0.5f, 0.0f, 0.0f));
		worldMatrix = translationMatrix * scalingMatrix;
		_line.draw(sh, GL_LINES, 0, 3, worldMatrix, glm::vec3(1.0f, 0.0f, 0.0f));

		//Y
		rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, scale * 0.5f, 0.0f));
		worldMatrix = translationMatrix * rotation * scalingMatrix;
		_line.draw(sh, GL_LINES, 0, 3, worldMatrix, glm::vec3(0.0f, 1.0f, 0.0f));

		//Z
		rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, scale * 0.5f));
		worldMatrix = translationMatrix * rotation * scalingMatrix;
		_line.draw(sh, GL_LINES, 0, 3, worldMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
		/////////////////////////////////////////////////////////////////////////////////////////////////

		// Swap buffers
		glfwSwapBuffers(win);
		// Check/call events
		glfwPollEvents();

		if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			// Escape to close window
			glfwSetWindowShouldClose(win, true);
		}

		if (glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS) {
			cam.reset();
		}


		glUseProgram(0);
	}

	// Cleanup
	glfwTerminate();

	return 0;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.zoom(-yoffset);
}