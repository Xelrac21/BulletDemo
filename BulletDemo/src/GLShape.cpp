#include "GLShape.h"

GLShape::GLShape(const glm::vec3& color)
	: m_ShapeColor(color)
{
	m_ShapeTransform.setIdentity();
}

GLShape::~GLShape()
{
	glDeleteVertexArrays(1, &m_VertexArrayID);
	glDeleteBuffers(1, &m_VertexBufferID);
	glDeleteBuffers(1, &m_IndexBufferID);
}

/*
GLShape::GLShape(const GLShape& glShape)
{

}

GLShape::operator=(const GLShape& glShape)
{

}*/

void GLShape::SetupBuffers()
{
	glGenVertexArrays(1, &m_VertexArrayID);
	glBindVertexArray(m_VertexArrayID);

	//vertex buffer
	glGenBuffers(1, &m_VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, m_NumVerts * sizeof(ShapeVertex), &m_VertexBuffer[0], GL_STATIC_DRAW);

	//index buffer
	glGenBuffers(1, &m_IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_NumIndices * sizeof(unsigned int), &m_IndexBuffer[0], GL_STATIC_DRAW);

	//vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, ShapeVertex::position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, ShapeVertex::normal));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void GLShape::DrawShape(GLuint shaderProgramID, const char* uniformName)
{
	glUniform4f(glGetUniformLocation(shaderProgramID, "u_InputColor"), m_ShapeColor.x, m_ShapeColor.y, m_ShapeColor.z, 1.0f);

	if (m_VertexArrayID == GL_FALSE)
		return;

	float modelOpenGLMatrix[16];
	m_ShapeTransform.getOpenGLMatrix(modelOpenGLMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, uniformName), 1, GL_FALSE, modelOpenGLMatrix);

	glBindVertexArray(m_VertexArrayID);
	glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	m_ShapeTransform.setIdentity();
}

void GLShape::ResetShaderModelMatrix(GLuint shaderProgramID, const char* uniformModelMatrixName)
{
	glm::mat4 modelMatrix(1.0f);
	unsigned int modelMatrixID = glGetUniformLocation(shaderProgramID, uniformModelMatrixName);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void GLShape::CalculateNormals()
{
	for (unsigned int i = 0; i < m_NumIndices; i += 3)
	{
		unsigned int index0 = m_IndexBuffer[i];
		unsigned int index1 = m_IndexBuffer[i + 1];
		unsigned int index2 = m_IndexBuffer[i + 2];

		glm::vec3 vector10 = m_VertexBuffer[index1].position - m_VertexBuffer[index0].position;
		glm::vec3 vector20 = m_VertexBuffer[index2].position - m_VertexBuffer[index0].position;

		glm::vec3 vertexNormal = glm::cross(vector10, vector20);
		vertexNormal = glm::normalize(vertexNormal);

		m_VertexBuffer[index0].normal += vertexNormal;
		m_VertexBuffer[index1].normal += vertexNormal;
		m_VertexBuffer[index2].normal += vertexNormal;
	}

	for (unsigned int i = 0; i < m_NumVerts; ++i)
	{
		m_VertexBuffer[i].normal = glm::normalize(m_VertexBuffer[i].normal);
	}
}