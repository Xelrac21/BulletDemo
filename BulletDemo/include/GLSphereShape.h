#pragma once

#include "GLShape.h"

class GLSphereShape : public GLShape
{
private:
	float m_Radius;

	unsigned int m_NumLats;
	unsigned int m_NumLongs;

protected:
	virtual void CreateVertexBuffer() override;
	virtual void CreateIndexBuffer() override;

public:
	GLSphereShape() = delete;
	GLSphereShape(float radius, unsigned int numLats, unsigned int numLongs, const glm::vec3& color);
	~GLSphereShape();

	virtual bool IsMatchWithCollisionShape(btCollisionShape* collisionShape) const override;
};