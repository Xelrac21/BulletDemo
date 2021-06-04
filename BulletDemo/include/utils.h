#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <vector>
#include <ctime>

#include "GLBoxShape.h"
#include "GLPlaneShape.h"
#include "GLCylinderShape.h"
#include "GLSphereShape.h"

#define GROUND_HEIGHT -3
#define CANNON_RB
#define CANNON_VEHICLE

void ResetShaderModelMatrix(GLuint shaderProgramID, const char* uniformModelMatrixName);

void ResetShaderColorVector(GLuint shaderProgramID, const char* uniformColorName);

GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

btRigidBody* CreateRigidBody(btCollisionShape* shape, const btVector3& position, btScalar mass, GLShape* glShape = nullptr);

void GetVertexArrayFromShapeHull(btShapeHull& shapeHull, float outVertexBuffer[], const int numVerts);

void GetIndexArrayFromShapeHull(btShapeHull& shapeHull, unsigned int outIndexBuffer[], const int numIndices);

void CreateAndShootSphere(btDiscreteDynamicsWorld* world, btCollisionShape* sphereShape, btScalar mass, btVector3 startPosition, btVector3 acceleration);

GLBoxShape* CreateRandomSizedBox(btDiscreteDynamicsWorld* world, btAlignedObjectArray<btCollisionShape*>& collisionShapesTracker, const btVector3& startPosition, btScalar maxMass, btScalar minMass, float maxHalfX, float minHalfX, float maxHalfY, float minHalfY, float maxHalfZ, float minHalfZ);

void GetYawPitchAndRoll(const glm::mat4& transform, float& yaw, float& pitch, float& roll);

float RandomFloat(float maxFloat, float minFloat);

void GenerateBoxObstacles(btDiscreteDynamicsWorld* world, btAlignedObjectArray<btCollisionShape*>& collisionShapesTracker, std::vector<GLBoxShape*>& boxObstacles);
