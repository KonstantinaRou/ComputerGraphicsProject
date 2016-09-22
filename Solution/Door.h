#ifndef DOOR_H
#define DOOR_H
#pragma once
#include "../Source/SceneGraph/TransformNode.h"
#include "../Source/OBJ/OGLMesh.h"
#include "../Source/HelpLib.h"
class Door
{
public:
	TransformNode* doorTransform;
	TransformNode* rightdoortrans;
	TransformNode* leftdoortrans;
	float x;
	float z;
	float rotation;
	bool isOpen;
	Door(TransformNode* parent, OGLMesh* doorwaySidemesh, OGLMesh* doormesh, float x, float z, float rotation);
	void checkDoor(int x, int y);
	void openDoor();
	void closeDoor();
	~Door(void);
};
#endif