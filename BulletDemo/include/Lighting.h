#pragma once

#include <glm.hpp>

struct DirectionalLight
{
	//Ambient Light
	glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float AmbientIntensity = 1.0f;

	//Diffuse Light
	glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float DiffuseIntensity = 1.0f;
};

void SetDirectionalLight(GLuint shaderProgramID, const DirectionalLight& Light)
{
	glUniform3f(glGetUniformLocation(shaderProgramID, "u_DirectionalLight.Color"), Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(glGetUniformLocation(shaderProgramID, "u_DirectionalLight.AmbientIntensity"), Light.AmbientIntensity);
	glm::vec3 Direction = glm::normalize(Light.Direction);
	glUniform3f(glGetUniformLocation(shaderProgramID, "u_DirectionalLight.Direction"), Direction.x, Direction.y, Direction.z);
	glUniform1f(glGetUniformLocation(shaderProgramID, "u_DirectionalLight.DiffuseIntensity"), Light.DiffuseIntensity);
}
