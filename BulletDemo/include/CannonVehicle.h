#pragma once

#include "utils.h"
#include "SphereProjectile.h"

class CannonVehicle
{
private:
	struct BulletVehicleHandlingValues
	{
		//vehicle steering and movement values
		float steeringValue = 0.0f;
		const float steeringIncrement = 0.04f;
		const float steeringClamp = 0.4f;

		float engineForce = 0.0f;
		const float maxEngineForce = 800.0f;
		const float maxBrakingForce = 20.0f;
		const float brakingIncrement = 0.2f;
		float brakingForce = maxBrakingForce;
	};

private:
	BulletVehicleHandlingValues m_CannonHandlingValues;

	float m_CannonBarrelHalfLength;
	float m_CannonBarrelBackRadius;
	float m_CannonBarrelFrontRadius;

	float m_CannonButtRadius;

	float m_CannonBaseHalfLength;
	float m_CannonBaseHalfHeight;
	float m_CannonBaseHalfWidth;

	float m_CannonWheelRadius;
	float m_CannonWheelHalfLength;

	int m_CannonBarrelChildIndex;
	int m_CannonButtIndex;

	btCompoundShape* m_CannonCollisionShape = nullptr;
	btCollisionShape* m_CannonBarrelCollisionShape = nullptr;
	btCollisionShape* m_CannonButtCollisionShape = nullptr;
	btCollisionShape* m_CannonBaseCollisionShape = nullptr;

	btRigidBody* m_CannonRB = nullptr;
	btRaycastVehicle* m_CannonVehicle = nullptr;

public:
	GLCylinderShape m_CannonBarrelGLShape;
	GLSphereShape m_CannonButtGLShape;
	GLBoxShape m_CannonBaseGLShape;
	GLCylinderShape m_CannonWheelGLShape;

	SphereProjectile m_CannonBallProjectile;

private:
	void SetupCannonRigidBody(btDiscreteDynamicsWorld* world);
	void SetupCannonVehicle(btDiscreteDynamicsWorld* world);
	void RotateCannonBarrelInput(GLFWwindow* window, float deltaTime);

public:
	CannonVehicle() = delete;
	CannonVehicle(btDiscreteDynamicsWorld* world, float cannonBarrelHalfLength, float cannonBarrelBackRadius, float cannonBarrelFrontRadius, float cannonBaseHalfLength, float cannonBaseHalfHeight, float cannonBaseHalfWidth, float cannonWheelRadius, float cannonWheelHalfLength);
	~CannonVehicle();

	inline btRaycastVehicle* GetRaycastVehicle() { return m_CannonVehicle; }

	void HandleCannonInputs(GLFWwindow* window, float deltaTime);
	void ShootFromBarrel(btDiscreteDynamicsWorld* world, float cannonBallMass);
	void DrawWheels(GLuint shaderProgram);
};
