#pragma once

#include "GLShape.h"

class GLPlaneShape : public GLShape
{
private:
	glm::vec3 m_Normal;
	float m_halfExtentsX;
	float m_halfExtentsZ;

protected:
	virtual void CreateVertexBuffer() override;
	virtual void CreateIndexBuffer() override;

public:
	GLPlaneShape() = delete;
	GLPlaneShape(float halfExtentsX, float halfExtentsZ, glm::vec3 normal, const glm::vec3& color);
	~GLPlaneShape();

	virtual bool IsMatchWithCollisionShape(btCollisionShape* collisionShape) const override;
};
