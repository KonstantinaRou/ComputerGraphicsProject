//----------------------------------------------------//
//                                                    //
// File: Renderer.cpp                                 //
// Initialization, rendering and input handling		  //
// happens here										  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//

#include "HelpLib.h"		// - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.

#include "OBJ/OBJLoader.h"	// - Header file for the OBJ Loader
#include "OBJ/OGLMesh.h"	// - Header file for the OGL mesh
#include "ShaderGLSL.h"		// - Header file for GLSL objects
#include "Light.h"			// - Header file for Lights
#include "Shaders.h"		// - Header file for all the shaders
#include "Renderer.h"		// - Header file for our OpenGL functions
#include "../Solution/Trap.h"		// - Header file for our OpenGL functions
#include "../Solution/Totem.h"
#include "../Solution/Door.h"
#include "SceneGraph/Root.h"
#include "SceneGraph/TransformNode.h"
#include "SceneGraph/GroupNode.h"
#include "SceneGraph/GeometryNode.h"
#include "../Solution/BorderLimit.h"
#include <vector> 
#include <string>
#include <iostream>


#define MESSAGE	"Underworld live 17.03.04"

//player speed
float z_speed=1.0f;
glm::vec3 sdir = glm::vec3(0.0f, 0.0f, 0.0f);
float s=2.0f;
float theta=4.70f;			 
float fi=0.40f;
float camera_rotate_y_factor=0.05;
float camera_rotate_x_factor=0.05;
int numOfDragonsCollected=0;
int lifes=5;

// window settings
float current_width;
float current_height;

// camera parameters
glm::vec3 eye;
glm::vec3 target;
glm::vec3 up;

// transformation matrices
glm::mat4x4 world_to_camera_matrix;
glm::mat4x4 perspective_projection_matrix;

// Mouse button and coords
int mouse_button = -1;
int prev_x = 0;
int prev_y = 0;

// World Rotation
float world_rotate_x = 0;
glm::vec3 world_translate;

// Rendering mode
int rendering_mode = GL_FILL;

bool hasDied = false;

// OBJ models
OGLMesh* groundwhiteMesh;
OGLMesh* spherewhiteMesh;



OGLMesh*  small_room_mesh;
OGLMesh*  corridor_medium_mesh;
OGLMesh*  corridor_T_mesh;
OGLMesh*  corridor_turn_wide_mesh;
OGLMesh*  hall_mesh;
OGLMesh*  wall0_5_mesh;
OGLMesh*  wall1_mesh;
OGLMesh*  wall2_mesh;
OGLMesh*  floortiles2;
OGLMesh*  floortiles1;
OGLMesh*  spikes_mesh;
OGLMesh*  spikeTrap_mesh;
OGLMesh*  totem_mesh;
OGLMesh*  arrow_mesh;
OGLMesh* golden_dragon;
OGLMesh* pedestal;
OGLMesh* doorWayMesh;
OGLMesh* doorMesh;

TransformNode* walls_transform[78];
GeometryNode* walls_geom[78];

TransformNode* golden_dragon_transform[10];
GeometryNode* golden_dragon_geom[10];

TransformNode* pedestal_transform[10];
GeometryNode* pedestal_geom[10];


Door* door;
std::vector<Trap*> traps;
std::vector<Totem*> totems;
int spikemsecLoop=1800;

int const q = 8;
int const w=4;
TransformNode* floor_transform1[14][7];
GeometryNode* floor_geom1 [14][7];
TransformNode* floor_transform2[6][3];
GeometryNode* floor_geom2[6][3];
TransformNode* floor_transform3[5][2];
GeometryNode* floor_geom3[5][2];
TransformNode* floor_transform4[q][w];
GeometryNode* floor_geom4[q][w];

TransformNode* lostpakai1_transform;
GeometryNode* lostpakai1_geom;
TransformNode* lostpakai2_transform;
GeometryNode* lostpakai2_geom;


TransformNode* totem3_transform;
TransformNode* totem4_transform;
TransformNode* totem5_transform;
GeometryNode* totem3_geom;
GeometryNode* totem4_geom;
GeometryNode* totem5_geom;


// Scene graph nodes
Root* root;
TransformNode* world_transform;
TransformNode* ground_transform;
GeometryNode* ground_geom;

OGLMesh* playerMesh;
TransformNode* player_transform;
GeometryNode* player_geom;

// Shaders
// basic geometry shader
BasicGeometryShader* basic_geometry_shader;
// spotlight shader
SpotLightShader* spotlight_shader;
// shadow map shader
ShadowMapShader* shadowmap_shader;

// Lights
SpotLight* spotlight;

// light parameters (for animating the light)
float light_rotationY;

// frame buffer object
GLuint shadow_fbo;
GLuint shadow_tex;
float shadow_dimensions;

// forward declarations
bool CreateShadowFBO();
void DrawSceneToShadowFBO();
bool CreateShaders();
bool LoadObjModels();
void DrawOGLMesh(OGLMesh* mesh, glm::mat4x4& object_to_world_transform);
void SceneGraphExampleInit();
void SceneGraphExample2Init();
void Sprint( float x,float y,float z, char *message);
//void SceneGraphExample3Init();
void DrawSpotLightSource();
void SceneGraphDraw();

// This init function is called before FREEGLUT goes into its main loop.
bool InitializeRenderer(void)   
{ 
	//borders = new BorderLimit();
	// Initialize GLEW (openGL extensions)
	// GLEW is a library which determines which OpenGL extensions are supported on each platform
	// For example, in windows we need this since Microsoft does not support anything beyond OpenGL 1.1 (look at the gl/gl.h header file)
	// The code below uses GLEW to include any extensions supported in this platform
	glewExperimental = GL_TRUE;
	GLuint err = glewInit();
	// skip GLEW bug in glewInit
	unsigned int _enum = glGetError();	
	if (GLEW_OK != err)
	{
		PrintToOutputWindow("GLEW error %s", glewGetErrorString(err));
		return false;
	}
	else
		PrintToOutputWindow("Using GLEW %s", glewGetString(GLEW_VERSION));

	// check specifically for OpenGL 3.3 support
	bool ogl33_supported = (bool)(GLEW_GET_VAR(__GLEW_VERSION_3_3)!=0);
	if (!ogl33_supported)
	{
		PrintToOutputWindow("OpenGL 3.3 not supported. Exiting.");
		return false;
	}
	else
		PrintToOutputWindow("OpenGL 3.3 supported.");

	glutSetCursor(GLUT_CURSOR_NONE); 
	

	// define the lights here
	spotlight = new SpotLight();
	spotlight->m_name = "spotlight";

	// for the camera
	eye = glm::vec3(0.0f, 0.0f, 1.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);//glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	world_translate = glm::vec3(0);

	
	// Create the shaders here
	// Shader declaration is in the Shaders.h file
	// The CreateShaders function initializes the pointers, compiles the shader files
	// and checks for uniforms
	if (!CreateShaders())
	{
		return false;
	}

	// Load the OBJ models here
	if (!LoadObjModels())
	{
		PrintToOutputWindow("Error in loading obj models. Exiting");
		return false;
	}

	// Enable the depth buffer
	// Default is disabled
	glEnable(GL_DEPTH_TEST);

	// glDepthFunc tells OpenGL to accept fragments whose depth is less or equal than the current depth value
	// Note that 0.0 denotes the near field and 1.0 denotes the far field
	// Initial value is GL_LESS
	glDepthFunc(GL_LEQUAL);

	// Set the value to clear the depth buffer to value 1.0 (all far) (using glClear later on)
	glClearDepth(1.0f);

	// Enable Face Culling
	//glEnable(GL_CULL_FACE);
	// Tell OpenGL to cull back faces
	//glCullFace(GL_BACK);
	// Tell OpenGL that the front faces are the ones in counter-clockwise order
	glFrontFace(GL_CCW);

	// Initialize the FBO
	CreateShadowFBO();

	// Enable Scene Graph
	
	SceneGraphExampleInit();
	
	
	
	
	// check if we have generated any OpenGL errors
	glError();
	PrintToOutputWindow("############################");
	PrintToOutputWindow("Lifes:%d",lifes);
	PrintToOutputWindow("Dragons to collect:10");
	PrintToOutputWindow("############################");
	return true;
}

bool CreateShadowFBO()
{
	// set the shadow dimensions
	shadow_dimensions = 2*1024.0f;

	// Create a framebuffer object (FBO)
	// When bound, rendering will happen on the FBO's attached textures
	glGenFramebuffers(1, &shadow_fbo);
	// set the fbo active
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

	// since we need to render only depth maps, we tell OpenGL that we do not need
	// to write to any color buffer
	glDrawBuffer(GL_NONE);
	// similarly, we do not need to read from any color buffer
	glReadBuffer(GL_NONE);

	// generate a texture to write to
	glGenTextures(1, &shadow_tex);
	// set the texture active
	glBindTexture(GL_TEXTURE_2D, shadow_tex);

	PrintToOutputWindow("%s, Generated Depth Texture: %i.", spotlight->m_name.c_str(), shadow_tex);

	// allocate space for a depth texture
	// each value will be stored in a single channel 32-bit float value
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadow_dimensions, shadow_dimensions, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	// we set linear interpolation for the filtering and clamp to edge for wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// FBO's do not have any default buffers to write to
	// instead, we need to set a number of images attached to them which will specify where to write
	// information such as color and depth
	// The common approach is to attach one or more color textures and one depth texture
	// For shadow mapping, only a depth texture needs to be attached (Which was created above)
	// the depth texture is attached to the depth attachment point of the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_tex, 0);

	// check for any FBO errors (similar to glError)
	// this checks for example, if the attachments that were set using glFramebufferTexture are valid
	if (checkFrameBufferError("(Depth) Incomplete SpotLight fbo")) return false;

	// set the texture id to the spotlight object
	// this is later used in the geometry node to pass it as a texture to the spotlight shader
	spotlight->m_shadow_map_texture_id = shadow_tex;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void SceneGraphExampleInit()
{
	// Scene graph example
	// background settings
	// Set the RGBA values to clear the color buffer (using glClear later on)
	glClearColor(0.03f, 0.2f, 0.22f, 1.0f); // for spot light

	// eye settings 
	eye = glm::vec3(0.0f, 1.0f, 0.0f);

	// light settings
	spotlight->m_color = glm::vec3(1.0f, 0.57f, 0.16f);
	spotlight->m_initial_target = glm::vec3(0.0f, 0.0f, 0.0f);
	spotlight->m_initial_position = glm::vec3(5.0f, 20.0f, 0.0f);
	spotlight->m_near_range = 8.0f;
	spotlight->m_far_range = 100.0f;
	spotlight->m_aperture = 60.0f;

	// scene settings
	root = new Root();
	world_transform = new TransformNode("world_transform");
	ground_transform = new TransformNode("ground_transform");



	for (int i = 0; i< 10; i++ ) {
		golden_dragon_transform[i]=new TransformNode("gd_"+i);
		golden_dragon_geom[i]= new GeometryNode("gd_"+i,golden_dragon);
	}

	for (int i = 0; i<10; i++ ) {
		pedestal_transform[i]=new TransformNode("pd_"+i);
		pedestal_geom[i]= new GeometryNode("pd_"+i,pedestal);
	}
	

	lostpakai1_transform= new TransformNode("lostpakai1_transform");
	lostpakai1_geom=new GeometryNode("lostpakai1_geom",floortiles1);
	lostpakai2_transform= new TransformNode("lostpakai2_transform");
	lostpakai2_geom=new GeometryNode("lostpakai2_geom",floortiles2);

	lostpakai1_transform->AddChild(lostpakai1_geom);
	lostpakai2_transform->AddChild(lostpakai2_geom);

	world_transform->AddChild(lostpakai1_transform);
	world_transform->AddChild(lostpakai2_transform);

	for (int i = 0; i <14; i++)
	{
		for(int j=0;j<7 ;j++)
			{
				floor_transform1[i][j]= new TransformNode("floor1_"+i+j);
				if(i%2==0)
				{
					floor_geom1[i][j]=new GeometryNode("floor1_"+i+j,floortiles2);
				}
				else 
				{
				floor_geom1[i][j]=new GeometryNode("floor1_"+i+j,floortiles1);
				}
			}
	}

	for (int i = 0; i <6; i++)
	{
		for(int j=0;j<3 ;j++)
			{
				floor_transform2[i][j]= new TransformNode("floor2_"+i+j);
				if(i%2==0)
				{
					floor_geom2[i][j]=new GeometryNode("floor2_"+i+j,floortiles2);
				}
				else 
				{
				floor_geom2[i][j]=new GeometryNode("floor2_"+i+j,floortiles1);
				}
			}
	}

	for (int i = 0; i <5; i++)
	{
		for(int j=0;j<2 ;j++)
			{
				floor_transform3[i][j]= new TransformNode("floor3_"+i+j);
				if(i%2==0)
				{
					floor_geom3[i][j]=new GeometryNode("floor3_"+i+j,floortiles2);
				}
				else 
				{
				floor_geom3[i][j]=new GeometryNode("floor3_"+i+j,floortiles1);
				}
			}
	}

	for (int i = 0; i <q; i++)
	{
		for(int j=0;j<w ;j++)
			{
				floor_transform4[i][j]= new TransformNode("floor3_"+i+j);
				if(i%2==0)
				{
					floor_geom4[i][j]=new GeometryNode("floor3_"+i+j,floortiles2);
				}
				else 
				{
				floor_geom4[i][j]=new GeometryNode("floor3_"+i+j,floortiles1);
				}
			}
	}




	for (int i = 0; i < 78; i++)
	{
		walls_transform[i]= new TransformNode("wall_"+i);
	}

	
	walls_geom[0]= new GeometryNode("w_1_geom", small_room_mesh);
	walls_geom[1]= new GeometryNode("w_2_geom", corridor_medium_mesh);
	walls_geom[2]= new GeometryNode("w_3_geom", corridor_T_mesh);
	walls_geom[3]= new GeometryNode("w_4_geom", corridor_turn_wide_mesh);
	
	walls_geom[4]= new GeometryNode("w_5_geom", wall2_mesh);
	walls_geom[5]= new GeometryNode("w_6_geom", wall2_mesh);
	walls_geom[6]= new GeometryNode("w_7_geom", wall1_mesh);

	walls_geom[7]= new GeometryNode("w_8_geom", corridor_medium_mesh);
	walls_geom[8]= new GeometryNode("w_9_geom", corridor_medium_mesh);

	//---hall
	walls_geom[9]= new GeometryNode("w_10_geom", hall_mesh);

	//-- katw

	walls_geom[10]= new GeometryNode("w_11_geom", wall2_mesh);
	walls_geom[11]= new GeometryNode("w_12_geom", wall1_mesh);
	walls_geom[12]= new GeometryNode("w_13_geom", wall2_mesh);

	walls_geom[13]= new GeometryNode("w_14_geom", corridor_medium_mesh);

	walls_geom[14]= new GeometryNode("w_15_geom", wall0_5_mesh);
	walls_geom[15]= new GeometryNode("w_16_geom", wall0_5_mesh);
	walls_geom[16]= new GeometryNode("w_17_geom", wall0_5_mesh);
	walls_geom[17]= new GeometryNode("w_18_geom", wall0_5_mesh);

	walls_geom[18]= new GeometryNode("w_19_geom", corridor_medium_mesh);
	walls_geom[19]= new GeometryNode("w_20_geom", wall2_mesh);

	walls_geom[20]= new GeometryNode("w_21_geom", wall2_mesh);
	walls_geom[21]= new GeometryNode("w_22_geom", wall1_mesh);
	walls_geom[22]= new GeometryNode("w_23_geom", wall2_mesh);

	//panw aristera
	walls_geom[23]= new GeometryNode("w_24_geom", wall0_5_mesh);
	for (int i = 24; i <= 28; i++)
	{
		walls_geom[i]= new GeometryNode("w_24_geom"+i, wall2_mesh);//ftiaxto
	}
	//panw deksia
	walls_geom[29]= new GeometryNode("w_30_geom", wall1_mesh);
	for (int i = 30; i <= 35; i++)
	{
		walls_geom[i]= new GeometryNode("w_30_geom"+i, wall2_mesh);//ftiaxto
	}
	walls_geom[36]= new GeometryNode("w_37_geom", wall1_mesh);
	walls_geom[37]= new GeometryNode("w_38_geom", wall2_mesh);
	walls_geom[38]= new GeometryNode("w_39_geom", wall2_mesh);
	walls_geom[39]= new GeometryNode("w_40_geom", wall0_5_mesh);
	walls_geom[40]= new GeometryNode("w_41_geom", wall2_mesh);
	walls_geom[41]= new GeometryNode("w_42_geom", wall1_mesh);

	//------------

	//--------------------------------------------------------------
	walls_geom[42] = new GeometryNode("w_43_geom",corridor_medium_mesh);
	walls_geom[43] = new GeometryNode("w_44_geom",corridor_medium_mesh);
	walls_geom[44] = new GeometryNode("w_45_geom",wall2_mesh);
	walls_geom[45] = new GeometryNode("w_46_geom",wall1_mesh);
	//--katw
	for (int i = 46; i <= 52; i++)
	{
		walls_geom[i]= new GeometryNode("w_47_geom"+i, wall2_mesh);//ftiaxto
	}

	walls_geom[53] = new GeometryNode("w_54_geom",wall1_mesh);
	walls_geom[54] = new GeometryNode("w_55_geom",wall2_mesh);
	walls_geom[55] = new GeometryNode("w_56_geom",wall2_mesh);
	walls_geom[56] = new GeometryNode("w_57_geom",wall1_mesh);
	walls_geom[57] = new GeometryNode("w_58_geom",wall2_mesh);
	walls_geom[58] = new GeometryNode("w_59_geom",wall1_mesh);
	//--panw

	for (int i = 59; i <= 65; i++)
	{
		walls_geom[i]= new GeometryNode("w_60_geom"+i, wall2_mesh);//ftiaxto
	}

	walls_geom[66] = new GeometryNode("w_67_geom",wall1_mesh);
	walls_geom[67] = new GeometryNode("w_68_geom",wall2_mesh);
	walls_geom[68] = new GeometryNode("w_69_geom",wall0_5_mesh);
	walls_geom[69] = new GeometryNode("w_70_geom",wall2_mesh);
	walls_geom[70] = new GeometryNode("w_71_geom",wall0_5_mesh);
	
	walls_geom[71] = new GeometryNode("w_72_geom",wall2_mesh);
	walls_geom[72] = new GeometryNode("w_73_geom",wall2_mesh);
	walls_geom[73] = new GeometryNode("w_74_geom",wall2_mesh);
	walls_geom[74] = new GeometryNode("w_75_geom",wall2_mesh);
	walls_geom[75] = new GeometryNode("w_76_geom",wall2_mesh);
	walls_geom[76] = new GeometryNode("w_77_geom",wall2_mesh);
	walls_geom[77] = new GeometryNode("w_78_geom",corridor_medium_mesh);

	for (int i = 0; i <10; i++)
	{
		world_transform->AddChild(golden_dragon_transform[i]);
	}

	
	for(int i=0;i<10 ;i++)
	{
		golden_dragon_transform[i]->AddChild(golden_dragon_geom[i]);
	}

	for (int i = 0; i <10; i++)
	{
		world_transform->AddChild(pedestal_transform[i]);
	}

	
	for(int i=0;i<10 ;i++)
	{
		pedestal_transform[i]->AddChild(pedestal_geom[i]);
	}

	door = new Door(world_transform,doorWayMesh,doorMesh,19,9,90);
	//Door* door2 = new Door(world_transform,doorWayMesh,doorMesh,15,9,90);
	//------------
	//Trap::Trap(TransformNode* parent, OGLMesh* trapMesh, OGLMesh* spikeMesh, float cooldown,float x, float z, float rotation)
	Trap* trap1 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,2500,1.5f,12.0f,0.0f);
	traps.push_back(trap1);
	Trap* trap2 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,2500,2.5f,12.0f,0.0f);
	traps.push_back(trap2);
	Trap* trap3 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,10.5f,11.5f,0.0f);
	traps.push_back(trap3);
	Trap* trap4 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,15.5f,11.5f,0.0f);
	traps.push_back(trap4);
	Trap* trap5 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,24.0f,11.0f,90.0f);
	traps.push_back(trap5);
	Trap* trap6 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,24.0f,12.0f,90.0f);
	traps.push_back(trap6);
	Trap* trap7 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,24.0f,7.0f,90.0f);
	traps.push_back(trap7);
	Trap* trap8 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,24.0f,6.0f,90.0f);
	traps.push_back(trap8);
	Trap* trap9 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,35.5f,10.5f,0.0f);
	traps.push_back(trap9);
	Trap* trap10 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,34.5f,10.5f,0.0f);
	traps.push_back(trap10);
	Trap* trap11 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,35.5f,7.0f,0.0f);
	traps.push_back(trap11);
	Trap* trap12 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,34.5f,7.0f,0.0f);
	traps.push_back(trap12);
	Trap* trap13 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,14.0f,4.5f,90.0f);
	traps.push_back(trap13);
	Trap* trap14 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,14.0f,3.5f,90.0f);
	traps.push_back(trap14);
	Trap* trap15 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,13.5f,3.0f,0.0f);
	traps.push_back(trap15);
	Trap* trap16 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,8.50f,6.0f,0.0f);
	traps.push_back(trap16);
	Trap* trap17 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,7.5f,6.0f,0.0f);
	traps.push_back(trap17);
	Trap* trap18 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,7.0f,7.0f,90.0f);
	traps.push_back(trap18);
	Trap* trap19 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,5.0f,7.0f,90.0f);
	traps.push_back(trap19);
	Trap* trap20 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,5.0f,6.0f,0.0f);
	traps.push_back(trap20);
	Trap* trap21 =  new Trap(world_transform,spikeTrap_mesh, spikes_mesh,1800,4.0f,6.0f,0.0f);
	traps.push_back(trap21);

	totem3_transform = new TransformNode("totem3");
	totem4_transform = new TransformNode("totem4");
	totem5_transform = new TransformNode("totem5");
	
		 


	totem3_geom = new GeometryNode("totem3_geom",totem_mesh);
	totem4_geom = new GeometryNode("totem4_geom",totem_mesh);
	totem5_geom = new GeometryNode("totem5_geom",totem_mesh);


	totem3_transform->AddChild(totem3_geom);
	totem4_transform->AddChild(totem4_geom);
	totem5_transform->AddChild(totem5_geom);
	

	player_transform = new TransformNode("player_transform");

	ground_geom = new GeometryNode("ground_geom", groundwhiteMesh);
	

	player_geom = new GeometryNode("player_geom",playerMesh);

	// construct the render tree
	root->AddChild(world_transform);
	world_transform->AddChild(ground_transform);

	for (int i = 0; i < 78; i++)
	{
		world_transform->AddChild(walls_transform[i]);
	}

	for (int i = 0; i <14; i++)
	{
		for(int j=0;j<7 ;j++)
			{
				world_transform->AddChild(floor_transform1[i][j]);
			}
	}

	for (int i = 0; i <6; i++)
	{
		for(int j=0;j<3 ;j++)
			{
				world_transform->AddChild(floor_transform2[i][j]);
			}
	}

	for (int i = 0; i <5; i++)
	{
		for(int j=0;j<2 ;j++)
			{
				world_transform->AddChild(floor_transform3[i][j]);
			}
	}

	for (int i = 0; i <q; i++)
	{
		for(int j=0;j<w ;j++)
			{
				world_transform->AddChild(floor_transform4[i][j]);
			}
	}



	world_transform->AddChild(totem3_transform);
	world_transform->AddChild(totem4_transform);
	world_transform->AddChild(totem5_transform);
	
	world_transform->AddChild(player_transform);

	ground_transform->AddChild(ground_geom);

	for (int i = 0; i < 78; i++)
	{
		walls_transform[i]->AddChild(walls_geom[i]);
	}

	for (int i = 0; i <14; i++)
	{
		for(int j=0;j<7 ;j++)
			{
				floor_transform1[i][j]->AddChild(floor_geom1[i][j]);
			}
	}

	for (int i = 0; i <6; i++)
	{
		for(int j=0;j<3 ;j++)
			{
				floor_transform2[i][j]->AddChild(floor_geom2[i][j]);
			}
	}

	for (int i = 0; i <5; i++)
	{
		for(int j=0;j<2 ;j++)
			{
				floor_transform3[i][j]->AddChild(floor_geom3[i][j]);
			}
	}

	for (int i = 0; i <q; i++)
	{
		for(int j=0;j<w ;j++)
			{
				floor_transform4[i][j]->AddChild(floor_geom4[i][j]);
			}
	}


	player_transform->AddChild(player_geom);

	// initialize root (sets the parent->child relationships in the tree)
	root->Init();

	// since we are using more than one shaders, we are passing them to the root node
	// so they can be accessible from the geometry node during the Draw call
	// this is because each shader requires different uniform variables
	root->SetSpotlightShader(spotlight_shader);
	root->SetShadowMapShader(shadowmap_shader);

	// it's better to apply any static transformations here since 
	// they only need to be set once

	
	// Ground
	player_transform->SetTranslation(2.0f, 0.0f, 2.0f);
	player_transform->SetScale(1.0f, 1.0f, 1.0f);

	ground_transform->SetTranslation(0.0f, -10.0f, 0.0f);


	walls_transform[2]->SetRotation(90,0,1,0);
	walls_transform[3]->SetRotation(90,0,1,0);
	walls_transform[4]->SetRotation(90,0,1,0);
	walls_transform[5]->SetRotation(90,0,1,0);
	walls_transform[7]->SetRotation(90,0,1,0);
	walls_transform[8]->SetRotation(90,0,1,0);
	walls_transform[9]->SetRotation(90,0,1,0);

	//
	
	walls_transform[10]->SetRotation(90,0,1,0);
	walls_transform[11]->SetRotation(90,0,1,0);
	

	walls_transform[21]->SetRotation(90,0,1,0);
	walls_transform[22]->SetRotation(90,0,1,0);
	//walls_transform[23]->SetRotation(90,0,1,0);
	walls_transform[24]->SetRotation(90,0,1,0);

	walls_transform[29]->SetRotation(90,0,1,0);
	walls_transform[31]->SetRotation(90,0,1,0);
	walls_transform[32]->SetRotation(90,0,1,0);
	walls_transform[37]->SetRotation(90,0,1,0);
	walls_transform[38]->SetRotation(90,0,1,0);
	walls_transform[41]->SetRotation(90,0,1,0);
	


	//
	walls_transform[42]->SetRotation(90,0,1,0);///allagh
	walls_transform[43]->SetRotation(90,0,1,0);
	walls_transform[44]->SetRotation(90,0,1,0);
	walls_transform[45]->SetRotation(90,0,1,0);
	walls_transform[54]->SetRotation(90,0,1,0);
	walls_transform[55]->SetRotation(90,0,1,0);
	walls_transform[56]->SetRotation(90,0,1,0);
	walls_transform[57]->SetRotation(90,0,1,0);

	walls_transform[67]->SetRotation(90,0,1,0);
	walls_transform[66]->SetRotation(90,0,1,0);
	walls_transform[68]->SetRotation(90,0,1,0);
	walls_transform[69]->SetRotation(90,0,1,0);
	walls_transform[70]->SetRotation(90,0,1,0);
	walls_transform[72]->SetRotation(90,0,1,0);
	
	walls_transform[74]->SetRotation(90,0,1,0);
	walls_transform[75]->SetRotation(90,0,1,0);
	walls_transform[71]->SetRotation(90,0,1,0);
	walls_transform[77]->SetRotation(90,0,1,0);

	//
	walls_transform[0]->SetTranslation(2.0f, 0.0f, 2.0f);
	walls_transform[1]->SetTranslation(2.0f, 0.0f, 5.0f);
	walls_transform[2]->SetTranslation(2.0f, 0.0f, 9.0f);
	walls_transform[3]->SetTranslation(2.5f, 0.0f, 14.0f);
	walls_transform[4]->SetTranslation(4.5f, 0.0f, 15.0f);
	walls_transform[5]->SetTranslation(4.5f, 0.0f, 13.0f);
	walls_transform[6]->SetTranslation(3.5f, 0.0f, 12.0f);
	walls_transform[7]->SetTranslation(6.0f, 0.0f, 9.0f);
	walls_transform[8]->SetTranslation(8.0f, 0.0f, 9.0f);
	walls_transform[9]->SetTranslation(14.0f, 0.0f, 9.0f);

	//-------------------

	walls_transform[10]->SetTranslation(9.5f, 0.0f, 13.0f);
	walls_transform[11]->SetTranslation(9.5f, 0.0f, 14.5f);
	walls_transform[12]->SetTranslation(11.0f, 0.0f, 14.5f);
	walls_transform[13]->SetTranslation(13.0f, 0.0f, 16.0f);

	walls_transform[14]->SetTranslation(12.0f, 0.0f, 15.75f);
	walls_transform[15]->SetTranslation(13.5f, 0.0f, 15.75f);
	walls_transform[16]->SetTranslation(12.0f, 0.0f, 16.75f);
	walls_transform[17]->SetTranslation(13.5f, 0.0f, 16.75f);

	walls_transform[18]->SetTranslation(13.0f, 0.0f, 18.0f);
	walls_transform[19]->SetTranslation(13.0f, 0.0f, 18.5f);

	walls_transform[20]->SetTranslation(15.0f, 0.0f, 14.5f);
	walls_transform[21]->SetTranslation(16.0f, 0.0f, 14.5f);
	walls_transform[22]->SetTranslation(16.0f, 0.0f, 13.0f);
	
	walls_transform[23]->SetTranslation(11.5f, 0.0f, 5.5f);
	walls_transform[24]->SetTranslation(11.5f, 0.0f, 4.5f);
	walls_transform[25]->SetTranslation(11.0f, 0.0f, 3.0f);
	walls_transform[26]->SetTranslation(9.0f, 0.0f, 3.0f);
	walls_transform[27]->SetTranslation(7.0f, 0.0f, 3.0f);
	walls_transform[28]->SetTranslation(5.0f, 0.0f, 3.0f);
	//done
	walls_transform[29]->SetTranslation(14.0f, 0.0f, 5.5f);
	walls_transform[30]->SetTranslation(13.0f, 0.0f, 5.0f);
	walls_transform[31]->SetTranslation(12.5f, 0.0f, 4.0f);
	walls_transform[32]->SetTranslation(12.5f, 0.0f, 2.0f);

	walls_transform[33]->SetTranslation(14.0f, 0.0f, 1.0f);
	walls_transform[34]->SetTranslation(16.0f, 0.0f, 1.0f);
	walls_transform[35]->SetTranslation(18.0f, 0.0f, 1.0f);
	walls_transform[36]->SetTranslation(19.5f, 0.0f, 1.0f);

	walls_transform[37]->SetTranslation(19.5f, 0.0f, 2.5f);
	walls_transform[38]->SetTranslation(19.5f, 0.0f, 4.5f);
	walls_transform[39]->SetTranslation(19.0f, 0.0f, 5.0f);
	walls_transform[40]->SetTranslation(18.0f, 0.0f, 5.0f);
	walls_transform[41]->SetTranslation(16.5f, 0.0f, 5.5f);


	//---------------------------------------------------------
	walls_transform[42]->SetTranslation(18.0f,0.0f,9.0f);
	walls_transform[43]->SetTranslation(20.0f,0.0f,9.0f);
	walls_transform[44]->SetTranslation(21.0f,0.0f,11.0f);
	walls_transform[45]->SetTranslation(21.0f,0.0f,12.5f);
	int j=0;
	for (int i =46 ; i<=52;i++) {
		walls_transform[i]->SetTranslation(22.0f+j,0.0f,12.5f);
		j=j+2;
	}

	walls_transform[53]->SetTranslation(35.5f,0.0f,12.5f);
	walls_transform[54]->SetTranslation(35.5f,0.0f,11.5f);
	
	walls_transform[55]->SetTranslation(37.5f,0.0f,9.5f);
	walls_transform[56]->SetTranslation(37.5f,0.0f,8.0f);

	walls_transform[57]->SetTranslation(35.5f,0.0f,6.5f);
	
	walls_transform[58]->SetTranslation(35.5f,0.0f,5.0f);

	walls_transform[60]->SetTranslation(36.0f,0.0f,5.5f);
	
	walls_transform[67]->SetTranslation(21.0f,0.0f,7.0f);
	walls_transform[66]->SetTranslation(21.0f,0.0f,5.5f);
	
	j=0;
	for (int i =65 ; i>=59;i--) {
		walls_transform[i]->SetTranslation(22.5f+j,0.0f,5.0f);
		j=j+2;
	}
	walls_transform[68]->SetTranslation(24.0f,0.0f,8.0f);
	walls_transform[69]->SetTranslation(24.0f,0.0f,9.0f);

	walls_transform[72]->SetTranslation(31.0f,0.0f,10.0f);
	walls_transform[71]->SetTranslation(31.0f,0.0f,8.0f);

	walls_transform[70]->SetTranslation(24.0f,0.0f,10.5f);
	walls_transform[76]->SetTranslation(32.5f,0.0f,7.0f);
	
	walls_transform[73]->SetTranslation(32.5f,0.0f,10.5f);

	walls_transform[74]->SetTranslation(33.5f,0.0f,10.0f);
	walls_transform[75]->SetTranslation(33.5f,0.0f,8.0f);

	walls_transform[77]->SetTranslation(36.5f,0.0f,9.0f);


	golden_dragon_transform[0]->SetRotation(180.0f,0.0f,1.0f,0.0f);
	golden_dragon_transform[0]->SetTranslation(4.f,0,7.f);
	golden_dragon_transform[1]->SetRotation(270.0f,0.0f,1.0f,0.0f);
	golden_dragon_transform[1]->SetTranslation(8.5f,0,7.f);
	golden_dragon_transform[2]->SetTranslation(4.f,0,13.f);
	golden_dragon_transform[3]->SetRotation(180.0f,0.0f,1.0f,0.0f);
	golden_dragon_transform[3]->SetTranslation(13.5f,0,4.5f);
	golden_dragon_transform[4]->SetTranslation(19.f,0,2.f);
	golden_dragon_transform[5]->SetTranslation(13.0f,0,12.5f);
	golden_dragon_transform[6]->SetRotation(180.0f,0.0f,1.0f,0.0f);
	golden_dragon_transform[6]->SetTranslation(13.f,0,18.f);
	
	golden_dragon_transform[7]->SetTranslation(35.f,0,6.f);
	golden_dragon_transform[8]->SetRotation(270.0f,0.0f,1.0f,0.0f);
	golden_dragon_transform[8]->SetTranslation(35.f,0,12.f);

	golden_dragon_transform[9]->SetRotation(280.0f,0.0f,1.0f,0.0f);
	golden_dragon_transform[9]->SetTranslation(37.0f,0,9.f);

	pedestal_transform[0]->SetTranslation(4.f,0,7.f);
	pedestal_transform[1]->SetTranslation(8.5f,0,7.f);
	pedestal_transform[2]->SetTranslation(4.f,0,13.f);
	pedestal_transform[3]->SetTranslation(13.5f,0,4.5f);
	pedestal_transform[4]->SetTranslation(19.f,0,2.f);
	pedestal_transform[5]->SetTranslation(13.0f,0,12.5f);
	pedestal_transform[6]->SetTranslation(13.f,0,18.f);
	pedestal_transform[7]->SetTranslation(35.0f,0,6.f);
	pedestal_transform[8]->SetTranslation(35.0f,0,12.f);
	pedestal_transform[9]->SetTranslation(37.0f,0,9.f);
	//-------------------------
	
	lostpakai1_transform->SetTranslation(14.5,0.0f,5);
	lostpakai2_transform->SetTranslation(16,0.0f,5);

	for (int i = 0; i <14; i++)
	{
	
		for(int j=0;j<7 ;j++)
			{
				floor_transform1[i][j]->SetTranslation(22+i,0.0f,j+6);
			}
	}

	for (int i = 0; i <6; i++)
	{
	
		for(int j=0;j<3 ;j++)
			{
				floor_transform2[i][j]->SetTranslation(14+i,0.0f,j+2);
			}
	}

	for (int i = 0; i <5; i++)
	{
	
		for(int j=0;j<2 ;j++)
			{
				floor_transform3[i][j]->SetTranslation(11+i,0.0f,j+13);
			}
	}

	for (int i = 0; i <q; i++)
	{
	
		for(int j=0;j<w ;j++)
			{
				floor_transform4[i][j]->SetTranslation(4+i,0.0f,j+4);
			}
	}

	
	
	

	totem3_transform->SetTranslation(17.5f,0.0f,3.0f);
	totem4_transform->SetTranslation(26.0f,0.0f,9.0f);
	totem5_transform->SetTranslation(29.0f,0.0f,9.0f);

	

	player_transform->SetRotation(180.0f,0.0f,1.0f,0.0f);
	
	totems.push_back(new Totem(totem3_transform,arrow_mesh));
	totems.push_back(new Totem(totem4_transform,arrow_mesh));
	totems.push_back(new Totem(totem5_transform,arrow_mesh));
}



bool LoadObjModels()
{
	OBJLoader* objLoader = new OBJLoader();

	// for all scenes
	spherewhiteMesh = objLoader->loadMesh("sphere_white.obj", "..\\..\\Data\\Other", true);
	groundwhiteMesh = objLoader->loadMesh("plane_white.obj", "..\\..\\Data\\Other", true);



	// for scene 1
	small_room_mesh = objLoader->loadMesh("Room1_Simple_Small.obj", "..\\..\\Data\\Dungeon", true);
	corridor_medium_mesh = objLoader->loadMesh("Corridor1_Medium.obj", "..\\..\\Data\\Dungeon", true);
	corridor_T_mesh = objLoader->loadMesh("Corridor1_Medium_T_Junction.obj", "..\\..\\Data\\Dungeon", true);
	corridor_turn_wide_mesh = objLoader->loadMesh("Corridor1_Wide_Turn.obj", "..\\..\\Data\\Dungeon", true);
	hall_mesh = objLoader->loadMesh("Hall1.obj", "..\\..\\Data\\Dungeon", true);
	wall0_5_mesh = objLoader->loadMesh("Wall1_Pillar_H1.0.obj", "..\\..\\Data\\Dungeon", true);
	wall1_mesh = objLoader->loadMesh("Wall1_Half_H1.0.obj", "..\\..\\Data\\Dungeon", true);
	wall2_mesh = objLoader->loadMesh("Wall1_H1.0.obj", "..\\..\\Data\\Dungeon", true);
	//sphereEarthMesh = objLoader->loadMesh("sphere_earth.obj", "..\\..\\Data\\Other", true);

	playerMesh = objLoader->loadMesh("Warrior.obj", "..\\..\\Data\\Dungeon", true);
	floortiles2 = objLoader->loadMesh("FloorTiles2.obj", "..\\..\\Data\\Dungeon", true);
	floortiles1=objLoader->loadMesh("FloorTiles1.obj", "..\\..\\Data\\Dungeon", true);


	spikes_mesh = objLoader->loadMesh("Spikes.obj", "..\\..\\Data\\Dungeon", true);
	spikeTrap_mesh = objLoader->loadMesh("SpikeTrap.obj", "..\\..\\Data\\Dungeon", true);
	totem_mesh = objLoader->loadMesh("Totem.obj", "..\\..\\Data\\Dungeon", true);
	arrow_mesh = objLoader->loadMesh("Arrow.obj", "..\\..\\Data\\Dungeon", true);

	golden_dragon=objLoader->loadMesh("GoldenDragon.obj", "..\\..\\Data\\Dungeon", true);
	pedestal= objLoader->loadMesh("Pedestal.obj", "..\\..\\Data\\Dungeon", true);

	doorWayMesh=objLoader->loadMesh("Doorway1_Side.obj", "..\\..\\Data\\Dungeon", true);
	doorMesh=objLoader->loadMesh("Door1.obj", "..\\..\\Data\\Dungeon", true);

	

	return true;
}

bool CreateShaders()
{
	// Basic Geometry Shader (the shader with no lighting calculations, similar to the previous labs)
	// The only difference here is that we are storing the shader pointer, the program id and the uniforms in 
	// a struct instead of using global variables
	// All the shaders below are located in the Shaders.h file

	// Basic Geometry shader
	basic_geometry_shader = new BasicGeometryShader();
	basic_geometry_shader->shader = new ShaderGLSL("BasicGeometry");
	// compile
	bool shader_loaded = basic_geometry_shader->shader->LoadAndCompile();
	if (!shader_loaded) return false;
	// get the program id
	basic_geometry_shader->program_id = basic_geometry_shader->shader->GetProgram();
	// check for uniforms
	basic_geometry_shader->uniform_m = glGetUniformLocation(basic_geometry_shader->program_id, "uniform_m");
	basic_geometry_shader->uniform_v = glGetUniformLocation(basic_geometry_shader->program_id, "uniform_v");
	basic_geometry_shader->uniform_p = glGetUniformLocation(basic_geometry_shader->program_id, "uniform_p");
	basic_geometry_shader->uniform_color = glGetUniformLocation(basic_geometry_shader->program_id, "uniform_material_color");

	// these are for the samplers
	basic_geometry_shader->uniform_sampler_diffuse = glGetUniformLocation(basic_geometry_shader->program_id, "uniform_sampler_diffuse");
	basic_geometry_shader->uniform_has_sampler_diffuse = glGetUniformLocation(basic_geometry_shader->program_id, "uniform_has_sampler_diffuse");

	// Spotlight light shader
	// This is used for rendering geometry using a spotlight shader
	spotlight_shader = new SpotLightShader();
	spotlight_shader->shader = new ShaderGLSL("SpotLight");
	// compile
	shader_loaded = spotlight_shader->shader->LoadAndCompile();
	if (!shader_loaded) return false;
	// get the program id
	spotlight_shader->program_id = spotlight_shader->shader->GetProgram();
	// check for uniforms
	spotlight_shader->uniform_m = glGetUniformLocation(spotlight_shader->program_id, "uniform_m");
	spotlight_shader->uniform_v = glGetUniformLocation(spotlight_shader->program_id, "uniform_v");
	spotlight_shader->uniform_p = glGetUniformLocation(spotlight_shader->program_id, "uniform_p");
	spotlight_shader->uniform_color = glGetUniformLocation(spotlight_shader->program_id, "uniform_material_color");
	spotlight_shader->uniform_normal_matrix_ecs = glGetUniformLocation(spotlight_shader->program_id, "uniform_normal_matrix_ecs");
	spotlight_shader->uniform_light_color = glGetUniformLocation(spotlight_shader->program_id, "uniform_light_color");
	spotlight_shader->uniform_light_position_ecs = glGetUniformLocation(spotlight_shader->program_id, "uniform_light_position_ecs");
	spotlight_shader->uniform_light_direction_ecs = glGetUniformLocation(spotlight_shader->program_id, "uniform_light_direction_ecs");

	// these are for the samplers
	spotlight_shader->uniform_sampler_diffuse = glGetUniformLocation(spotlight_shader->program_id, "uniform_sampler_diffuse");
	spotlight_shader->uniform_sampler_normal = glGetUniformLocation(spotlight_shader->program_id, "uniform_sampler_normal");
	spotlight_shader->uniform_sampler_specular = glGetUniformLocation(spotlight_shader->program_id, "uniform_sampler_specular");
	spotlight_shader->uniform_sampler_emission = glGetUniformLocation(spotlight_shader->program_id, "uniform_sampler_emission");
	spotlight_shader->uniform_has_sampler_diffuse = glGetUniformLocation(spotlight_shader->program_id, "uniform_has_sampler_diffuse");
	spotlight_shader->uniform_has_sampler_normal = glGetUniformLocation(spotlight_shader->program_id, "uniform_has_sampler_normal");
	spotlight_shader->uniform_has_sampler_specular = glGetUniformLocation(spotlight_shader->program_id, "uniform_has_sampler_specular");
	spotlight_shader->uniform_has_sampler_emission = glGetUniformLocation(spotlight_shader->program_id, "uniform_has_sampler_emission");

	// these are for the shadow calculations
	spotlight_shader->uniform_sampler_shadow_map = glGetUniformLocation(spotlight_shader->program_id, "uniform_sampler_shadow_map");
	spotlight_shader->uniform_view_inverse = glGetUniformLocation(spotlight_shader->program_id, "uniform_view_inverse");
	spotlight_shader->uniform_light_view_projection = glGetUniformLocation(spotlight_shader->program_id, "uniform_light_view_projection");	
	
	// Shadow Map shader
	// This is used for rendering the geometry to the shadow map depth buffer
	shadowmap_shader = new ShadowMapShader();
	shadowmap_shader->shader = new ShaderGLSL("BasicShadow");
	// compile
	shader_loaded = shadowmap_shader->shader->LoadAndCompile();
	if (!shader_loaded) return false;
	// get the program id
	shadowmap_shader->program_id = shadowmap_shader->shader->GetProgram();
	// check for uniforms
	shadowmap_shader->uniform_mvp = glGetUniformLocation(shadowmap_shader->program_id, "uniform_mvp");

	// all shaders loaded OK
	return true;
}


// Render function. Every time our window has to be drawn, this is called.
int oldTimeSinceStart;

void Render(void)
{
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = timeSinceStart - oldTimeSinceStart;
    oldTimeSinceStart = timeSinceStart;

	for (std::vector<Trap*>::iterator it = traps.begin(); it != traps.end(); ++it)
		(*it)->adjustSpikes(deltaTime);
	for (std::vector<Totem*>::iterator it = totems.begin(); it != totems.end(); ++it)
		(*it)->rotateTotem(deltaTime);
	
	glClearColor(0.17, 0.34, 0.39, 0.0);
	
	glClear(GL_COLOR_BUFFER_BIT);
	

	glm::vec3 pp = player_transform->GetWorldPosition();
	
	sdir=s*glm::vec3(cos(theta),sin(fi),sin(theta)*cos(fi));

	eye = pp+glm::vec3(0.0f, 1.0f, 0.0)+sdir;

	target = pp+glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 ppos = player_transform->GetTranslation();
	
	door->checkDoor(ppos.x,ppos.z);

	//PrintToOutputWindow("fi%f",fi);
	//PrintToOutputWindow("theta%f",theta);

	


	if(lifes==0&& (!hasDied)&&numOfDragonsCollected!=10)
	{
		PrintToOutputWindow("DEAD!!!!!!!!!!DEAD!!!!!!!!!!DEAD!!!!!!!!!!");
		player_transform->SetRotation(90,1,0,0);
		hasDied=true;
	}
	else if(isHazzard(ppos.z*2,ppos.x*2)&& (!hasDied)&&numOfDragonsCollected!=10)
	{
		PrintToOutputWindow("Lost life");
		

		//
		 
		 lifes--;
		 PrintToOutputWindow("Lifes left:%d",lifes);
		 if(lifes!=0)
		 {
			 player_transform->SetTranslation(2,0,2);
			 player_transform->SetRotation(180,0,1,0);
			 theta=4.70f; 
			 fi=0.40f;
		 }
		
	}

	if(numOfDragonsCollected==10)
	{
		 player_transform->SetRotation( ((int)player_transform->GetRotationAngle()+5)%360,0,1,0);
	}
	
	if (root == nullptr) return;




	// Set the rendering mode
	glPolygonMode(GL_FRONT_AND_BACK, rendering_mode);


	// CAMERA SETUP
	world_to_camera_matrix = glm::lookAt(eye, target, up);

	// change the rotation angle for the lights
	// this allows for the lights to be rotated
	light_rotationY += 0.4f;
	if (light_rotationY > 360.0f) light_rotationY -= 360.0f;
	
	spotlight->m_transformed_position = spotlight->m_initial_position;
	spotlight->m_transformed_target = spotlight->m_initial_target;
	// Uncomment this to rotate light (also need to uncomment the DrawSpotLightSource to get the sphere that rotates the light to rotate as well)
	spotlight->m_transformed_position = glm::vec3(glm::rotate(light_rotationY, 0.0f, 1.0f, 0.0f) * glm::vec4(spotlight->m_initial_position, 1.0f));
	
	// Write to Shadow Map FBO
	DrawSceneToShadowFBO();

	// now, render the scene as usual
	SceneGraphDraw();
	

	
	glutSwapBuffers();     
	
	glError();
	
}

// Release all memory allocated by pointers using new
void ReleaseGLUT()
{
	
}

void DrawSceneToShadowFBO()
{
	// switch the rendering to happen on the FBO rather than the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	// clear the depth color for this framebuffer
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Create the worldviewprojection matrix for the light source
	// Again, since a light node is missing, the world transformation needs to be applied
	glm::mat4x4 wld = glm::translate(world_translate) * glm::rotate(world_rotate_x, 0.0f, 1.0f, 0.0f);
	spotlight->m_transformed_position = glm::vec3(wld * glm::vec4(spotlight->m_transformed_position, 1.0f));
	spotlight->m_transformed_target = glm::vec3(wld * glm::vec4(spotlight->m_transformed_target, 1.0f));

	// Create the light's world to view space matrix
	// we need to build a "camera" as viewed from the light
	// so we need an up vector, a target and a light "eye" position
	// create the direction vector
	glm::vec3 light_direction = glm::normalize(spotlight->m_transformed_target - spotlight->m_transformed_position);
	
	// this check is simply a sanity check for the internal cross product in glm::lookAt
	// just in case the light direction vector is 0,1,0
	// if it is, the up vector is set to 0,0,1
	glm::vec3 up;
	if (fabs(light_direction.z) < 0.001 && fabs(light_direction.x) < 0.001)
		up = glm::vec3(0,0,1);
	else
		up = glm::vec3(0,1,0);

	// construct the light view matrix that transforms world space to light view space (WCS -> LCS)
	// LCS is the view space of the light, similar to ECS which is the view space for the camera
	glm::mat4x4 world_to_light_view_matrix = glm::lookAt(spotlight->m_transformed_position, spotlight->m_transformed_target, up);

	//float h = spotlight->m_near_range *glm::tan(glm::radians(spotlight->m_aperture * 0.5f));
	//glm::mat4x4 light_projection_matrix = glm::frustum(-h, h, -h, h, spotlight->m_near_range, spotlight->m_far_range);
	// aspect ratio is 1 since both width and height are the same (dimensions of the texture)
	glm::mat4x4 light_projection_matrix = glm::perspective(90.0f, 1.0f, spotlight->m_near_range, spotlight->m_far_range);

	// also we need to set a new viewport
	// this viewport has the dimensions of the target depth texture (the shadow texture)
	glViewport(0.0f, 0.0f, shadow_dimensions, shadow_dimensions);

	// now draw the scene as usual

	// USE SCENE GRAPH
	root->SetLightViewMat(world_to_light_view_matrix);
	root->SetLightProjectionMat(light_projection_matrix);

	// also set the world transformations here since they will be retrieved as part of the M matrix in the GeometryNode
	world_transform->SetTranslation(world_translate.x, world_translate.y, world_translate.z);
	world_transform->SetRotation(world_rotate_x, 0.0f, 1.0f, 0.0f);
	
	// Draw the scene by traversing the scene graph
	// 0 renders the geometry using the spotlight shader
	// 1 renders the geometry to the shadow map
	root->Draw(1);

	// unbind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DrawSpotLightSource()
{
	// draw the light source
	// we draw the light source (for the omni light) by translating a small sphere onto the light's position
	// this is the local transform (S T R) which scales, translates and rotates on the translated orbit
	glm::mat4x4 obj = glm::translate(glm::vec3(spotlight->m_initial_position)) * glm::scale(0.2f, 0.2f, 0.2f);
	// rotate the sphere so that it follows the light
	obj = glm::rotate(light_rotationY, 0.0f, 1.0f, 0.0f) * obj;

	// we also need to build the world transform (since we can rotate the scene with the mouse or move the world with the arrow keys)
	glm::mat4x4 wld = glm::translate(world_translate) * glm::rotate(world_rotate_x, 0.0f, 1.0f, 0.0f);
	obj = wld * obj;
		
	// draw the mesh
	DrawOGLMesh(spherewhiteMesh, obj);
}

void SceneGraphDraw()
{
	// right before rendering we need to set the target framebuffer to
	// the default framebuffer
	// this is because we used an FBO to switch the rendering to another location
	// set the back buffer as the target framebuffer
	glDrawBuffer(GL_BACK);

	// Clear depth and color buffers.
	// We can clear both using an | operand to create the buffer-clear mask.
	// glClear is called here as well because we are working on the default framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the viewport back to the actual window dimensions (it was changed by the FBO rendering code)
	glViewport(0.0f, 0.0f, current_width, current_height);

	// draw the light source for the spot light
	DrawSpotLightSource();

	// USE SCENE GRAPH
	root->SetViewMat(world_to_camera_matrix);
	root->SetProjectionMat(perspective_projection_matrix);

	// set current active light
	// in this example, the sunlight is set as the active directional light
	// and the candle light as the active omni light
	// to draw using the directional light, we pass 0 to the Draw function
	// and to draw using the omni light, we pass 1 to the Draw function
	// to draw using multiple lights, we simply need to enable blending
	// and use an additive blend function glBlendFunc(GL_ONE, GL_ONE)
	// since lights are additive
	root->SetActiveSpotlight(spotlight);
	
	// Draw the scene by traversing the scene graph
	// 0 renders the geometry using the spotlight shader
	// 1 renders the geometry to the shadow map
	root->Draw(0);
}

void removeDragonFromBorder(int py,int px) 
{
	float y= py;
	float x = px;
	glm::vec3 temp =  glm::vec3(x,0.f,y);
	
	
	for(int i=0;i<10;i++)
	{
		///PrintToOutputWindow("the translation is %f %f", golden_dragon_transform[i]->GetTranslation().x ,  golden_dragon_transform[i]->GetTranslation().z );
		//PrintToOutputWindow("the temp is %f %f", x , y );
		
	
		for (float g= -2; g<=2 ; g+=0.5)
		{
			for (float h= -2; h<=2 ; h+=0.5) 
			{
				if(((golden_dragon_transform[i]->GetTranslation().x )*2 == (temp.x+g) &&(golden_dragon_transform[i]->GetTranslation().z )*2 ==( temp.z+h)))
				{
					removeDragon(y,x);
					setWall(y,x);
					golden_dragon_transform[i]->RemoveChild(golden_dragon_geom[i]);
					break;
				}
			}
			
		}
	}
}

// Keyboard callback function.
// When a key is pressed, GLUT calls this, passing the keys character in the key parameter. 
// The x,y values are the window mouse coordinates when the key was pressed
void Keyboard(unsigned char key, int x, int y)
{
	
		
	//PrintToOutputWindow("Keyboard");
	float previous_rot = player_transform->GetRotationAngle(); 
	//PrintToOutputWindow("%f", previous_rot);
	float dx=0.0f,dz=0.0f;
	switch(key)
	{
	case 'w':
	case 'W':
		//player_transform->SetRotation(180.0f,0.f,1.f,0.f);
		
		if(previous_rot==180)
			dz+=0.1;
		else if(previous_rot==90)
			dx-=0.1f;
		else if(previous_rot==0)
			dz-=0.1f;
		else if(previous_rot==-90 || previous_rot==270)
			dx+=0.1f;
		//eye.z -= 1.0f;
		break;
	case 's':
	case 'S':
		//eye.z += 1.0f;
		if(!hasDied)
		player_transform->SetRotation(0.0f,0.f,1.f,0.f);
		//dz-=0.1f*cos(player_transform->GetRotationAngle());
		//dx-=0.1f*sin(player_transform->GetRotationAngle());
		//dz-=0.1f;
		break;
	case 'd':
	case 'D':
		//eye.x -= 1.0f;
		if(!hasDied)
		if(previous_rot==0) {
		player_transform->SetRotation(270.f,0.f,1.f,0.f);
		}
		else {
		player_transform->SetRotation(previous_rot-90.f,0.f,1.f,0.f);
		}
		//dx+=0.1f;
		break;
	case 'a':
	case 'A':
		previous_rot= ((int)previous_rot+90)%(360);
		if(!hasDied)
		player_transform->SetRotation((float)previous_rot,0.f,1.f,0.f);
		//dx-=0.1f;
		//eye.x += 1.0f;
		break;
	case 'e':
	case 'E':
		//eye.y += 1.0f;
		s+=0.5f;
		break;
	case 'q':
	case 'Q':
		//eye.y += 1.0f;
		s-=0.5f;
		break;
	case 'f':
	case 'F':
		//eye.y -= 1.0f;
		glutFullScreenToggle();
		PrintToOutputWindow("width %d ",glutGet(GLUT_WINDOW_WIDTH));
		PrintToOutputWindow("Heigh %d ",glutGet(GLUT_WINDOW_HEIGHT));
		PrintToOutputWindow("X: %d ",glutGet(GLUT_WINDOW_X));
		PrintToOutputWindow("Y: %d ",glutGet(GLUT_WINDOW_Y));
		
		break;
	case 27: // escape
		glutLeaveMainLoop();
		return;
	default:
		return;
		

	}
	dz*=z_speed;
	dx*=z_speed;
	glm::vec3 move = glm::vec3(dx,0.0f,dz);
	glm::vec3 newPos = player_transform->GetTranslation()+move; 
	
	int px=newPos.x * 2;
	int py=newPos.z * 2;
	/*PrintToOutputWindow("%d",px);
	PrintToOutputWindow("%d",py);
	PrintToOutputWindow("%s","########################################");*/
 
	if ((isWall(py,px)==false) && (isDragon(py,px)==false)&&(!hasDied)) {
		player_transform->SetTranslation(newPos);
	}
	 if(/*isWall(py,px)==false && */isDragon(py,px)==true) {
		//player_transform->SetTranslation(newPos);
		removeDragonFromBorder(py,px);
		numOfDragonsCollected++;
		PrintToOutputWindow("dragons collected: %d",numOfDragonsCollected);
	}
	 if(numOfDragonsCollected==10)
	 {
		 PrintToOutputWindow("%s","All dragons collected");
	 }

	//glm::vec3 oldPos = player_transform->GetTranslation();
	//player_transform->SetTranslation(oldPos+move);
	//player_transform->SetTranslation(newPos);
}




// Special Keyboard callback function.
// When a special key is pressed (such as arrows or function keys), GLUT calls this, passing the keys character in the key parameter. 
// The x,y values are the window mouse coordinates when the key was pressed
void KeyboardSpecial(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		if (rendering_mode == GL_POINT) rendering_mode = GL_LINE;
		else if (rendering_mode == GL_LINE) rendering_mode = GL_FILL;
		else if (rendering_mode == GL_FILL) rendering_mode = GL_POINT;
	case GLUT_KEY_F2:
		break;
	case GLUT_KEY_PAGE_UP:
		world_translate.y += 1.0f;
		break;
	case GLUT_KEY_PAGE_DOWN:
		world_translate.y -= 1.0f;
		break;
	case GLUT_KEY_UP:
		world_translate.z += 1.0f;
		break;
	case GLUT_KEY_DOWN:
		world_translate.z -= 1.0f;
		break;
	case GLUT_KEY_LEFT:
		world_translate.x -= 1.0f;
		break;
	case GLUT_KEY_RIGHT:
		world_translate.x += 1.0f;
		break;
	default:
		return;
	}
}

// This function redraws the scene every 16ms
void TimerSync(int ms)
{
	glutPostRedisplay();
	glutTimerFunc(16, TimerSync, 16);
}

// Mouse callback function
// button refers to the mouse button, state refers to whether button is in up or down state
// and x, y are the new mouse coordinates
void Mouse(int button, int state, int x, int y)
{
	//PrintToOutputWindow("move");
	//OutputDebugString("Entered Mouse\n");
	if (state == GLUT_DOWN)
	{
		prev_x = x;
		prev_y = y;
		

		//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 32);
	}
	//PrintToOutputWindow("Down1");
	
	//
		//prev_x = x;
		//prev_y = y;
	//
	mouse_button = button;
}


void Sprint( float x,float y,float z, char *message)
{
	glColor3f( 1.0f, 0.5f, 0.5f );
  glRasterPos2f(50, 50);
  int len, i;
  len = (int)strlen(message);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
  }
}


// Method to handle the mouse motion
void MouseMotion(int x, int y)
{
	//PrintToOutputWindow("Down2");
	//
	//PrintToOutputWindow("move8");
	/*target.x -= (x - prev_x) * 0.05f;
		target.y += (y - prev_y) * 0.05f;

		prev_x = x;
		prev_y = y;*/
	//
	//OutputDebugString("Entered Mouse Motion\n")
	switch (mouse_button)
	{
	case GLUT_LEFT_BUTTON:
		target.x -= (x - prev_x) * 0.05f;
		target.y += (y - prev_y) * 0.05f;

		prev_x = x;
		prev_y = y;
		//glutPostRedisplay();
		break;
	case GLUT_RIGHT_BUTTON:

		world_rotate_x += (x - prev_x) * 0.1f;

		prev_x = x;
		prev_y = y;

		//glutPostRedisplay();
		break;
	default:
		break;
	}
}

static float lastx = 0.0;
static float lasty = 0.0;


void MousePassiveMotion(int x, int y)
{
	

    

  /*  if((abs((int)lastx)>10)||(abs((int)lasty)>10))
    {
        lastx = (float)x;
        lasty = (float)y;

    }*/
	

	// Ama to prohgoumeno y einai arketa megalutero
	// Afto to kanoume gia na mhn uparxoun apotomes metavoles
	
	if(std::abs(lastx - x) > 3) {
		if(x>lastx)
			//world_rotate_x += (lastx-x) * rotate_Factor;
			//eye.x+=(lastx)*0.1;
			theta+=camera_rotate_y_factor;
		else
			theta-=camera_rotate_y_factor;
	}
	
	if(std::abs(y - lasty) > 3) {
		if(y>lasty)
			//world_rotate_x += (lastx-x) * rotate_Factor;
			//eye.x+=(lastx)*0.1;
			fi+=camera_rotate_x_factor;
		else
			fi-=camera_rotate_x_factor;
			//world_rotate_x -= (lastx-x) * rotate_Factor;
			//world_rotate_x -= (500) * rotate_Factor;
	}
	//if(y>lastx)
		//world_rotate_y += (500) * rotate_Factor;
		//eye.x+=(lastx)*0.1;
    //else
		//world_rotate_y+= (lasty-y) * rotate_Factor;
	int width =glutGet(GLUT_WINDOW_WIDTH);
	int heght = glutGet(GLUT_WINDOW_HEIGHT);
	int windowX = glutGet(GLUT_WINDOW_X);
	int windowY = glutGet(GLUT_WINDOW_Y);
	if(x>width-150)
		glutWarpPointer(180,y);
	if(x<150)
		glutWarpPointer(width-180,y);
	if(fi>3.0f*PI/8.0f)
		fi=3.0f*PI/8.0f;
	else if(fi<PI/8.0f)
		fi=PI/8.0f;
    //    //Camera.RotateY(lastx*0.01);
    //if((float)y > lasty)
    //    Camera.RotateX(lasty*0.01);
    //else 
    //    Camera.RotateX(-lasty*0.01); 
   //lastx = (float)x;
    //lasty = (float)y;
    //Display();
	lastx = x;//(float)x-lastx;
    lasty = y;//(float)y-lasty;
	
	
}

// Resize callback function
// The parameters indicate the new width and height.
void Resize(int width, int height)
{ 
	// Hack to void zero height
	if (height == 0) height = 1;


	// -------------------------------------------VIEWPORT----------------------------------------------//
	// Set up the viewport (the part of the window where the rendering happens)
	// 0, 0 are the coordinates at the LOWER LEFT corner of the viewport rectangle, in pixels
	// Width, height Specify the width and height of the viewport, in pixels.
	// Typically, this is the size of the window
	// This information will be used for the viewport transformation
	glViewport(0, 0, width, height);
	// -------------------------------------------------------------------------------------------------//
	
	
	// aspect ratio of the window
	float aspect_ratio = (float) width / (float) height;
	// value for the near clipping plane. Anything before that will be clipped
	float near_field_value = 1.0f;
	// value for the far clipping plane. Anything beyond that will be clipped
	float far_field_value = 1000.0f;

	// --------------------------------PERSPECTIVE PROJECTION-------------------------------------------//
	// each time we resize the window, a new projection matrix must be defined
	// use GLM to create a perspective projection matrix (good for 3D rendering)
	// vertical field of view angle
	float vertical_FOV = 30.0f;

	// use GLM to create a perspective projection matrix
	perspective_projection_matrix = glm::perspective(vertical_FOV, aspect_ratio, near_field_value, far_field_value);

	// -------------------------------------------------------------------------------------------------//

	// keep these values globally to reset the viewport after rendering to an FBO
	current_width = width;
	current_height = height;
}

// This is the rendering code (taken from the Draw in the GeometryNode) that renders a mesh
// loaded from an OBJ file
// This is used for the Blending example and since this is shown better without any lighting effects
// we only use the Basic Geometry shader
void DrawOGLMesh(OGLMesh* mesh, glm::mat4x4& object_to_world_transform)
{
	// bind the VAO
	glBindVertexArray(mesh->vao);

	// get the world transformation
	glm::mat4x4& M = object_to_world_transform;
	// get the view transformation
	glm::mat4x4& V = world_to_camera_matrix;
	// get the projection transformation
	glm::mat4x4& P = perspective_projection_matrix;

	std::vector<OBJMaterial*>& materials = mesh->materials;

	// set the shader active
	glUseProgram(basic_geometry_shader->program_id);

	// pass any global shader parameters (independent of material attributes)
	glUniformMatrix4fv(basic_geometry_shader->uniform_m, 1, false, &M[0][0]);
	glUniformMatrix4fv(basic_geometry_shader->uniform_v, 1, false, &V[0][0]);
	glUniformMatrix4fv(basic_geometry_shader->uniform_p, 1, false, &P[0][0]);

	// loop through all the elements
	for (GLint i=0; i < mesh->num_elements; i++)
	{
		if (mesh->elements[i].triangles==0)
			continue;

		// Material and texture goes here.
		int mtrIdx = mesh->elements[i].material_index;
		OBJMaterial& cur_material = *materials[mtrIdx];

		// use the material color
		glUniform4f(basic_geometry_shader->uniform_color, cur_material.m_diffuse[0], cur_material.m_diffuse[1], cur_material.m_diffuse[2], cur_material.m_opacity);

		// draw within a range in the index buffer
		glDrawRangeElements(
			GL_TRIANGLES, 
			mesh->elements[i].start_index, 
			mesh->elements[i].start_index+mesh->elements[i].triangles*3, 
			mesh->elements[i].triangles*3, 
			GL_UNSIGNED_INT, 
			(void*)(mesh->elements[i].start_index*sizeof(GLuint))
			);
	}

	glBindVertexArray(0);
}