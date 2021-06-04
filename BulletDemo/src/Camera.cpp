#include "Camera.h"

void Camera::Matrix(float fovDEG, float nearPlane, float farPlane, GLuint shaderProgramID, const char* uniform)
{
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);

	view = glm::lookAt(m_Position, m_Position + m_Orientation, m_UpDirection);
	projection = glm::perspective(glm::radians(fovDEG), m_Width/m_Height, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::HandleInputs(GLFWwindow* window, float deltaTime)
{
	if (m_IsFreeCam)
	{
		HandleFreeCameraInput(window, deltaTime);
	}
	else
	{
		HandleFollowObjectCameraInput(window, deltaTime);
	}
}

void Camera::HandleFreeCameraInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS && m_FollowObject != nullptr)
	{
		m_IsFreeCam = false;
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glm::vec3 positionIntermediate(0.0f);
	glm::vec3 cameraXDirection = glm::cross(m_Orientation, m_UpDirection);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		positionIntermediate += deltaTime * m_Speed * m_Orientation;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		positionIntermediate += deltaTime * m_Speed * -cameraXDirection;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		positionIntermediate += deltaTime * m_Speed * -m_Orientation;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		positionIntermediate += deltaTime * m_Speed * cameraXDirection;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		positionIntermediate += deltaTime * m_Speed * m_UpDirection;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		positionIntermediate += deltaTime * m_Speed * -m_UpDirection;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (m_FirstClick == true)
		{
			m_FirstClick = false;
		}
		else
		{
			float mouseXDiff = -1.0f * (mouseX - m_PreviousMousePos.x);
			float mouseYDiff = -1.0f * (mouseY - m_PreviousMousePos.y);

			if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
			{
				glm::vec3 newOrientation = glm::rotate(m_Orientation, glm::radians(deltaTime * mouseYDiff * m_Sensitivity), -cameraXDirection);
				if (abs(glm::angle(newOrientation, m_UpDirection)) <= glm::radians(175.0f))
				{
					m_Orientation = newOrientation;
				}

				m_Orientation = glm::rotate(m_Orientation, glm::radians(deltaTime * mouseXDiff * m_Sensitivity), -m_UpDirection);
			}
			else
			{
				glm::vec3 dragDirection = glm::vec3(mouseXDiff, -mouseYDiff, 0.0f);

				glm::vec3 frontReference = glm::vec3(0.0, 0.0, -1.0);
				if (m_Orientation != frontReference)
				{
					float rotAngle = glm::angle(frontReference, m_Orientation);
					if (rotAngle > glm::pi<float>() * 0.5)
					{
						//glm::cross gives undesirable results when the angle between the reference and the orientation is greater than 90deg
						frontReference.z = 1.0;
						dragDirection = glm::rotate(dragDirection, glm::pi<float>(), m_UpDirection);
						rotAngle = glm::angle(frontReference, m_Orientation);
					}

					glm::vec3 rotAxis = glm::cross(frontReference, m_Orientation);
					dragDirection = glm::rotate(dragDirection, rotAngle, rotAxis);

				}

				positionIntermediate += deltaTime * dragDirection;
			}
		}

		m_PreviousMousePos.x = mouseX;
		m_PreviousMousePos.y = mouseY;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (m_FirstClick == true)
		{
			m_FirstClick = false;
		}
		else
		{
			float mouseXDiff = -1.0f * (mouseX - m_PreviousMousePos.x);
			float mouseYDiff = -1.0f * (mouseY - m_PreviousMousePos.y);

			glm::vec3 newOrientation = glm::rotate(m_Orientation, glm::radians(deltaTime * mouseYDiff * m_Sensitivity), -cameraXDirection);
			if (abs(glm::angle(newOrientation, m_UpDirection)) <= glm::radians(175.0f))
			{
				m_Orientation = newOrientation;
			}

			m_Orientation = glm::rotate(m_Orientation, glm::radians(deltaTime * mouseXDiff * m_Sensitivity), -m_UpDirection);
		}

		m_PreviousMousePos.x = mouseX;
		m_PreviousMousePos.y = mouseY;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
	{
		m_PreviousMousePos.x = 0.0;
		m_PreviousMousePos.y = 0.0;
		m_FirstClick = true;
	}

	//glm::normalize(positionIntermediate);
	m_Position += positionIntermediate;
}

void Camera::HandleFollowObjectCameraInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		m_IsFreeCam = true;
		return;
	}

	if (m_FollowObject == nullptr)
		return;

	btVector3 bulletFollowObjectForwardVector = m_FollowObject->getForwardVector();
	glm::vec3 followObjectForwardVector(bulletFollowObjectForwardVector.getX(), bulletFollowObjectForwardVector.getY(), bulletFollowObjectForwardVector.getZ());

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glm::vec3 positionIntermediate(0.0f);
	glm::vec3 cameraXDirection = glm::cross(m_Orientation, m_UpDirection);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	float halfWidth = m_Width * 0.5;
	float halfHeight = m_Height * 0.5;

	//convert to coordinates with origin at the center
	mouseX = mouseX - halfWidth;
	mouseY = mouseY - halfHeight;

	float angleX = glm::radians(70.0f) * (mouseX / halfWidth);
	float angleY = glm::radians(45.0f) * (mouseY / halfHeight);

	//clamp upwards facing y to before seeing beneath floor
	if (angleY < glm::radians(-16.0f))
		angleY = glm::radians(-16.0f);

	m_Orientation = glm::rotate(followObjectForwardVector, angleY, -cameraXDirection);
	m_Orientation = glm::rotate(m_Orientation, angleX, -m_UpDirection);

	btTransform followObjectTransform;
	m_FollowObject->getRigidBody()->getMotionState()->getWorldTransform(followObjectTransform);

	btVector3 bulletFollowObjectPosition = followObjectTransform.getOrigin();
	glm::vec3 followObjectPosition(bulletFollowObjectPosition.getX(), bulletFollowObjectPosition.getY(), bulletFollowObjectPosition.getZ());

	m_Position = followObjectPosition - (m_Orientation * 10.0f);
	m_Position.y += 3.0f;
}

void Camera::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	float scrollDir = yoffset / abs(yoffset);
	m_Position += 0.5f * m_Orientation * scrollDir;
}