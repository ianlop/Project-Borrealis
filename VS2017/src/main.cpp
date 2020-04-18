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
#include <random>


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
	std::vector<objMesh*> modelsList;
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
		float size = randomizeHeight(.05f, .2f);
		//std::cout << buildingChoice << std::endl;
		//std::cout << size << std::endl;
		modelsList.push_back(new objMesh(buildingOptions[buildingChoice], glm::vec3(1.f, 1.f, 1.f), glm::vec3(i.first * 10.f, 0.f, i.second * 10.f), glm::vec3(.05f, size, .05f)));
		seed++;
	}
	//auto roads = gr->getRoadPos();
	//for (auto i : roads)
	//{
	//	modelsList.push_back(new objMesh("assets/models/sphere.ob", glm::vec3(1.f, 0.f, 0.f), glm::vec3(i.first * 15.f, 0.f, i.second * 15.f), glm::vec3(.5f)));
	//}


	//Textures!!1

	Texture snow("assets/textures/snow.jpg", GL_TEXTURE_2D);
	Texture asphalt("assets/textures/asphalt.jpg", GL_TEXTURE_2D);
	Texture col("assets/textures/color.png", GL_TEXTURE_2D);
	plane.setTexture(&asphalt);
	plane.setTexture(&col);

	////////////////////IAN DRAWINGS/////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(-45.0, 45.0);

	//cars
	objMesh lexus("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(dis(gen), -0.05f, -1.0), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus2("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(-10.0f / 12, -0.05f, dis(gen)), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus3("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(dis(gen), -0.05f, 1.0), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus4("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(10.0f / 12, -0.05f, dis(gen)), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus5("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(dis(gen), -0.05f, -1.0), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus6("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(-10.0f / 12, -0.05f, dis(gen)), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus7("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(dis(gen), -0.05f, 1.0), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	objMesh lexus8("assets/models/lexus.ob", glm::vec3(.99f), glm::vec3(10.0f / 12, -0.05f, dis(gen)), glm::vec3(0.4f / 12, 0.4f / 12, 0.4f / 12));
	std::uniform_real_distribution<> dis2(0.0, 1.0);
	lexus.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus2.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus2.rotate(0.0, 90.0, 0.0);
	lexus3.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus3.rotate(0.0, 180.0, 0.0);
	lexus4.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus4.rotate(0.0, -90.0, 0.0);
	lexus5.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus6.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus6.rotate(0.0, 90.0, 0.0);
	lexus7.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus7.rotate(0.0, 180.0, 0.0);
	lexus8.setColor(glm::vec3(dis2(gen), dis2(gen), dis2(gen)));
	lexus8.rotate(0.0, -90.0, 0.0);
	modelsList.push_back(&lexus);
	modelsList.push_back(&lexus2);
	modelsList.push_back(&lexus3);
	modelsList.push_back(&lexus4);
	modelsList.push_back(&lexus5);
	modelsList.push_back(&lexus6);
	modelsList.push_back(&lexus7);
	modelsList.push_back(&lexus8);

	//stops signs in each corner of crossed street
	objMesh stop("assets/models/StopSign.ob", glm::vec3(.99f), glm::vec3(22.f / 12, 2.1f / 12, -15.f / 12), glm::vec3(1.5f / 12, 1.f / 12, 1.f / 12));
	objMesh stop2("assets/models/StopSign.ob", glm::vec3(.99f), glm::vec3(16.f / 12, 2.1f / 12, 15.f / 12), glm::vec3(1.5f / 12, 1.f / 12, 1.f / 12));
	objMesh stop3("assets/models/StopSign.ob", glm::vec3(.99f), glm::vec3(-16.f / 12, 2.1f / 12, 15.f / 12), glm::vec3(1.5f / 12, 1.f / 12, 1.f / 12));
	objMesh stop4("assets/models/StopSign.ob", glm::vec3(.99f), glm::vec3(-15.f / 12, 2.1f / 12, -22.f / 12), glm::vec3(1.5f / 12, 1.f / 12, 1.f / 12));
	stop2.rotate(0.0, -90.0, 0.0);
	stop3.rotate(0.0, -180.0, 0.0);
	stop4.rotate(0.0, 90.0, 0.0);
	modelsList.push_back(&stop);
	modelsList.push_back(&stop2);
	modelsList.push_back(&stop3);
	modelsList.push_back(&stop4);

	//benches
	std::vector<objMesh*> bus_stop;
	std::vector<objMesh*> bus_stop2;

	//trash cans
	std::vector<objMesh*> tcs;
	std::vector<objMesh*> tcs2;
	std::vector<objMesh*> tcs3;
	std::vector<objMesh*> tcs4;

	//a collection of all the curved street lights
	std::vector<objMesh*> st_lights;//y rhs positive
	std::vector<objMesh*> st_lights2;//y rhs negative
	std::vector<objMesh*> st_lights3;//y lhs positive
	std::vector<objMesh*> st_lights4;//y lhs negative

	//Textures!!1

	Texture dark("assets/textures/snow.jpg", GL_TEXTURE_2D);


	Texture stop_text("assets/textures/stop.png", GL_TEXTURE_2D);
	stop.setTexture(&stop_text);
	stop2.setTexture(&stop_text);
	stop3.setTexture(&stop_text);
	stop4.setTexture(&stop_text);



	Texture black("assets/textures/metal.jpg", GL_TEXTURE_2D);

	//Texture concrete("assets/textures/concrete.jpg", GL_TEXTURE_2D);
	//plane.setTexture(&concrete);

	Texture green("assets/textures/green.jpg", GL_TEXTURE_2D);
	
	//setting up street lights in cross like manner
	float adder = -50;
	//adding in the trash cans
	for (float i = 0.0; i < 26.0; i++) {

		st_lights.push_back(new objMesh("assets/models/StreetLamp1.ob", glm::vec3(.9), glm::vec3(2.0, 0.0f, adder), glm::vec3(0.75f / 12, 0.95f / 12, 0.75f / 12)));
		st_lights[i]->rotate(0, 180, 0);
		st_lights[i]->setTexture(&black);

		st_lights3.push_back(new objMesh("assets/models/StreetLamp1.ob", glm::vec3(.9), glm::vec3(-2.0f, 0.0f, adder), glm::vec3(0.75f / 12, 0.95f / 12, 0.75f / 12)));
		st_lights3[i]->rotate(0, 360, 0);
		st_lights3[i]->setTexture(&black);

		st_lights2.push_back(new objMesh("assets/models/StreetLamp1.ob", glm::vec3(.9), glm::vec3(adder, 0.0f, 2.0f), glm::vec3(0.75f / 12, 0.95f / 12, 0.75f / 12)));
		st_lights2[i]->rotate(0, 90, 0);
		st_lights2[i]->setTexture(&black);

		st_lights4.push_back(new objMesh("assets/models/StreetLamp1.ob", glm::vec3(.9), glm::vec3(adder, 0.0f, -2.0f), glm::vec3(0.75f / 12, 0.95f / 12, 0.75f / 12)));
		st_lights4[i]->rotate(0, 270, 0);
		st_lights4[i]->setTexture(&black);

		tcs.push_back(new objMesh("assets/models/trash_can.ob", glm::vec3(.99f), glm::vec3(2.05f, 0.0f, adder + 1), glm::vec3(4.75f / 12, 4.75f / 12, 4.75f / 12)));
		tcs[i]->rotate(0, 360, 0);
		tcs[i]->setTexture(&green);

		tcs2.push_back(new objMesh("assets/models/trash_can.ob", glm::vec3(.99f), glm::vec3(-2.05f, 0.0f, adder + 1), glm::vec3(4.75f / 12, 4.75f / 12, 4.75f / 12)));
		tcs2[i]->rotate(0, 360, 0);
		tcs2[i]->setTexture(&green);

		tcs3.push_back(new objMesh("assets/models/trash_can.ob", glm::vec3(.99f), glm::vec3(adder - 0.25, 0.0f, 3.8), glm::vec3(4.75f / 12, 4.75f / 12, 4.75f / 12)));
		tcs3[i]->rotate(0, 360, 0);
		tcs3[i]->setTexture(&green);

		tcs4.push_back(new objMesh("assets/models/trash_can.ob", glm::vec3(.99f), glm::vec3(adder - 0.25, 0.0f, -1.8), glm::vec3(4.75f / 12, 4.75f / 12, 4.75f / 12)));
		tcs4[i]->rotate(0, 360, 0);
		tcs4[i]->setTexture(&green);

		adder += 4.0;
	}
	//adding bus stops
	adder = -50.0;
	for (int i = 0; i < 6; ++i) {
		adder += 15;
		if (i % 2 == 0) {
			bus_stop.push_back(new objMesh("assets/models/bench.ob", glm::vec3(.99f), glm::vec3(2.5, 0.0, adder-4.5), glm::vec3(0.015, 0.015, 0.015)));
			bus_stop2.push_back(new objMesh("assets/models/bench.ob", glm::vec3(.99f), glm::vec3(adder, 0.0,3.25), glm::vec3(0.015, 0.015, 0.015)));
			bus_stop[i]->rotate(0.0, 90.0, 0.0);
			bus_stop2[i]->rotate(0.0, 360.0, 0.0);
			bus_stop[i]->setColor(glm::vec3(0.59, 0.72, 0.99));
			bus_stop2[i]->setColor(glm::vec3(0.59, 0.72, 0.99));
		}
		else {
			bus_stop.push_back(new objMesh("assets/models/bench.ob", glm::vec3(.99f), glm::vec3(-2.5, 0.0, adder-4.5), glm::vec3(0.015, 0.015, 0.015)));
			bus_stop2.push_back(new objMesh("assets/models/bench.ob", glm::vec3(.99f), glm::vec3(adder, 0.0, -3.25), glm::vec3(0.015, 0.015, 0.015)));
			bus_stop[i]->rotate(0.0, -90.0, 0.0);
			bus_stop2[i]->rotate(0.0, 180.0, 0.0);
			bus_stop[i]->setColor(glm::vec3(0.59, 0.72, 0.99));
			bus_stop2[i]->setColor(glm::vec3(0.59, 0.72, 0.99));
		}
	}

	////////////////////IAN DRAWINGS//////////////////////////////////////////////////////////////////////////////////////////////////////

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


	glm::vec3 lightPos = glm::vec3(0.f, 10.f, -1.f);


	// Setting up Camera with starting point
	float spd = 1.0f;
	
	glm::vec3 Eye = glm::vec3(0.0f, 25.0f, 10.0f);
	//glm::vec3 Eye = lightPos;
	glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);


	cam = Camera(&Eye, &Center, &Up, &spd, PERSPECTIVE, win);

	//north facing walls
	cam.addWall(1, {glm::vec3(3.73f, 0.f, 3.19f), glm::vec3(0.f, 0.f, -1.f)});
	cam.addWall(1, {glm::vec3(-4.22f, 0.f, 4.35f), glm::vec3(0.f, 0.f, -1.f)});
	//south facing walls
	cam.addWall(2, {glm::vec3(-5.29f, 0.f, -5.01f), glm::vec3(0.f, 0.f, 1.f)});
	cam.addWall(2, {glm::vec3(6.23f, 0.f, -4.86f), glm::vec3(0.f, 0.f, 1.f)});
	//east facing walls
	cam.addWall(3, {glm::vec3(-4.22f, 0.f, 4.35f), glm::vec3(1.f, 0.f, 0.f)});
	cam.addWall(3, {glm::vec3(-5.29f, 0.f, -5.01f), glm::vec3(1.f, 0.f, 0.f)});
	//west facing walls
	cam.addWall(4, {glm::vec3(3.73f, 0.f, 3.19f), glm::vec3(-1.f, 0.f, 0.f)});
	cam.addWall(4, {glm::vec3(6.23f, 0.f, -4.86f), glm::vec3(-1.f, 0.f, 0.f)});


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

		//plane
		plane.draw(&depthShader);

		for (auto i : modelsList)
			i->draw(&depthShader);

		//b.draw(&depthShader);


		////////////////////IAN DRAWINGS///////////////////////////
		
		for (float i = 0.0; i < 26.0; i++) {
			tcs[i]->draw(&depthShader);
			tcs2[i]->draw(&depthShader);
			tcs3[i]->draw(&depthShader);
			tcs4[i]->draw(&depthShader);
			st_lights[i]->draw(&depthShader);
			st_lights2[i]->draw(&depthShader);
			st_lights3[i]->draw(&depthShader);
			st_lights4[i]->draw(&depthShader);
		}
		//bus stops
		for (int i = 0; i < 6; i++) {
			bus_stop[i]->draw(&depthShader);
			bus_stop2[i]->draw(&depthShader);
		}
		////////////////////IAN DRAWINGS///////////////////////////

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

		////////////////////IAN DRAWINGS///////////////////////////
		plane.draw(&sh);
		//b.draw(&sh);

		//id.draw(&sh);
		for (auto i : modelsList)
			i->draw(&sh);


		for (float i = 0.0; i < 26.0; i++) {
			tcs[i]->draw(&sh);
			tcs2[i]->draw(&sh);
			tcs3[i]->draw(&sh);
			tcs4[i]->draw(&sh);
			st_lights[i]->draw(&sh);
			st_lights2[i]->draw(&sh);
			st_lights3[i]->draw(&sh);
			st_lights4[i]->draw(&sh);

		}

		for (int i = 0; i < 6; i++) {
			bus_stop[i]->draw(&sh);
			bus_stop2[i]->draw(&sh);
		}
		////////////////////IAN DRAWINGS///////////////////////////

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
		//_line.draw(sh, GL_LINES, 0, 3, worldMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
		/////////////////////////////////////////////////////////////////////////////////////////////////


		glLineWidth(1);



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
	for (int i = 0; i < st_lights.size(); i++) {
		delete st_lights[i];
	}
	
	// Cleanup
	glfwTerminate();

	return 0;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.zoom(-yoffset);
}