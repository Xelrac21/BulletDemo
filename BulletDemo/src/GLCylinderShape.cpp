#include "GLCylinderShape.h"

GLCylinderShape::GLCylinderShape(float radius1, float radius2, float halfLength, unsigned int numLats, unsigned int numLongs, const glm::vec3& color, const glm::vec3& alignToNormal /* = glm::vec3(0.0, 0.0, 1.0) */)
	: m_Radius1(radius1)
	, m_Radius2(radius2)
	, m_HalfLength(halfLength)
	, m_NumLats(numLats)
	, m_NumLongs(numLongs)
	, GLShape(color)
{
	CreateVertexBuffer();
	CreateIndexBuffer();

	//rotate to normal
	float rotationAngle = glm::angle(alignToNormal, glm::vec3(0.0, 0.0, 1.0));
	if (rotationAngle != 0.0)
	{
		glm::vec3 axisOfRotation = glm::cross(alignToNormal, glm::vec3(0.0, 0.0, 1.0));
		for (unsigned int i = 0; i < m_VertexBuffer.size(); ++i)
		{
			m_VertexBuffer[i].position = glm::rotate(m_VertexBuffer[i].position, rotationAngle, axisOfRotation);
		}
	}

	CalculateNormals();
	SetupBuffers();
}

GLCylinderShape::~GLCylinderShape()
{

}

void GLCylinderShape::CreateVertexBuffer()
{
	float radiusDiff = m_Radius1 - m_Radius2;
	float radiusStep = radiusDiff / m_NumLats;
	float latSectionLength = (2.0f * m_HalfLength) / (m_NumLats - 1);
	float longitudeSectionLength = 2.0f * glm::pi<float>() / m_NumLongs;

	ShapeVertex vertex;
	float currentLatRadius, currentLatZPos, currentTheta, xCoord, yCoord;
	for (unsigned int i = 0; i < m_NumLats; ++i)
	{
		currentLatRadius = m_Radius1 - (radiusStep * i);
		currentLatZPos = m_HalfLength - i * latSectionLength;
		for (unsigned int j = 0; j < m_NumLongs; ++j)
		{
			currentTheta = j * longitudeSectionLength;
			xCoord = currentLatRadius * glm::cos(currentTheta);
			yCoord = currentLatRadius * glm::sin(currentTheta);

			vertex.position.x = xCoord;
			vertex.position.y = yCoord;
			vertex.position.z = currentLatZPos;

			m_VertexBuffer.push_back(vertex);
		}
	}

	//top vertex
	vertex.position.x = 0.0f;
	vertex.position.y = 0.0f;
	vertex.position.z = m_HalfLength;
	m_VertexBuffer.push_back(vertex);

	//bottom vertex
	vertex.position.z = -m_HalfLength;
	m_VertexBuffer.push_back(vertex);

	m_NumVerts = m_VertexBuffer.size();
}

void GLCylinderShape::CreateIndexBuffer()
{
	unsigned int triangleIndex1, triangleIndex2;
	for (unsigned int i = 0; i < m_NumLats - 1; ++i)
	{
		triangleIndex1 = i * m_NumLongs;
		triangleIndex2 = triangleIndex1 + m_NumLongs;
		for (unsigned int j = 0; j < m_NumLongs; ++j, ++triangleIndex1, ++triangleIndex2)
		{
			if (j == m_NumLongs - 1)
			{
				m_IndexBuffer.push_back(triangleIndex1);
				m_IndexBuffer.push_back(triangleIndex2);
				m_IndexBuffer.push_back(i * m_NumLongs);

				m_IndexBuffer.push_back(i * m_NumLongs);
				m_IndexBuffer.push_back(triangleIndex2);
				m_IndexBuffer.push_back(i * m_NumLongs + m_NumLongs);
			}
			else
			{
				m_IndexBuffer.push_back(triangleIndex1);
				m_IndexBuffer.push_back(triangleIndex2);
				m_IndexBuffer.push_back(triangleIndex1 + 1);

				m_IndexBuffer.push_back(triangleIndex1 + 1);
				m_IndexBuffer.push_back(triangleIndex2);
				m_IndexBuffer.push_back(triangleIndex2 + 1);
			}
		}
	}

	unsigned int topVertexIndex = m_NumVerts - 2;
	unsigned int bottomVertexIndex = m_NumVerts - 1;
	unsigned int lastLatitudeStartIndex = topVertexIndex - m_NumLongs;
	for (unsigned int i = 0, lastLatIndex = lastLatitudeStartIndex; i < m_NumLongs; ++i, ++lastLatIndex)
	{
		if (i == m_NumLongs - 1)
		{
			m_IndexBuffer.push_back(i);
			m_IndexBuffer.push_back(0);
			m_IndexBuffer.push_back(topVertexIndex);

			m_IndexBuffer.push_back(lastLatIndex);
			m_IndexBuffer.push_back(bottomVertexIndex);
			m_IndexBuffer.push_back(lastLatitudeStartIndex);
		}
		else
		{
			m_IndexBuffer.push_back(i);
			m_IndexBuffer.push_back(i + 1);
			m_IndexBuffer.push_back(topVertexIndex);

			m_IndexBuffer.push_back(lastLatIndex);
			m_IndexBuffer.push_back(bottomVertexIndex);
			m_IndexBuffer.push_back(lastLatIndex + 1);
		}
	}

	m_NumIndices = m_IndexBuffer.size();
}

bool GLCylinderShape::IsMatchWithCollisionShape(btCollisionShape* collisionShape) const
{
	int shapeType = collisionShape->getShapeType();
	return shapeType == CYLINDER_SHAPE_PROXYTYPE;
}