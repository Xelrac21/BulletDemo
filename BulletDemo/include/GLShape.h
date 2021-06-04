#pragma once

#include <vector>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>
#include <btBulletDynamicsCommon.h>

class GLShape
{
protected:
	struct ShapeVertex
	{
		glm::vec3 position;
		glm::vec3 normal;

		ShapeVertex()
		{
			glm::vec3 zeroes(0.0f, 0.0f, 0.0f);
			position = zeroes;
			normal = zeroes;
		}
	};

protected:
	glm::vec3 m_ShapeColor;

	unsigned int m_NumVerts = 0;
	unsigned int m_NumIndices = 0;

	GLuint m_VertexArrayID = GL_FALSE;
	GLuint m_VertexBufferID = GL_FALSE;
	GLuint m_IndexBufferID = GL_FALSE;

public:
	std::vector<ShapeVertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;

	btTransform m_ShapeTransform;

protected:
	virtual void CreateVertexBuffer() = 0;
	virtual void CreateIndexBuffer() = 0;
	virtual void SetupBuffers();

	void CalculateNormals();

public:
	GLShape() = delete;
	GLShape(const glm::vec3& color);
	virtual ~GLShape();

	/*GLShape(const GLShape& glShape);
	GLShape& operator=(const GLShape& glShape);*/

	virtual void DrawShape(GLuint shaderProgramID, const char* uniformName);
	virtual bool IsMatchWithCollisionShape(btCollisionShape* collisionShape) const { return true; };

	static void ResetShaderModelMatrix(GLuint shaderProgramID, const char* uniformModelMatrixName);

	inline void ApplyTransform(const btTransform& transform) { m_ShapeTransform *= transform; }
};
