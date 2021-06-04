#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>

#include "utils.h"

class Camera
{
private:
	const glm::vec3 m_UpDirection = glm::vec3(0.0, 1.0, 0.0);

	glm::vec3 m_Position;
	glm::vec3 m_Orientation = glm::vec3(0.0, 0.0, -1.0);

	glm::vec2 m_PreviousMousePos = glm::vec2(0.0f, 0.0f);

	float m_Speed = 7.0f;
	float m_Sensitivity = 7.0f;

	float m_Width;
	float m_Height;

	btRaycastVehicle* m_FollowObject;

	bool m_FirstClick = true;
	bool m_IsFreeCam = true;

public:
	inline glm::vec3 GetPosition() { return m_Position; }
	inline glm::vec3 GetOrientation() { return m_Orientation; }

	void Matrix(float fovDEG, float nearPlane, float farPlane, GLuint shaderProgramID, const char* uniform);
	void HandleInputs(GLFWwindow* window, float deltaTime);
	void HandleFreeCameraInput(GLFWwindow* window, float deltaTime);
	void HandleFollowObjectCameraInput(GLFWwindow* window, float deltaTime);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	Camera(float width, float height, glm::vec3 initialPosition, GLFWwindow* window, btRaycastVehicle* followObject = nullptr)
		: m_Width(width)
		, m_Height(height)
		, m_Position(initialPosition)
	{
		m_FollowObject = followObject;

		glfwSetWindowUserPointer(window, this);
		auto func = [](GLFWwindow* window, double xoffset, double yoffset)
		{
			static_cast<Camera*>(glfwGetWindowUserPointer(window))->ScrollCallback(window, xoffset, yoffset);
		};

		glfwSetScrollCallback(window, func);
	}
};
