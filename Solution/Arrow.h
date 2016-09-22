#ifndef ARROW_H
#define ARROW_H
#pragma once
#include "../Source/SceneGraph/TransformNode.h"
#include "../Source/SceneGraph/GeometryNode.h"
#include "../Source/OBJ/OGLMesh.h"
#include "../Source/HelpLib.h"
class Arrow
{
public:
	static int i;
	TransformNode* totemNode;
	TransformNode* arrowNode;
	GeometryNode* geometry;
	OGLMesh* arrowMesh;
	int angle;
	float speed;
	float lifespan;
	Arrow(void);
	Arrow(TransformNode* totemNode, OGLMesh* arrowMesh,int angle);
	void moveArrow(float time);
	bool hasCollided();
	~Arrow(void);
};

#endif