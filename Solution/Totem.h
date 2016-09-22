#ifndef тотел_H
#define тотел_H
#pragma once
#include "../Source/SceneGraph/TransformNode.h"
#include "../Source/OBJ/OGLMesh.h"
#include "../Source/HelpLib.h"
#include "Arrow.h"
class Totem
{
private:
	TransformNode* tranform;
	OGLMesh* arrowMesh;
	bool spinning;
	int arrowRegenAngle;
	float rotateSpeed;
	bool clockwise;
	std::vector<Arrow*> arrows;
	

public:

	Totem(void);
	Totem(TransformNode* tranform, OGLMesh* arrowMesh);

	void rotateTotem(float time);

	~Totem(void);
};
#endif