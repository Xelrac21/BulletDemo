#include "GLPlaneShape.h"

GLPlaneShape::GLPlaneShape(float halfExtentsX, float halfExtentsZ, glm::vec3 normal, const glm::vec3& color)
	: m_halfExtentsX(halfExtentsX)
	, m_halfExtentsZ(halfExtentsZ)
	, m_Normal(glm::normalize(normal))
	, GLShape(color)
{
	CreateVertexBuffer();
	CreateIndexBuffer();

	CalculateNormals();
	SetupBuffers();
}

GLPlaneShape::~GLPlaneShape()
{

}

void GLPlaneShape::CreateVertexBuffer()
{
	glm::vec3 forwardLeft = glm::vec3(-m_halfExtentsX, 0, m_halfExtentsZ);
	glm::vec3 forwardRight = glm::vec3(m_halfExtentsX, 0, m_halfExtentsZ);
	glm::vec3 backwardLeft = glm::vec3(-m_halfExtentsX, 0, -m_halfExtentsZ);
	glm::vec3 backwardRight = glm::vec3(m_halfExtentsX, 0, -m_halfExtentsZ);

	float rotationAngle = glm::angle(m_Normal, glm::vec3(0.0, 1.0, 0.0));
	if (rotationAngle != 0)
	{
		glm::vec3 axisOfRotation = glm::cross(glm::vec3(0, 1, 0), m_Normal);

		forwardLeft = glm::rotate(forwardLeft, rotationAngle, axisOfRotation);
		forwardRight = glm::rotate(forwardRight, rotationAngle, axisOfRotation);
		backwardLeft = glm::rotate(backwardLeft, rotationAngle, axisOfRotation);
		backwardRight = glm::rotate(backwardRight, rotationAngle, axisOfRotation);
	}

	ShapeVertex vertex;
	vertex.position = forwardLeft;
	m_VertexBuffer.push_back(vertex);

	vertex.position = forwardRight;
	m_VertexBuffer.push_back(vertex);

	vertex.position = backwardLeft;
	m_VertexBuffer.push_back(vertex);

	vertex.position = backwardRight;
	m_VertexBuffer.push_back(vertex);

	m_NumVerts = m_VertexBuffer.size();
}

void GLPlaneShape::CreateIndexBuffer()
{
	unsigned int indexBuffer[6] = {
		0, 1, 3,
		0, 3, 2
	};

	m_IndexBuffer = std::vector<unsigned int>(indexBuffer, indexBuffer + 6);
	m_NumIndices = m_IndexBuffer.size();
}

bool GLPlaneShape::IsMatchWithCollisionShape(btCollisionShape* collisionShape) const
{
	int shapeType = collisionShape->getShapeType();
	return shapeType == STATIC_PLANE_PROXYTYPE;
}