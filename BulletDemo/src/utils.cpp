#include "utils.h"

void ResetShaderModelMatrix(GLuint shaderProgramID, const char* uniformModelMatrixName)
{
	glm::mat4 modelMatrix(1.0f);
	unsigned int modelMatrixID = glGetUniformLocation(shaderProgramID, uniformModelMatrixName);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void ResetShaderColorVector(GLuint shaderProgramID, const char* uniformColorName)
{
	GLuint colorLocation = glGetUniformLocation(shaderProgramID, uniformColorName);
	glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
}

GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

btRigidBody* CreateRigidBody(btCollisionShape* shape, const btVector3& position, btScalar mass, GLShape* glShape)
{
	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(position);
	btMotionState* motionState = new btDefaultMotionState(shapeTransform);

	btVector3 localIntertia(0, 0, 0);
	if (mass > 0.0f)
	{
		shape->calculateLocalInertia(mass, localIntertia);
	}

	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, shape, localIntertia);
	btRigidBody* rigidBody = new btRigidBody(constructionInfo);

	if (rigidBody != nullptr && glShape != nullptr)
		rigidBody->setUserPointer(glShape);

	return rigidBody;
}

void GetVertexArrayFromShapeHull(btShapeHull& shapeHull, float outVertexBuffer[], const int numVerts)
{
	const btVector3* shapeVertices = shapeHull.getVertexPointer();
	for (int i = 0; i < numVerts; ++i)
	{
		const btVector3 currentVector = *(shapeVertices + i);
		outVertexBuffer[i * 3] = currentVector.x();
		outVertexBuffer[i * 3 + 1] = currentVector.y();
		outVertexBuffer[i * 3 + 2] = currentVector.z();
	}
}

void GetIndexArrayFromShapeHull(btShapeHull& shapeHull, unsigned int outIndexBuffer[], const int numIndices)
{
	const unsigned int* shapeIndices = shapeHull.getIndexPointer();
	for (int i = 0; i < numIndices; ++i)
	{
		outIndexBuffer[i] = *(shapeIndices + i);
	}
}

void CreateAndShootSphere(btDiscreteDynamicsWorld* world, btCollisionShape* sphereShape, btScalar mass, btVector3 startPosition, btVector3 acceleration)
{
	btRigidBody* sphereRB = CreateRigidBody(sphereShape, startPosition, mass);
	world->addRigidBody(sphereRB);
	sphereRB->applyCentralForce(mass * acceleration);
}

GLBoxShape* CreateRandomSizedBox(btDiscreteDynamicsWorld* world, btAlignedObjectArray<btCollisionShape*>& collisionShapesTracker, const btVector3& startPosition, btScalar maxMass, btScalar minMass, float maxHalfX, float minHalfX, float maxHalfY, float minHalfY, float maxHalfZ, float minHalfZ)
{
	float halfExtentsX = RandomFloat(maxHalfX, minHalfX);
	float halfExtentsY = RandomFloat(maxHalfY, minHalfY);
	float halfExtentsZ = RandomFloat(maxHalfZ, minHalfZ);

	float randomPercent = RandomFloat(1.0, 0.0);
	btScalar mass = randomPercent > 0.2 ? RandomFloat(maxMass, minMass) : 0.0f;

	glm::vec3 color = mass == 0.0f ? glm::vec3(0.6f, 0.6f, 0.6f) : glm::vec3(0.0, 0.0, 1.0);

	GLBoxShape* box = new GLBoxShape(halfExtentsX, halfExtentsY, halfExtentsZ, color);

	btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(halfExtentsX, halfExtentsY, halfExtentsZ));
	collisionShapesTracker.push_back(boxCollisionShape);
	btRigidBody* boxRB = CreateRigidBody(boxCollisionShape, startPosition, mass, box);
	world->addRigidBody(boxRB);

	return box;
}

void GetYawPitchAndRoll(const glm::mat4& transform, float& yaw, float& pitch, float& roll)
{
	pitch = -atan2(-transform[2][1], transform[2][2]);
	yaw = -atan2(transform[2][0], sqrt(pow(transform[0][0], 2) + pow(transform[1][0], 2)));
	roll = atan2(cos(pitch) * transform[0][1] + sin(pitch) * transform[0][2], cos(pitch) * transform[1][1] + sin(pitch) * transform[1][2]);
}

float RandomFloat(float maxFloat, float minFloat)
{
	srand(rand() ^ time(0));
	float randomFloat = minFloat + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxFloat - minFloat)));
	return randomFloat;
}

void GenerateBoxObstacles(btDiscreteDynamicsWorld* world, btAlignedObjectArray<btCollisionShape*>& collisionShapesTracker, std::vector<GLBoxShape*>& boxObstacles)
{
	unsigned int numBoxesPerRing = 15;
	unsigned int numRings = 5;

	float yCoord = GROUND_HEIGHT + 0.01 + 2.0f;
	float deviationRadius = 0.3f;
	float currentTheta = 0.0f;
	float radiusFromCenter = 12.0f;
	for (unsigned int i = 0; i < numRings; ++i, radiusFromCenter += RandomFloat(5.0f, 3.0f))
	{
		for (unsigned int j = 0; j < numBoxesPerRing; ++j, currentTheta += RandomFloat(1.0f, 0.5f))
		{
			float xCoord = radiusFromCenter * cos(currentTheta) + RandomFloat(deviationRadius, -deviationRadius);
			float zCoord = -radiusFromCenter * sin(currentTheta) + RandomFloat(deviationRadius, -deviationRadius);

			btVector3 startPosition(xCoord, yCoord, zCoord);
			GLBoxShape* boxObstacle = CreateRandomSizedBox(world, collisionShapesTracker, startPosition, 8, 2, 0.8, 0.4, 2, 0.8, 1.2, 0.6);
			boxObstacles.push_back(boxObstacle);
		}
	}
}