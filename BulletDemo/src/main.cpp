#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>

#include "utils.h"
#include "Camera.h"
#include "Lighting.h"
#include "CannonVehicle.h"
#include "SphereProjectile.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1280

GLFWwindow* InitDefaultGlFWwindow()
{
	if (glfwInit() != GLFW_TRUE)
		return nullptr;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bullet Demo", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	glEnable(GL_DEPTH_TEST);

	glfwSwapInterval(1);

	return window;
}

void InitDefaultPhysicsWorld(btDiscreteDynamicsWorld*& world, btCollisionDispatcher*& dispatcher, btBroadphaseInterface*& broadPhase, btConstraintSolver* constraintSolver, btCollisionConfiguration*& collisionConfig, const btVector3& gravity)
{
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);

	broadPhase = new btDbvtBroadphase();
	//broadPhase = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000)); //a broadphase alternative available in bullet physics

	constraintSolver = new btSequentialImpulseConstraintSolver();

	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, constraintSolver, collisionConfig);
	world->setGravity(gravity);
}

int main(void)
{
	GLFWwindow* window = InitDefaultGlFWwindow();
	if (window == nullptr)
		return -1;

	if (glewInit() != GLEW_OK)
		return -2;

	// Vertex Shader source code
	const char* vertexShaderSource = "#version 440 core\n"
		"layout (location = 0) in vec3 Position;\n"
		"layout (location = 1) in vec3 Normal;\n"
		"uniform mat4 camMatrix;\n"
		"uniform mat4 u_ModelMatrix;\n"
		"out vec3 Normal0;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = camMatrix * u_ModelMatrix * vec4(Position, 1.0);\n"
		"   Normal0 = (u_ModelMatrix * vec4(Normal, 0.0)).xyz;\n"
		"}\n\0";

	//Fragment Shader source code
	const char* fragmentShaderSource = "#version 440 core\n"
		"in vec3 Normal0;\n"
		"out vec4 FragColor;\n"
		"struct DirectionalLight\n"
		"{\n"
		"	vec3 Color;\n"
		"	float AmbientIntensity;\n"
		"	vec3 Direction;\n"
		"	float DiffuseIntensity;\n"
		"};\n"
		"uniform vec4 u_InputColor;\n"
		"uniform DirectionalLight u_DirectionalLight;\n"
		"void main()\n"
		"{\n"
		"   vec4 AmbientColor = vec4(u_DirectionalLight.Color * u_DirectionalLight.AmbientIntensity, 1.0f);\n"
		"   float DiffuseFactor = max(dot(normalize(Normal0), -u_DirectionalLight.Direction), 0.0f);\n"
		"	vec4 DiffuseColor = vec4(u_DirectionalLight.Color * u_DirectionalLight.DiffuseIntensity * DiffuseFactor, 1.0f);\n"
		"   FragColor = u_InputColor * (AmbientColor + DiffuseColor);\n"
		"}\n\0";

	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	DirectionalLight directionalLight;
	directionalLight.AmbientIntensity = 0.2f;
	directionalLight.DiffuseIntensity = 1.0f;
	directionalLight.Direction = glm::vec3(-0.5f, -1.0f, 0.0f);

	//Setup world
	btCollisionConfiguration* defaultCollisionConfig = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* constraintSolver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;
	InitDefaultPhysicsWorld(world, dispatcher, broadPhase, constraintSolver, defaultCollisionConfig, btVector3(0, -10, 0));

	//Collision shapes tracker
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	//Setup floor
	btVector3 worldFloorNormal(0, 1, 0);
	GLPlaneShape worldFloorGLShape(10.0, 10.0, glm::vec3(worldFloorNormal.x(), worldFloorNormal.y(), worldFloorNormal.z()), glm::vec3(0.4f, 0.4f, 0.4f));
	btStaticPlaneShape* worldFloor = new btStaticPlaneShape(worldFloorNormal, 0);
	//btBoxShape* worldFloor = new btBoxShape(btVector3(10.0f, 0.0f, 10.0f));
	collisionShapes.push_back(worldFloor);
	btRigidBody* floorRB = CreateRigidBody(worldFloor, btVector3(0, GROUND_HEIGHT, 0), btScalar(0), &worldFloorGLShape);
	if (floorRB != nullptr)
	{
		world->addRigidBody(floorRB);
	}

	//cannon dimensions
	float cannonBarrelHalfLength = 1.0f;
	float cannonBarrelBackRadius = 0.25f;
	float cannonBarrelFrontRadius = 0.18f;

	float cannonButtRadius = cannonBarrelBackRadius - 0.01;

	float cannonBaseHalfLength = 0.5f;
	float cannonBaseHalfHeight = 0.2f;
	float cannonBaseHalfWidth = 0.4f;

	float cannonWheelRadius = 0.2f;
	float cannonWheelHalfLength = 0.12f;

#ifndef CANNON_RB //enable just the shapes and not yet put together into one
	GLBoxShape cannonBaseGLShape(cannonBaseHalfWidth, cannonBaseHalfHeight, cannonBaseHalfLength, glm::vec3(1.0, 0.0, 0.0));
	GLCylinderShape cannonBarrelGLShape(cannonBarrelFrontRadius, cannonBarrelBackRadius, cannonBarrelHalfLength, 10, 25, glm::vec3(0.0, 1.0, 0.0));
	GLSphereShape cannonButtGLShape(cannonButtRadius, 25, 25, glm::vec3(0.0, 0.0, 1.0));

	//create cannon parts collision shapes
	btCollisionShape* cannonBarrel = new btCylinderShapeZ(btVector3(cannonBarrelBackRadius, cannonBarrelBackRadius, cannonBarrelHalfLength));
	btCollisionShape* cannonButt = new btSphereShape(cannonButtRadius);
	btCollisionShape* cannonBase = new btBoxShape(btVector3(cannonBaseHalfWidth, cannonBaseHalfHeight, cannonBaseHalfLength));
	collisionShapes.push_back(cannonBarrel);
	collisionShapes.push_back(cannonButt);
	collisionShapes.push_back(cannonBase);

	//create individual rigid bodies for each cannon part
	btRigidBody* cannonBaseRB = CreateRigidBody(cannonBase, btVector3(-0.6f, 0.7f, 0), btScalar(20), &cannonBaseGLShape);
	world->addRigidBody(cannonBaseRB);

	btRigidBody* cannonBarrelRB = CreateRigidBody(cannonBarrel, btVector3(2.0f, 2.0f, 2.0f), btScalar(20), &cannonBarrelGLShape);
	world->addRigidBody(cannonBarrelRB);

	btRigidBody* cannonButtRB = CreateRigidBody(cannonButt, btVector3(1.9f, 4.0f, 2.0f), btScalar(10), &cannonButtGLShape);
	world->addRigidBody(cannonButtRB);
#else // create cannon compount shape
	CannonVehicle cannonVehicle(world, cannonBarrelHalfLength, cannonBarrelBackRadius, cannonBarrelFrontRadius, cannonBaseHalfLength, cannonBaseHalfHeight, cannonBaseHalfWidth, cannonWheelRadius, cannonWheelHalfLength);
#endif //CANNON_RB

	bool shootFromCannon = false;
	SphereProjectile cannonBall(cannonBarrelFrontRadius);

	//obstacles tracker
	std::vector<GLBoxShape*> boxObstacles;

	//initialize camera
#ifdef CANNON_RB
	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 15.0f), window, cannonVehicle.GetRaycastVehicle());
#else
	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 15.0f), window);
#endif

	//clock
	btClock stepClock;
	btClock shootClock;
	btClock obstacleSpawnClock;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		btScalar stepTime = stepClock.getTimeSeconds();
		stepClock.reset();

		//spawn box obstacles in a ring around the origin
		if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS && obstacleSpawnClock.getTimeMilliseconds() > 500.0f)
		{
			GenerateBoxObstacles(world, collisionShapes, boxObstacles);
			obstacleSpawnClock.reset();
		}

#ifdef CANNON_RB
		cannonVehicle.HandleCannonInputs(window, stepTime);

		//enable or disable shoot from cannon
		if (shootClock.getTimeMilliseconds() >= 200.0f && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			shootFromCannon = !shootFromCannon;
			shootClock.reset();
		}
#endif

		//Create and shoot sphere
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && shootClock.getTimeMilliseconds() >= 500.0f)
		{
			btScalar cannonBallMass = 20.0f;

#ifdef CANNON_RB
			if (shootFromCannon)
			{
				cannonVehicle.ShootFromBarrel(world, cannonBallMass);
			}
			else
#endif
			{
				btVector3 cameraPosition(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
				btVector3 cannonBallAcceleration(camera.GetOrientation().x, camera.GetOrientation().y, camera.GetOrientation().z);
				cannonBallAcceleration = 30.0f * cannonBallMass * cannonBallAcceleration;

				cannonBall.CreateAndShootSphereProjectile(world, cannonBallMass, cannonBallAcceleration, cameraPosition);
			}
			shootClock.reset();
		}

		world->stepSimulation(stepTime);

		//clear frame for next drawing
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw lighting
		SetDirectionalLight(shaderProgram, directionalLight);
		
		//walk through all collision objects in world and render their gl shapes
		//if the collision object has a compound shape, walk through its child shapes and render their gl shapes
		btAlignedObjectArray<btCollisionObject*>& collisionObjects = world->getCollisionObjectArray();
		for (int i = 0; i < collisionObjects.size(); ++i)
		{
			btCollisionObject* collisionObject = collisionObjects[i];
			btRigidBody* collisionObjectRigidBody = btRigidBody::upcast(collisionObject);

			btTransform rbTransform;
			if (collisionObjectRigidBody != nullptr && collisionObjectRigidBody->getMotionState() != nullptr)
			{
				collisionObjectRigidBody->getMotionState()->getWorldTransform(rbTransform);
			}
			else
			{
				rbTransform = collisionObject->getWorldTransform();
			}

			btCollisionShape* rbCollisionShape = collisionObjectRigidBody->getCollisionShape();
			if (rbCollisionShape == nullptr)
				continue;

			//draw associated shape for each rigid body. If it is a compound shape, take into account the local positions of its child shapes
			const int rbShapeType = rbCollisionShape->getShapeType();
			if (rbShapeType == COMPOUND_SHAPE_PROXYTYPE)
			{
				if (btCompoundShape* rbCompoundShape = dynamic_cast<btCompoundShape*>(rbCollisionShape))
				{
					for (int j = 0; j < rbCompoundShape->getNumChildShapes(); ++j)
					{
						btCollisionShape* childShape = rbCompoundShape->getChildShape(j);
						if (childShape == nullptr)
							continue;

						if (GLShape* glShape = static_cast<GLShape*>(childShape->getUserPointer()))
						{
							btTransform childTransform = rbCompoundShape->getChildTransform(j);
							glShape->ApplyTransform(rbTransform);
							glShape->ApplyTransform(childTransform);
							glShape->DrawShape(shaderProgram, "u_ModelMatrix");
						}
					}
				}
			}
			else if (GLShape* glShape = static_cast<GLShape*>(collisionObjectRigidBody->getUserPointer()))
			{
				glShape->ApplyTransform(rbTransform);
				glShape->DrawShape(shaderProgram, "u_ModelMatrix");
			}
		}

#ifdef CANNON_VEHICLE
		cannonVehicle.DrawWheels(shaderProgram);
#endif

		//update camera
		camera.HandleInputs(window, stepTime);
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);

	for (int i = collisionShapes.size() - 1; i >= 0; --i)
	{	
		delete collisionShapes[i];
	}
	collisionShapes.clear();

	for (GLBoxShape* boxObstacle : boxObstacles)
	{
		delete boxObstacle;
	}
	boxObstacles.clear();

	delete world;
	delete defaultCollisionConfig;
	delete dispatcher;
	delete broadPhase;
	delete constraintSolver;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}