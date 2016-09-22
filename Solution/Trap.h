#ifndef TRAP_H
#define TRAP_H
#pragma once
#include "../Source/SceneGraph/TransformNode.h"
#include "../Source/OBJ/OGLMesh.h"
#include "../Source/HelpLib.h"
class Trap
{

private:

	TransformNode* tranform;
	

	static int counter;
	static OGLMesh spikeMesh;
	static OGLMesh trapMesh;
	int baseSpikeCounter;
	int spikemsecLoop;
	bool spikesUP;
	bool spikesDown;
	bool moveSpikesUp;
	bool moveSpikesDown;
	glm::vec3 spikeVel;

	Trap(void);

public:
	
	Trap(TransformNode* parent, OGLMesh* trapMesh, OGLMesh* spikeMesh, float cooldown,float x, float z, float rotation);
	
	
	void setCooldown(float cooldown);

	void adjustSpikes(float time);

	//void move(glm::vec3 movement);

	void updateBorders();

	~Trap(void);

};
#endif