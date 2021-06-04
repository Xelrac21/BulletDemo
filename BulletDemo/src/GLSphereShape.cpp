#include "GLSphereShape.h"

GLSphereShape::GLSphereShape(float radius, unsigned int numLats, unsigned int numLongs, const glm::vec3& color)
	: m_Radius(radius)
	, m_NumLats(numLats)
	, m_NumLongs(numLongs)
	, GLShape(color)
{
	CreateVertexBuffer();
	CreateIndexBuffer();

	CalculateNormals();
	SetupBuffers();
}

GLSphereShape::~GLSphereShape()
{

}

void GLSphereShape::CreateVertexBuffer()
{
	float latSectionLength = glm::pi<float>() / (m_NumLats + 1);
	float longitudeSectionLength = 2.0f * glm::pi<float>() / m_NumLongs;

	ShapeVertex vertex;
	float currentPhi, currentTheta, currentRsinPhi, xCoord, yCoord, zCoord;
	for (unsigned int i = 0; i < m_NumLats; ++i)
	{
		currentPhi = i * latSectionLength + latSectionLength;
		currentRsinPhi = m_Radius * sin(currentPhi);
		zCoord = m_Radius * cos(currentPhi);
		for (unsigned int j = 0; j < m_NumLongs; ++j)
		{
			currentTheta = j * longitudeSectionLength;
			xCoord = currentRsinPhi * cos(currentTheta);
			yCoord = currentRsinPhi * sin(currentTheta);

			vertex.position.x = xCoord;
			vertex.position.y = yCoord;
			vertex.position.z = zCoord;
			m_VertexBuffer.push_back(vertex);
		}
	}

	//top vertex
	vertex.position.x = 0.0f;
	vertex.position.y = 0.0f;
	vertex.position.z = m_Radius;
	m_VertexBuffer.push_back(vertex);

	//bottom vertex
	vertex.position.z = -m_Radius;
	m_VertexBuffer.push_back(vertex);

	m_NumVerts = m_VertexBuffer.size();
}

void GLSphereShape::CreateIndexBuffer()
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

				m_IndexBuffer.push_back(triangleIndex2);
				m_IndexBuffer.push_back(i * m_NumLongs + m_NumLongs);
				m_IndexBuffer.push_back(i * m_NumLongs);
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

bool GLSphereShape::IsMatchWithCollisionShape(btCollisionShape* collisionShape) const
{
	int shapeType = collisionShape->getShapeType();
	return shapeType == SPHERE_SHAPE_PROXYTYPE;
}