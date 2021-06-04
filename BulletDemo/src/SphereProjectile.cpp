#include "SphereProjectile.h"

SphereProjectile::SphereProjectile(float sphereProjectileRadius)
	: m_SphereProjectileRadius(sphereProjectileRadius)
	, m_SphereProjectileGLShape(m_SphereProjectileRadius, 25, 25, glm::vec3(1.0, 0.41, 0.71))
{
	m_SphereProjectileCollisionShape = new btSphereShape(m_SphereProjectileRadius);
}

SphereProjectile::~SphereProjectile()
{
	delete m_SphereProjectileCollisionShape;
}

btRigidBody* SphereProjectile::CreateSphereProjectile(float sphereProjectileMass, const btVector3& position)
{
	btRigidBody* sphereProjectileRB = CreateRigidBody(m_SphereProjectileCollisionShape, position, sphereProjectileMass, &m_SphereProjectileGLShape);
	if (sphereProjectileRB != nullptr)
	{
		//enable continuous collision detection
		sphereProjectileRB->setCcdMotionThreshold(1e-7);
		sphereProjectileRB->setCcdSweptSphereRadius(0.17);
	}

	return sphereProjectileRB;
}

btRigidBody* SphereProjectile::CreateSphereProjectileAndApplyTransform(float sphereProjectileMass, const btTransform& transform)
{
	btRigidBody* sphereProjectileRB = CreateSphereProjectile(sphereProjectileMass, transform.getOrigin());
	if (sphereProjectileRB != nullptr)
	{
		if (btMotionState* motionState = sphereProjectileRB->getMotionState())
		{
			motionState->setWorldTransform(transform);
		}

		sphereProjectileRB->setWorldTransform(transform);
	}

	return sphereProjectileRB;
}

void SphereProjectile::CreateAndShootSphereProjectile(btDiscreteDynamicsWorld* world, float sphereProjectileMass, const btVector3& force, const btVector3& position)
{
	if (btRigidBody* sphereProjectileRB = CreateSphereProjectile(sphereProjectileMass, position))
	{
		world->addRigidBody(sphereProjectileRB);

		//cannonBallRB->applyCentralForce(cannonBallMass * cannonBallAcceleration); //central force is more appropriate for forces being applied over many time steps
		sphereProjectileRB->applyCentralImpulse(force);
	}
}

void SphereProjectile::CreateAndShootSphereProjectile(btDiscreteDynamicsWorld* world, float sphereProjectileMass, const btVector3& force, const btTransform& transform)
{
	if (btRigidBody* sphereProjectileRB = CreateSphereProjectileAndApplyTransform(sphereProjectileMass, transform))
	{
		world->addRigidBody(sphereProjectileRB);

		//cannonBallRB->applyCentralForce(cannonBallMass * cannonBallAcceleration); //central force is more appropriate for forces being applied over many time steps
		sphereProjectileRB->applyCentralImpulse(force);
	}
}