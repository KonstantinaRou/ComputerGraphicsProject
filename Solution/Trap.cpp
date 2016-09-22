#include "Trap.h"
#include "../Source/SceneGraph/GeometryNode.h"
#include "../Source/Renderer.h"
#include "../Solution/BorderLimit.h"
Trap::Trap(void)
{
	
	baseSpikeCounter=0;
	spikemsecLoop=1800;
	spikesUP=false;
	spikesDown=false;
	moveSpikesUp=false;
	moveSpikesDown=false;
	spikeVel= glm::vec3(0.0f,0.012f,0.0f);
}

Trap::Trap(TransformNode* parent, OGLMesh* trapMesh, OGLMesh* spikeMesh, float cooldown,float x, float z, float rotation)
{
	TransformNode* trap = new TransformNode("trap");
	GeometryNode* trapGeom = new GeometryNode("trapgeom",trapMesh);
	trap->AddChild(trapGeom);
	parent->AddChild(trap);

	TransformNode* spikes = new TransformNode("spikes");
	GeometryNode* spikesGeom= new GeometryNode("spikesgeom",spikeMesh);
	spikes->AddChild(spikesGeom);
	parent->AddChild(spikes);

	trap->SetRotation(rotation,0,1,0);
	spikes->SetRotation(rotation,0,1,0);
	
	trap->SetTranslation(x,0,z);
	spikes->SetTranslation(x,0,z);

	this->tranform=spikes;
	this->spikemsecLoop=cooldown;
	baseSpikeCounter=0;
	spikesUP=false;
	spikesDown=false;
	moveSpikesUp=false;
	moveSpikesDown=false;
	spikeVel= glm::vec3(0.0f,0.012f,0.0f);
}

void Trap::setCooldown(float cooldown)
{
	this->spikemsecLoop=cooldown;
}

void Trap::adjustSpikes(float time)
{
	if(moveSpikesDown)
	{
		tranform->SetTranslation(tranform->GetTranslation()+(-time*spikeVel));
		spikesDown=false;
		if(tranform->GetTranslation().y<0)
		{
		glm::vec3 pos = tranform->GetTranslation();
		pos.y=0;
		tranform->SetTranslation(pos);
		moveSpikesDown=false;
		spikesUP=false;
		spikesDown=true;
		//updateBorders();
		}

	}
	else if(moveSpikesUp)
	{
		tranform->SetTranslation(tranform->GetTranslation()+(time*spikeVel));
		spikesDown=false;
		if(tranform->GetTranslation().y>1.5)
		{
			glm::vec3 pos = tranform->GetTranslation();
			pos.y=1.5f;
			tranform->SetTranslation(pos);
			moveSpikesUp=false;
			spikesUP=true;
			//updateBorders();
			//energopoihsh spike
			
		}
	}

	

	baseSpikeCounter+=time;
	if(baseSpikeCounter>=spikemsecLoop)
	{
		
		baseSpikeCounter=0;
		if(spikesUP)
		{
			moveSpikesDown=true;
		}else
		{
			moveSpikesUp=true;
			
		}
		updateBorders();
	}

	
}

void Trap::updateBorders()
{
	if(spikesDown)
	{
		glm::vec3 tpos = tranform->GetTranslation();
		if(tranform->GetRotationAngle()==0)
		{
			
			setTrap(tpos.z*2,tpos.x*2);
			setTrap(tpos.z*2,(tpos.x-0.5)*2);
			//border;
			//PrintToOutputWindow("TrapUp:x %f,z %f",tpos.x,tpos.z);
		}else if(tranform->GetRotationAngle()==90)
		{
			setTrap(tpos.z*2,tpos.x*2);
			setTrap((tpos.z-0.5)*2,tpos.x*2);
		}
	}else //if(spikesDown)
	{
		glm::vec3 tpos = tranform->GetTranslation();
		if(tranform->GetRotationAngle()==0)
		{
			
			removeTrap(tpos.z*2,tpos.x*2);
			removeTrap(tpos.z*2,(tpos.x-0.5)*2);
			//border;
			//PrintToOutputWindow("TrapDown:x %f,z %f",tpos.x,tpos.z);
		}
		else if(tranform->GetRotationAngle()==90)
		{
			removeTrap(tpos.z*2,tpos.x*2);
			removeTrap((tpos.z-0.5)*2,tpos.x*2);
		}
	}
}


Trap::~Trap(void)
{
}
