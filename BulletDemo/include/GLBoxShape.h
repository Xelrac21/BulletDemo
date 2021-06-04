#pragma once

#include "GLShape.h"

class GLBoxShape : public GLShape
{
private:
	float m_halfExtentsX = 0.0;
	float m_halfExtentsY = 0.0;
	float m_halfExtentsZ = 0.0;

protected:
	virtual void CreateVertexBuffer() override;
	virtual void CreateIndexBuffer() override;

public:
	GLBoxShape() = delete;
	GLBoxShape(float halfExtentsX, float halfExtentsY, float halfExtentsZ, const glm::vec3& color);
	~GLBoxShape();

	virtual bool IsMatchWithCollisionShape(btCollisionShape* collisionShape) const override;
};