#pragma once

#include "utils.h"

class SphereProjectile
{
private:
	btCollisionShape* m_SphereProjectileCollisionShape;
	float m_SphereProjectileRadius;

public:
	GLSphereShape m_SphereProjectileGLShape;

public:
	SphereProjectile() = delete;
	SphereProjectile(float sphereProjectileRadius);
	~SphereProjectile();

	btRigidBody* CreateSphereProjectile(float sphereProjectileMass, const btVector3& position = btVector3(0.0, 0.0, 0.0));
	btRigidBody* CreateSphereProjectileAndApplyTransform(float sphereProjectileMass, const btTransform& transform);

	void CreateAndShootSphereProjectile(btDiscreteDynamicsWorld* world, float sphereProjectileMass, const btVector3& force, const btVector3& position);
	void CreateAndShootSphereProjectile(btDiscreteDynamicsWorld* world, float sphereProjectileMass, const btVector3& force, const btTransform& transform);
};
