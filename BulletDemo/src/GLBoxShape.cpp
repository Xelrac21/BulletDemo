#include "GLBoxShape.h"

GLBoxShape::GLBoxShape(float halfExtentsX, float halfExtentsY, float halfExtentsZ, const glm::vec3& color)
	: m_halfExtentsX(halfExtentsX)
	, m_halfExtentsY(halfExtentsY)
	, m_halfExtentsZ(halfExtentsZ)
	, GLShape(color)
{
	CreateVertexBuffer();
	CreateIndexBuffer();

	CalculateNormals();
	SetupBuffers();
}

GLBoxShape::~GLBoxShape()
{

}

void GLBoxShape::CreateVertexBuffer()
{
	float vertexBuffer[24] = {
		-m_halfExtentsX, -m_halfExtentsY,  m_halfExtentsZ, //forward bottom left	0
		 m_halfExtentsX, -m_halfExtentsY,  m_halfExtentsZ, //forward bottom right	1
		-m_halfExtentsX,  m_halfExtentsY,  m_halfExtentsZ, //forward top left		2
		 m_halfExtentsX,  m_halfExtentsY,  m_halfExtentsZ, //forward top right		3
		-m_halfExtentsX, -m_halfExtentsY, -m_halfExtentsZ, //back bottom left		4
		 m_halfExtentsX, -m_halfExtentsY, -m_halfExtentsZ, //back bottom right		5
		-m_halfExtentsX,  m_halfExtentsY, -m_halfExtentsZ, //back top left			6
		 m_halfExtentsX,  m_halfExtentsY, -m_halfExtentsZ  //back top right			7
	};

	ShapeVertex vertex;
	for (unsigned int i = 0; i < 24; i += 3)
	{
		vertex.position.x = vertexBuffer[i];
		vertex.position.y = vertexBuffer[i + 1];
		vertex.position.z = vertexBuffer[i + 2];
		m_VertexBuffer.push_back(vertex);
	}

	m_NumVerts = m_VertexBuffer.size();
}

void GLBoxShape::CreateIndexBuffer()
{
	unsigned int indexBuffer[36] = {
		0, 4, 1,    1, 4, 5,    2, 3, 6,    3, 7, 6, //y axis faces
		0, 2, 6,    0, 6, 4,    1, 7, 3,    1, 5, 7, //x axis faces
		0, 1, 2,    1, 3, 2,    4, 6, 5,    5, 6, 7  //z axis faces
	};

	m_IndexBuffer = std::vector<unsigned int>(indexBuffer, indexBuffer + 36);
	m_NumIndices = m_IndexBuffer.size();
}

bool GLBoxShape::IsMatchWithCollisionShape(btCollisionShape* collisionShape) const
{
	int shapeType = collisionShape->getShapeType();
	return shapeType == BOX_SHAPE_PROXYTYPE;
}