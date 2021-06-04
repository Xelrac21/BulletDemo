#include "CannonVehicle.h"

CannonVehicle::CannonVehicle(btDiscreteDynamicsWorld* world, float cannonBarrelHalfLength, float cannonBarrelBackRadius, float cannonBarrelFrontRadius, float cannonBaseHalfLength, float cannonBaseHalfHeight, float cannonBaseHalfWidth, float cannonWheelRadius, float cannonWheelHalfLength)
	: m_CannonBarrelHalfLength(cannonBarrelHalfLength)
	, m_CannonBarrelBackRadius(cannonBarrelBackRadius)
	, m_CannonBarrelFrontRadius(cannonBarrelFrontRadius)
	, m_CannonButtRadius(m_CannonBarrelBackRadius - 0.01)
	, m_CannonBaseHalfLength(cannonBaseHalfLength)
	, m_CannonBaseHalfHeight(cannonBaseHalfHeight)
	, m_CannonBaseHalfWidth(cannonBaseHalfWidth)
	, m_CannonWheelRadius(cannonWheelRadius)
	, m_CannonWheelHalfLength(cannonWheelHalfLength)
	, m_CannonBarrelGLShape(m_CannonBarrelFrontRadius, m_CannonBarrelBackRadius, m_CannonBarrelHalfLength, 10, 25, glm::vec3(0.0, 1.0, 0.0))
	, m_CannonButtGLShape(m_CannonButtRadius, 25, 25, glm::vec3(0.0, 0.0, 1.0))
	, m_CannonBaseGLShape(m_CannonBaseHalfWidth, m_CannonBaseHalfHeight, m_CannonBaseHalfLength, glm::vec3(1.0, 0.0, 0.0))
	, m_CannonWheelGLShape(m_CannonWheelRadius, m_CannonWheelRadius, m_CannonWheelHalfLength, 4, 25, glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 0.0, 0.0))
	, m_CannonBallProjectile(m_CannonBarrelFrontRadius)
{
#ifdef CANNON_RB
	SetupCannonRigidBody(world);
#endif

#ifdef CANNON_VEHICLE
	SetupCannonVehicle(world);
#endif
}

CannonVehicle::~CannonVehicle()
{
	delete m_CannonCollisionShape;
	delete m_CannonBarrelCollisionShape;
	delete m_CannonButtCollisionShape;
	delete m_CannonBaseCollisionShape;

	delete m_CannonRB;
	delete m_CannonVehicle;
}

void CannonVehicle::SetupCannonRigidBody(btDiscreteDynamicsWorld* world)
{
	//create needed collision shapes
	m_CannonBarrelCollisionShape = new btCylinderShapeZ(btVector3(m_CannonBarrelBackRadius, m_CannonBarrelBackRadius, m_CannonBarrelHalfLength));
	m_CannonButtCollisionShape = new btSphereShape(m_CannonButtRadius);
	m_CannonBaseCollisionShape = new btBoxShape(btVector3(m_CannonBaseHalfWidth, m_CannonBaseHalfHeight, m_CannonBaseHalfLength));

	m_CannonCollisionShape = new btCompoundShape();

	//associate each child collision shape with a GLShape since a rigid body only has one user pointer
	m_CannonBarrelCollisionShape->setUserPointer(&m_CannonBarrelGLShape);
	m_CannonButtCollisionShape->setUserPointer(&m_CannonButtGLShape);
	m_CannonBaseCollisionShape->setUserPointer(&m_CannonBaseGLShape);

	//indices of child shapes in the order they are added
	m_CannonBarrelChildIndex = 0;
	m_CannonButtIndex = 1;

	//arrange cannon child shapes
	btTransform cannonTransform;
	cannonTransform.setIdentity();
	cannonTransform.setOrigin(btVector3(0, 1, 0));
	m_CannonCollisionShape->addChildShape(cannonTransform, m_CannonBarrelCollisionShape);

	cannonTransform.setIdentity();
	cannonTransform.setOrigin(btVector3(0, 1, -m_CannonBarrelHalfLength));
	m_CannonCollisionShape->addChildShape(cannonTransform, m_CannonButtCollisionShape);

	cannonTransform.setIdentity();
	cannonTransform.setOrigin(btVector3(0, 1 - m_CannonBarrelBackRadius - m_CannonBaseHalfHeight, -0.3));
	m_CannonCollisionShape->addChildShape(cannonTransform, m_CannonBaseCollisionShape);

	m_CannonRB = CreateRigidBody(m_CannonCollisionShape, btVector3(0, 0.2, 0), btScalar(450));
	if (m_CannonRB != nullptr)
	{
		world->addRigidBody(m_CannonRB);
	}
}

void CannonVehicle::SetupCannonVehicle(btDiscreteDynamicsWorld* world)
{
	btRaycastVehicle::btVehicleTuning vehicleTuning;
	btVehicleRaycaster* vehicleRayCaster = new btDefaultVehicleRaycaster(world);
	m_CannonVehicle = new btRaycastVehicle(vehicleTuning, m_CannonRB, vehicleRayCaster);
	world->addVehicle(m_CannonVehicle);

	//rb gets deactivated when not moving anymore, which causes the cannon vehicle to not be able to move unless the rigid body is reactivated
	if (m_CannonRB != nullptr)
		m_CannonRB->setActivationState(DISABLE_DEACTIVATION);

	m_CannonVehicle->setCoordinateSystem(0, 1, 2);

	btVector3 wheelAxle(-1.0, 0.0, 0.0);
	btVector3 wheelDirection(0.0, -1.0, 0.0);
	btScalar suspensionRestLength(0.6);
	float wheelXPos = m_CannonBaseHalfWidth + m_CannonWheelRadius + 0.05;
	float wheelYPos = 0.8f;
	float wheelZPos = m_CannonBaseHalfLength;

	//front left
	btVector3 connectionPoint(wheelXPos, wheelYPos, wheelZPos + 0.3);
	m_CannonVehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, m_CannonWheelRadius, vehicleTuning, true);

	//front right
	connectionPoint = btVector3(-wheelXPos, wheelYPos, wheelZPos + 0.3);
	m_CannonVehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, m_CannonWheelRadius, vehicleTuning, true);

	//back right
	connectionPoint = btVector3(-wheelXPos, wheelYPos, -wheelZPos - 0.3);
	m_CannonVehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, m_CannonWheelRadius, vehicleTuning, false);

	//back left
	connectionPoint = btVector3(wheelXPos, wheelYPos, -wheelZPos - 0.3);
	m_CannonVehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, m_CannonWheelRadius, vehicleTuning, false);

	for (int i = 0; i < m_CannonVehicle->getNumWheels(); ++i)
	{
		//values from Bullet ForkLiftDemo.cpp
		btWheelInfo& wheel = m_CannonVehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 20.f;
		wheel.m_wheelsDampingRelaxation = 2.3f;
		wheel.m_wheelsDampingCompression = 4.4f;
		wheel.m_frictionSlip = 1000.0f;
		wheel.m_rollInfluence = 0.1f;
	}
}

void CannonVehicle::HandleCannonInputs(GLFWwindow* window, float deltaTime)
{
	//rotating cannon barrel
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		RotateCannonBarrelInput(window, deltaTime);
	}

#ifdef CANNON_VEHICLE
	//vehicle move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_CannonHandlingValues.engineForce = m_CannonHandlingValues.maxEngineForce;
		m_CannonHandlingValues.brakingForce = 0.0f;
	}

	//vehicle move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_CannonHandlingValues.engineForce = -m_CannonHandlingValues.maxEngineForce;
		m_CannonHandlingValues.brakingForce = 0.0f;
	}

	//vehicle brake
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		m_CannonHandlingValues.engineForce = 0.0f;
		m_CannonHandlingValues.brakingForce += m_CannonHandlingValues.brakingIncrement;
		if (m_CannonHandlingValues.brakingForce > m_CannonHandlingValues.maxBrakingForce)
			m_CannonHandlingValues.brakingForce = m_CannonHandlingValues.maxBrakingForce;
	}

	//vehicle steer left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		m_CannonHandlingValues.steeringValue += m_CannonHandlingValues.steeringIncrement;
		if (m_CannonHandlingValues.steeringValue > m_CannonHandlingValues.steeringClamp)
			m_CannonHandlingValues.steeringValue = m_CannonHandlingValues.steeringClamp;
	}

	//vehicle steer right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		m_CannonHandlingValues.steeringValue -= m_CannonHandlingValues.steeringIncrement;
		if (m_CannonHandlingValues.steeringValue < -m_CannonHandlingValues.steeringClamp)
			m_CannonHandlingValues.steeringValue = -m_CannonHandlingValues.steeringClamp;
	}

	//apply vehicle handling
	for (int i = 0; i < m_CannonVehicle->getNumWheels(); ++i)
	{
		btWheelInfo& wheelInfo = m_CannonVehicle->getWheelInfo(i);
		wheelInfo.m_wheelsSuspensionForce;

		if (i == 2 || i == 3)
		{
			m_CannonVehicle->applyEngineForce(m_CannonHandlingValues.engineForce, i);
			m_CannonVehicle->setBrake(m_CannonHandlingValues.brakingForce, i);
		}
		else
		{
			m_CannonVehicle->setSteeringValue(m_CannonHandlingValues.steeringValue, i);
		}
	}
#endif
}

void CannonVehicle::RotateCannonBarrelInput(GLFWwindow* window, float deltaTime)
{
	if (m_CannonCollisionShape == nullptr)
		return;

	btTransform cannonPartTransform = m_CannonCollisionShape->getChildTransform(m_CannonBarrelChildIndex);
	glm::mat4 glCannonPartTransform;
	cannonPartTransform.getOpenGLMatrix(glm::value_ptr(glCannonPartTransform));

	float yaw, pitch, roll;
	GetYawPitchAndRoll(glCannonPartTransform, yaw, pitch, roll);

	float rotationAngleY = 0.0f;
	float rotationAngleX = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && pitch < glm::radians(60.0f))
	{
		rotationAngleY = deltaTime;
	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && pitch > 0.0f)
	{
		rotationAngleY = -deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && yaw > glm::radians(-45.0f))
	{
		rotationAngleX = deltaTime;
	}
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && yaw < glm::radians(45.0f))
	{
		rotationAngleX = -deltaTime;
	}

	//rotate cannon barrel
	float originOffset = 0.1f;
	glm::vec3 originVector(0.0f, 0.0f, m_CannonBarrelHalfLength - originOffset);

	glCannonPartTransform = glm::translate(glCannonPartTransform, -originVector);
	glCannonPartTransform = glm::rotate(glCannonPartTransform, rotationAngleY, glm::vec3(-1.0, 0.0, 0.0));
	glCannonPartTransform = glm::rotate(glCannonPartTransform, rotationAngleX, glm::vec3(0.0, 1.0, 0.0));
	glCannonPartTransform = glm::translate(glCannonPartTransform, originVector);

	cannonPartTransform.setFromOpenGLMatrix(glm::value_ptr(glCannonPartTransform));
	m_CannonCollisionShape->updateChildTransform(m_CannonBarrelChildIndex, cannonPartTransform);

	//rotate cannon butt
	cannonPartTransform = m_CannonCollisionShape->getChildTransform(m_CannonButtIndex);
	cannonPartTransform.getOpenGLMatrix(glm::value_ptr(glCannonPartTransform));

	originVector = glm::vec3(0.0f, 0.0f, -originOffset);
	glCannonPartTransform = glm::translate(glCannonPartTransform, -originVector);
	glCannonPartTransform = glm::rotate(glCannonPartTransform, rotationAngleY, glm::vec3(-1.0, 0.0, 0.0));
	glCannonPartTransform = glm::rotate(glCannonPartTransform, rotationAngleX, glm::vec3(0.0, 1.0, 0.0));
	glCannonPartTransform = glm::translate(glCannonPartTransform, originVector);

	cannonPartTransform.setFromOpenGLMatrix(glm::value_ptr(glCannonPartTransform));
	m_CannonCollisionShape->updateChildTransform(m_CannonButtIndex, cannonPartTransform);
}

void CannonVehicle::ShootFromBarrel(btDiscreteDynamicsWorld* world, float cannonBallMass)
{
	if (m_CannonRB == nullptr)
		return;

	btTransform cannonRBTransform = m_CannonRB->getWorldTransform();
	btTransform cannonBarrelTransform = m_CannonCollisionShape->getChildTransform(m_CannonBarrelChildIndex);

	btTransform cannonTipTranslation;
	cannonTipTranslation.setIdentity();
	btVector3 forwardVector(0.0f, 0.0f, m_CannonBarrelHalfLength);
	forwardVector = cannonBarrelTransform.getBasis() * forwardVector;
	cannonTipTranslation.setOrigin(forwardVector);

	btTransform cannonBallTransform = cannonRBTransform * cannonTipTranslation * cannonBarrelTransform;
	btVector3 cannonBallAcceleration = 50.0f * (cannonRBTransform.getBasis() * forwardVector);

	m_CannonBallProjectile.CreateAndShootSphereProjectile(world, cannonBallMass, cannonBallMass * cannonBallAcceleration, cannonBallTransform);
}

void CannonVehicle::DrawWheels(GLuint shaderProgram)
{
	if (m_CannonVehicle == nullptr)
		return;

	for (int i = 0; i < m_CannonVehicle->getNumWheels(); ++i)
	{
		btWheelInfo& wheel = m_CannonVehicle->getWheelInfo(i);
		btTransform wheelTransform = wheel.m_worldTransform;
		m_CannonWheelGLShape.ApplyTransform(wheelTransform);
		m_CannonWheelGLShape.DrawShape(shaderProgram, "u_ModelMatrix");
	}
}