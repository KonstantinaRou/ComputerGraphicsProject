#include "Arrow.h"
#include "../Source/Renderer.h"
#include "../Source/SceneGraph/Root.h"
#include <math.h>
#include "../Solution/BorderLimit.h"
Arrow::Arrow(void)
{
}
int Arrow::i=0;
Arrow::Arrow(TransformNode* totemNode, OGLMesh* arrowMesh,int angle)
{
	this->totemNode=totemNode;
	this->arrowMesh=arrowMesh;
	this->angle=angle;
	this->speed=0.007;
	this->lifespan=2000;
	TransformNode* parent = (TransformNode*)totemNode->GetParent();
	geometry = new GeometryNode("Genarrow"+(i++), arrowMesh);
	arrowNode = new TransformNode("arrowTrans"+(i++));
	arrowNode->AddChild(geometry);
	parent->AddChild(arrowNode);
	totemNode->GetWorld()->Init();
	
	arrowNode->SetRotation(angle,0,1,0);
	glm::vec3 startPos =totemNode->GetTranslation();
	glm::vec3 offset =glm::vec3(0.15,0,0.15);
	offset.x=offset.x*sin(angle*PI/180);
	offset.z=offset.z*cos(angle*PI/180);
	startPos+=offset;
	//PrintToOutputWindow("Arrow:x%f,z%f",vel.x,vel.z);
	arrowNode->SetTranslation(startPos.x,startPos.y,startPos.z);
	
	
	
}

bool Arrow::hasCollided()
{
	glm::vec3 pos=arrowNode->GetTranslation();
	if( isWall(pos.z*2,pos.x*2))
	{
		if(((int)arrowNode->GetRotationAngle())%180==0)
		{
		
			removeArrow(pos.z*2,pos.x*2);
			removeArrow(pos.z*2,(pos.x-0.5)*2);
		}else
		{
			removeArrow(pos.z*2,pos.x*2);
			removeArrow((pos.z-0.5)*2,pos.x*2);
		}
			return true;
	}
	return false;
}

void Arrow::moveArrow(float time)
{
	lifespan-=time;
	glm::vec3 newPos=arrowNode->GetTranslation();
	if(((int)arrowNode->GetRotationAngle())%180==0)
	{
		
		removeArrow(newPos.z*2,newPos.x*2);
		removeArrow(newPos.z*2,(newPos.x-0.5)*2);
	}else
	{
		removeArrow(newPos.z*2,newPos.x*2);
		removeArrow((newPos.z-0.5)*2,newPos.x*2);
	}
	//removeArrow(newPos.z*2,newPos.x*2);
	float delta = time*speed;
	newPos.x += delta*sin(angle*PI/180);
	newPos.z += delta*cos(angle*PI/180);
	arrowNode->SetTranslation(newPos);
	//setArrow(newPos.z*2,newPos.x*2);
	if(((int)arrowNode->GetRotationAngle())%180==0)
	{
		setArrow(newPos.z*2,newPos.x*2);
		setArrow(newPos.z*2,(newPos.x-0.5)*2);
	}else
	{
		setArrow(newPos.z*2,newPos.x*2);
		setArrow((newPos.z-0.5)*2,newPos.x*2);
		
	}
	//energopoihsh arrow
	

}

Arrow::~Arrow(void)
{
	
}
