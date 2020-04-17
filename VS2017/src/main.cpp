#include <iostream>
#include <cmath>

#include "Shader.h"
#include "Camera.h"
#include "ColoredVertex.h"
#include "Mesh.h"
#include "objMesh.h"
#include "objModel.h"
#include "Texture.h"

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
	GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "COMP371 - Assignment 1", NULL, NULL);
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
	Shader shSkybox("assets/shaders/skyboxVertex.glsl", "assets/shaders/skyboxFragment.glsl");

	GLfloat skyboxVertices[] = {
		
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

	// VAO for skybox
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glBindVertexArray(0);

	// Arrays for the shapes used in rendering
	coloredVertex line[] = {
		coloredVertex(glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
		coloredVertex(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	};


	// Creating meshes
	// creating line
	Mesh _line(line, sizeof(line), glm::vec3(1.0f, 1.0f, 0.0f));

	objMesh plane("assets/models/plane.ob", glm::vec3(.8f), glm::vec3(0.f), glm::vec3(50.f, 1.f, 50.f));

	//objMesh id("assets/models/sphere.obj", glm::vec3(1.f), glm::vec3(10.f, 1.1f, 10.f), glm::vec3(.1f));

	objMesh torso("assets/models/sphere.ob", glm::vec3(.99f), glm::vec3(0.f, 2.1f, 0.f), glm::vec3(1.5f, 1.f, 1.f));
	objMesh button1("assets/models/sphere.ob", glm::vec3(0.f), glm::vec3(0.f, 2.6f, .9f), glm::vec3(.2f, .2f, .2f));
	objMesh button2("assets/models/sphere.ob", glm::vec3(0.f), glm::vec3(0.f, 2.1f, .9f), glm::vec3(.2f, .2f, .2f));
	objMesh button3("assets/models/sphere.ob", glm::vec3(0.f), glm::vec3(0.f, 1.6f, .9f), glm::vec3(.2f, .2f, .2f));

	objMesh head("assets/models/sphere.ob", glm::vec3(1.f), glm::vec3(0.f, 3.6f, 0.f), glm::vec3(1.2f));
	objMesh hat1("assets/models/cube.ob", glm::vec3(1.f), glm::vec3(0.f, 4.8f, 0.f), glm::vec3(1.f, .3f, 1.f));
	objMesh hat2("assets/models/cube.ob", glm::vec3(1.f, .88f, .42f), glm::vec3(0.f, 5.8f, 0.f), glm::vec3(.7f, .5f, .7f));
	objMesh hat3("assets/models/cube.ob", glm::vec3(1.f), glm::vec3(0.f, 6.8f, 0.f), glm::vec3(.25f, .7f, .25f));
	objMesh eye("assets/models/cube.ob", glm::vec3(1.f), glm::vec3(0.f, 4.1f, 1.f), glm::vec3(.8f, .2f, .2f));

	objMesh leftArm("assets/models/cube.ob", glm::vec3(.5f, .37f, .2f), glm::vec3(2.8f, 2.1f, 0.f), glm::vec3(1.5f, .1f, .1f));
	objMesh rightArm("assets/models/cube.ob", glm::vec3(.5f, .37f, .2f), glm::vec3(-2.8f, 2.1f, 0.f), glm::vec3(1.5f, .1f, .1f));

	objMesh leftLeg("assets/models/cube.ob", glm::vec3(.5f, .37f, .2f), glm::vec3(.5f, .3f, 0.f), glm::vec3(.2f, 1.f, .2f));
	objMesh rightLeg("assets/models/cube.ob", glm::vec3(.5f, .37f, .2f), glm::vec3(-.5f, .3f, 0.f), glm::vec3(.2f, 1.f, .2f));
	objMesh leftFoot("assets/models/cube.ob", glm::vec3(0.f), glm::vec3(.5f, .1f, 0.f), glm::vec3(.35f, .1f, .35f));
	objMesh rightFoot("assets/models/cube.ob", glm::vec3(0.f), glm::vec3(-.5f, .1f, 0.f), glm::vec3(.35f, .1f, .35f));

	objMesh scarf("assets/models/cube.ob", glm::vec3(.81f, .55f, .18f), glm::vec3(0.f, 3.f, 0.f), glm::vec3(1.2f, .2f, 1.f), glm::vec3(0.f, 0.f, 10.f));
	objMesh scarfBit("assets/models/cube.ob", glm::vec3(.81f, .55f, .18f), glm::vec3(1.f, 3.f, 0.f), glm::vec3(1.2f, .2f, .2f), glm::vec3(0.f, 0.f, 10.f));

	objModel olaf;
	olaf.addMesh(&torso);
	olaf.addMesh(&button1);
	olaf.addMesh(&button2);
	olaf.addMesh(&button3);
	olaf.addMesh(&head);
	olaf.addMesh(&hat1);
	olaf.addMesh(&hat2);
	olaf.addMesh(&hat3);
	olaf.addMesh(&eye);
	olaf.addMesh(&leftArm);
	olaf.addMesh(&rightArm);
	olaf.addMesh(&leftLeg);
	olaf.addMesh(&leftFoot);
	olaf.addMesh(&rightLeg);
	olaf.addMesh(&rightFoot);
	olaf.addMesh(&scarf);
	olaf.addMesh(&scarfBit);

	//Textures!!1

	Texture snow("assets/textures/snow.jpg", GL_TEXTURE_2D);
	plane.setTexture(&snow);
	Texture carrot("assets/textures/carrot.jpg", GL_TEXTURE_2D);
	eye.setTexture(&carrot);
	Texture metal("assets/textures/metal.jpg", GL_TEXTURE_2D);
	hat1.setTexture(&metal);
	hat2.setTexture(&metal);
	hat3.setTexture(&metal);

	Texture col("assets/textures/color.png", GL_TEXTURE_2D);

	// Skybox Textures
	std::vector<const GLchar*> faces;
	faces.push_back("assets/textures/skybox_images/right.png");
	faces.push_back("assets/textures/skybox_images/left.png");
	faces.push_back("assets/textures/skybox_images/top.png");
	faces.push_back("assets/textures/skybox_images/down.png");
	faces.push_back("assets/textures/skybox_images/back.png");
	faces.push_back("assets/textures/skybox_images/front.png");

	GLuint skyboxTexture = Texture::LoadSkybox(faces);

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
	glm::vec3 lightPos = glm::vec3(0.f, 20.f, -1.f);

	hat1.setShiny(256.f);
	hat3.setShiny(256.f);


	
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

	//glm::mat4 projection = glm::perspective(cam.GetZoom(), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);

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
		olaf.draw(&depthShader);
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

		//plane.setTexture(&depthMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		//drawing
		//Drawing snowman at origin
		glLineWidth(1);
		glPointSize(10);

		
		plane.draw(&sh);
		olaf.draw(&sh);
		//id.draw(&sh, GL_TRIANGLES);


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

		// Drawing Skybox
		//glDepthFunc(GL_LEQUAL);
		//shSkybox.use();
		//glm::mat4 view = glm::mat4(glm::mat3(cam.updateView(shSkybox, win, dt))); // was just view before

		//glUniformMatrix4fv(glGetUniformLocation(shSkybox.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(glGetUniformLocation(shSkybox.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(win);
		// Check/call events
		glfwPollEvents();

		//TODO setup as callback
		if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			// Escape to close window
			glfwSetWindowShouldClose(win, true);
		}
		if (glfwGetKey(win, GLFW_KEY_L) == GLFW_PRESS) {
			// Wireframe with GL_LINE_LOOP
			olaf.changeType(GL_LINE_LOOP);
		}
		if (glfwGetKey(win, GLFW_KEY_T) == GLFW_PRESS) {
			// Shape with GL_TRIANGLES
			olaf.changeType(GL_TRIANGLES);
		}
		if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS) {
			// Points with GL_POINTS
			olaf.changeType(GL_POINTS);
		}

		

		if (glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS) {
			cam.reset();
		}


		if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
		{
			plane.setTexture(&snow);
			eye.setTexture(&carrot);
			hat1.setTexture(&metal);
			hat2.setTexture(&metal);
			hat3.setTexture(&metal);

			eye.setColor(glm::vec3(1.f));
			hat1.setColor(glm::vec3(1.f));
			hat3.setColor(glm::vec3(1.f));
		}
		if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			plane.setTexture(&col);
			eye.setTexture(&col);
			hat1.setTexture(&col);
			hat2.setTexture(&col);
			hat3.setTexture(&col);

			eye.setColor(glm::vec3(0.f));
			hat1.setColor(glm::vec3(0.f));
			hat3.setColor(glm::vec3(0.f));
		}

		if (glfwGetKey(win, GLFW_KEY_B) == GLFW_PRESS && !hasTurned)
		{
			shadows = !shadows;
			hasTurned = true;
		}
		if (glfwGetKey(win, GLFW_KEY_B) == GLFW_RELEASE)
		{
			//shadows = false;
			hasTurned = false;
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