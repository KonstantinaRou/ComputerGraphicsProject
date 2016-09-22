#include "Door.h"
#include "../Source/SceneGraph/GeometryNode.h"
#include "../Solution/BorderLimit.h"

Door::Door(TransformNode* parent, OGLMesh* doorwaySidemesh, OGLMesh* doormesh, float x, float z, float rotation)
{
	this->x=x;
	this->z=z;
	this->rotation=rotation;
	isOpen=false;
	TransformNode* rightdoorwaytrans = new TransformNode("rightdoorway");
	GeometryNode* rightdoorwaygeom = new GeometryNode("rightdoorway",doorwaySidemesh);
	rightdoorwaytrans->AddChild(rightdoorwaygeom);

	rightdoortrans = new TransformNode("rightdoortrans");
	GeometryNode* rightdoorgeom = new GeometryNode("rightdoorgeom",doormesh);
	rightdoortrans->AddChild(rightdoorgeom);

	doorTransform = new TransformNode("door");
	TransformNode* leftdoorwaytrans = new TransformNode("leftdoorway");
	GeometryNode* leftdoorwaygom = new GeometryNode("leftdoorway",doorwaySidemesh);
	leftdoorwaytrans->AddChild(leftdoorwaygom);

	leftdoortrans = new TransformNode("leftdoortrans");
	GeometryNode* leftdoorgeom = new GeometryNode("leftdoorgeom",doormesh);
	leftdoortrans->AddChild(leftdoorgeom);


	doorTransform->AddChild(leftdoorwaytrans);
	doorTransform->AddChild(rightdoorwaytrans);
	doorTransform->AddChild(rightdoortrans);
	doorTransform->AddChild(leftdoortrans);
	parent->AddChild(doorTransform);
	
	if(rotation==0)
	{
		leftdoorwaytrans->SetRotation(180,0,1,0);
		leftdoorwaytrans->SetTranslation(0.9,0,0);

		leftdoortrans->SetRotation(180,0,1,0);
		leftdoortrans->SetTranslation(0.79,0,0);

		rightdoorwaytrans->SetTranslation(-0.9,0,0);
		rightdoortrans->SetTranslation(-0.79,0,0);
	}
	else
	{
		
		rightdoorwaytrans->SetRotation(90,0,1,0);
		leftdoorwaytrans->SetRotation(270,0,1,0);

		rightdoortrans->SetRotation(90,0,1,0);
		leftdoortrans->SetRotation(270,0,1,0);

		leftdoorwaytrans->SetTranslation(0,0,-0.9);
		rightdoorwaytrans->SetTranslation(0,0,+0.9);
		rightdoortrans->SetTranslation(0,0,+0.79);
		leftdoortrans->SetTranslation(0,0,-0.79);
	}


	
	doorTransform->SetTranslation(x,0,z);
	setDoor(z*2,x*2);
	setDoor((z-1)*2,x*2);
	setDoor(z*2,(x-1)*2);
	setDoor((z-1)*2,(x-1)*2);
	/*openDoor();
	closeDoor();*/

}


void Door::checkDoor(int x, int y)
{
	if(isDoor(y*2, x*2))
		openDoor();
}

void Door::openDoor()
{
	if(!isOpen)
	{
		rightdoortrans->SetRotation(rightdoortrans->GetRotationAngle()-90,0,1,0);
		leftdoortrans->SetRotation(leftdoortrans->GetRotationAngle()+90,0,1,0);
	
		isOpen=true;
	}
}

void Door::closeDoor()
{
	if(isOpen)
	{
		rightdoortrans->SetRotation(rightdoortrans->GetRotationAngle()+90,0,1,0);
		leftdoortrans->SetRotation(leftdoortrans->GetRotationAngle()-90,0,1,0);
		isOpen=false;
	}
}

Door::~Door(void)
{
	
}
