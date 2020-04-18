#pragma once

#include "Shader.h"

#ifndef GLEW_STATIC
#define GLEW_STATIC 1
#endif


#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#ifndef _PROJECTION_TYPE
enum ProjectionType {
	PERSPECTIVE,
	ORTHOGRAPHIC
};
#endif

class Camera
{

public:
	Camera() = default;
	Camera(glm::vec3* Eye, glm::vec3* Center, glm::vec3* Up, float* _speed, ProjectionType Type, GLFWwindow* win);
	~Camera();
	void processMovement(GLFWwindow* win, float deltaTime);

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	void updateView(Shader sh, GLFWwindow* win, float deltaTime);
	void reset();

	void zoom(double amount);

	void addWall(int loc, std::pair<glm::vec3, glm::vec3> addition) {
		switch (loc)
		{
		//north
		case 1:
			camNorthWalls.push_back(addition);
			break;
		//south
		case 2:
			camSouthWalls.push_back(addition);
			break;
		//east
		case 3:
			camEastWalls.push_back(addition);
			break;
		//west
		case 4:
			camWestWalls.push_back(addition);
			break;
		default:
			std::cout << "Invalid direction" << std::endl;
			break;
		}
	}

	glm::mat4 GetViewMatrix() const { return viewMatrix; }
	glm::mat4 GetProjectionMatrix() const { return projectionMatrix; }


private:
	float* speed;
	float radius;
	float position;
	//float angle;
	glm::vec3* camEye; // position of the cam
	glm::vec3* camCenter; // where looking
	glm::vec3* camUp; // where up is
	double oldMousePosX, oldMousePosY;
	//double mousePosX, mousePosY;
	float yaw, pitch;
	float fov;

	bool checkCollision(glm::vec3 pos, glm::vec3 norm, float range = 1);

	//vector containing a list of walls to check for collision against
	std::vector<std::pair<glm::vec3, glm::vec3>> camNorthWalls;
	std::vector<std::pair<glm::vec3, glm::vec3>> camSouthWalls;
	std::vector<std::pair<glm::vec3, glm::vec3>> camEastWalls;
	std::vector<std::pair<glm::vec3, glm::vec3>> camWestWalls;
};

