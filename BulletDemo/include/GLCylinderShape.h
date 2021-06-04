#pragma once

#include "GLShape.h"

class GLCylinderShape : public GLShape
{
private:
	float m_Radius1;
	float m_Radius2;
	float m_HalfLength;

	unsigned int m_NumLats;
	unsigned int m_NumLongs;

protected:
	virtual void CreateVertexBuffer() override;
	virtual void CreateIndexBuffer() override;

public:
	GLCylinderShape() = delete;
	GLCylinderShape(float radius1, float radius2, float halfLength, unsigned int numLats, unsigned int numLongs, const glm::vec3& color, const glm::vec3& alignToNormal = glm::vec3(0.0, 0.0, 1.0));
	~GLCylinderShape();

	virtual bool IsMatchWithCollisionShape(btCollisionShape* collisionShape) const override;
};
