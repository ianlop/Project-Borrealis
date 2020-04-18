#include "Camera.h"

#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3* Eye, glm::vec3* Center, glm::vec3* Up, float* _speed, ProjectionType Type, GLFWwindow* win) 
{
	camEye = Eye;			// Position
	camCenter = Center;		// Looking At Point
	//camCenter = new glm::vec3(0.f);
	camUp = Up;				// Up
	speed = _speed;
	radius = 10.f;
	position = 0.f;
	fov = 70.f;

	if (Type == PERSPECTIVE) 
	{
		// projection matrix to have camera in perspective
		projectionMatrix = glm::perspective(glm::radians(fov),  // field of view in degrees
			1280.0f / 720.0f,      // aspect ratio
			0.01f, 500.0f);       // near and far (near > 0)
	}
	else 
	{
		// projection matrix to have camera in orthograpic TODO

	}

	viewMatrix = glm::mat4(1.f);

	glfwGetCursorPos(win, &oldMousePosX, &oldMousePosY);

	yaw = -90.0f;
	pitch = .0f;
}

Camera::~Camera() 
{
	camEye = nullptr;
	camCenter = nullptr;
	camUp = nullptr;
	speed = nullptr;
}

void Camera::processMovement(GLFWwindow* win, float deltaTime) 
{

	double mousePosX, mousePosY;
	glfwGetCursorPos(win, &mousePosX, &mousePosY);

	double dx = mousePosX - oldMousePosX;
	double dy = mousePosY - oldMousePosY;

	oldMousePosX = mousePosX;
	oldMousePosY = mousePosY;

	float sensitivity = 0.1f;
	dx *= sensitivity;
	dy *= -sensitivity;


	// Calcualting a speed normalized based on how much time has passed,
	// speed is no longer affected by fps
	float normalizedSpeed = *speed * deltaTime * 10;

	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) 
	{
		*camEye += *camCenter * normalizedSpeed;
	}
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) 
	{
		*camEye -= *camCenter * normalizedSpeed;
	}
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) 
	{
		*camEye -= glm::normalize(glm::cross(*camCenter, *camUp)) * normalizedSpeed;
	}
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) 
	{
		*camEye += glm::normalize(glm::cross(*camCenter, *camUp)) * normalizedSpeed;
	}
	if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		*camEye += glm::normalize(*camUp) * normalizedSpeed;
	}

	//colision with ground
	if (checkCollision(glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)))
		camEye->y = 1.f;

	std::cout << checkCollision(glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)) << std::endl;
	std::cout << camEye->y << std::endl;

	yaw += dx;
	pitch += dy;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	//collision detection


}

void Camera::updateView(Shader sh, GLFWwindow* win, float deltaTime) {

	// Projection Transform
	projectionMatrix = glm::perspective(glm::radians(fov),  // field of view in degrees
		1024.0f / 768.0f,      // aspect ratio
		0.01f, 200.0f);       // near and far (near > 0)
	sh.setMat4("projectionMatrix", projectionMatrix);
	sh.setVec3("viewPos", *camEye);

	// View Transform - from camera movement
	
	*camCenter = glm::normalize(glm::vec3(
		cosf(glm::radians(yaw)) * cosf(glm::radians(pitch)),
		sinf(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))
	));

	viewMatrix = glm::lookAt(
		*camEye,
		*camEye + *camCenter,
		*camUp
	);

	sh.setMat4("viewMatrix", viewMatrix);
}

void Camera::reset() {
	yaw = -90.0f;
	pitch = .0f;

	*camEye = glm::vec3(0.f, 10.f, 10.f);

}

void Camera::zoom(double amount)
{
	fov += amount;
}

bool Camera::checkCollision(glm::vec3 pos, glm::vec3 norm)
{
	//printf("\nThe to pos vector: %d\n", (*camEye - pos).y);
	//printf("The normal vector: %d\n", norm);
	//printf("The Dot Product: %d\n\n", glm::dot(norm, *camEye - pos));

	return glm::dot(norm, *camEye - pos) < 1;
}
