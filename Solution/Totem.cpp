#include <Math.h>
#include "Totem.h"

#include "../Source/Renderer.h"
#include "../Source/SceneGraph/GeometryNode.h"

Totem::Totem(void)
{
}



Totem::Totem(TransformNode* tranform, OGLMesh* arrowMesh)
{
	this->clockwise=false;
	this->tranform=tranform;
	this->arrowMesh=arrowMesh;
	arrowRegenAngle=90;
	rotateSpeed=0.07;
	
	
}

void Totem::rotateTotem(float time)
{
	float prevAngle = tranform->GetRotationAngle();
	float newAgnle;
	if(!clockwise)
	{
		
		newAgnle = ((int)(prevAngle+time*rotateSpeed))%360;
	}
	else
	{
		
		newAgnle = ((int)(prevAngle-time*rotateSpeed))%-360;
	}
		
	tranform->SetRotation(newAgnle,0,1,0);
	if(abs((int)prevAngle)%arrowRegenAngle>arrowRegenAngle/2&&abs((int)newAgnle)%arrowRegenAngle<arrowRegenAngle/2)
	//if(abs((int)newAgnle)%90==0)
	{
		Arrow* arrow =   new Arrow(tranform,arrowMesh,((((int)newAgnle))/arrowRegenAngle)*arrowRegenAngle);
		arrows.push_back(arrow);

		Arrow* arrowBack =   new Arrow(tranform,arrowMesh,((((((int)newAgnle))/arrowRegenAngle)*arrowRegenAngle)+180)%360);
		arrows.push_back(arrowBack);
		
		//PrintToOutputWindow("Arrow:%d",((((int)newAgnle))/arrowRegenAngle)*arrowRegenAngle);
		
	}


	for (int i = 0; i < arrows.size(); i++)
	{
		Arrow* ar =arrows[0];
		arrows.erase(arrows.begin());
		if(ar->hasCollided()||ar->lifespan<0)
		{
			((TransformNode*) (tranform->GetParent()))->RemoveChild(ar->arrowNode);
			//Arrow* ar = arrows[0];
			
			
			delete ar;

		}
		else
		{
			arrows.push_back(ar);
			ar->moveArrow(time);
		}
	}
	/*for (std::vector<Arrow*>::iterator it = arrows.begin(); it != arrows.end(); ++it)
	{
		
		(*it)->moveArrow(time);
		if((*it)->lifespan<=0)
		{
			arrows.erase(it);
			delete (*it);

		}
	}*/
	
}

Totem::~Totem(void)
{
}
