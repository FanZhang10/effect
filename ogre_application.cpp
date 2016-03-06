#include "ogre_application.h"
#include "bin/path_config.h"

namespace ogre_application {

/* Some configuration constants */
/* They are written here as global variables, but ideally they should be loaded from a configuration file */

/* Initialization */
const Ogre::String config_filename_g = "";
const Ogre::String plugins_filename_g = "";
const Ogre::String log_filename_g = "Ogre.log";

/* Main window settings */
const Ogre::String window_title_g = "Demo";
const Ogre::String custom_window_capacities_g = "";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

/* Viewport and camera settings */
float viewport_width_g = 0.95f;
float viewport_height_g = 0.95f;
float viewport_left_g = (1.0f - viewport_width_g) * 0.5f;
float viewport_top_g = (1.0f - viewport_height_g) * 0.5f;
unsigned short viewport_z_order_g = 100;
const Ogre::ColourValue viewport_background_color_g(0.0, 0.0, 0.0);
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 100.0;
Ogre::Vector3 camera_position_g(0.5, 0.5, 1.5);
Ogre::Vector3 camera_look_at_g(0.0, 0.0, 0.0);
Ogre::Vector3 camera_up_g(0.0, 1.0, 0.0);

/* Materials */
const Ogre::String material_directory_g = MATERIAL_DIRECTORY;


OgreApplication::OgreApplication(void){

    /* Don't do work in the constructor, leave it for the Init() function */
}


void OgreApplication::Init(void){

	/* Set default values for the variables */
	animating_ = false;
	space_down_ = false;
	animation_state_ = NULL;
	input_manager_ = NULL;
	keyboard_ = NULL;
	mouse_ = NULL;

	// For particles
	timer_ = 0.0;
	animating_particles_ = true;

	/* Run all initialization steps */
    InitRootNode();
    InitPlugins();
    InitRenderSystem();
    InitWindow();
    InitViewport();
	InitEvents();
	InitOIS();
	LoadMaterials();
}


void OgreApplication::InitRootNode(void){

    try {
		
		/* We need to have an Ogre root to be able to access all Ogre functions */
        ogre_root_ = std::auto_ptr<Ogre::Root>(new Ogre::Root(config_filename_g, plugins_filename_g, log_filename_g));
		//ogre_root_->showConfigDialog();

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
		throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitPlugins(void){

    try {

        Strings plugin_names;
        plugin_names.push_back("RenderSystem_GL");
		
        Strings::iterator iter = plugin_names.begin();
        Strings::iterator iter_end = plugin_names.end();
        for (; iter != iter_end; iter++){
            Ogre::String& plugin_name = (*iter);
            if (OGRE_DEBUG_MODE){
                plugin_name.append("_d");
            }
            ogre_root_->loadPlugin(plugin_name);
        }

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitRenderSystem(void){

    try {

        const Ogre::RenderSystemList& render_system_list = ogre_root_->getAvailableRenderers();
        if (render_system_list.size() == 0)
        {
			throw(OgreAppException(std::string("OgreApp::Exception: Sorry, no rendersystem was found.")));
        }

        Ogre::RenderSystem *render_system = render_system_list.at(0);
        ogre_root_->setRenderSystem(render_system);

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}

        
void OgreApplication::InitWindow(void){

    try {

        /* Create main window for the application */
		bool create_window_automatically = false;
        ogre_root_->initialise(create_window_automatically, window_title_g, custom_window_capacities_g);

        Ogre::NameValuePairList params;
        params["FSAA"] = "0";
        params["vsync"] = "true";
        ogre_window_ = ogre_root_->createRenderWindow(window_title_g, window_width_g, window_height_g, window_full_screen_g, &params);

        ogre_window_->setActive(true);
        ogre_window_->setAutoUpdated(false);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitViewport(void){

    try {

        /* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->createSceneManager(Ogre::ST_GENERIC, "MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

        /* Create camera object */
        Ogre::Camera* camera = scene_manager->createCamera("MyCamera");
        Ogre::SceneNode* camera_scene_node = root_scene_node->createChildSceneNode("MyCameraNode");
        camera_scene_node->attachObject(camera);

		camera->setNearClipDistance(camera_near_clip_distance_g);
        camera->setFarClipDistance(camera_far_clip_distance_g); 

		camera->setPosition(camera_position_g);
		camera->lookAt(camera_look_at_g);
		camera->setFixedYawAxis(true, camera_up_g);

        /* Create viewport */
        Ogre::Viewport *viewport = ogre_window_->addViewport(camera, viewport_z_order_g, viewport_left_g, viewport_top_g, viewport_width_g, viewport_height_g);

        viewport->setAutoUpdated(true);
        viewport->setBackgroundColour(viewport_background_color_g);

		/* Set aspect ratio */
        float ratio = float(viewport->getActualWidth()) / float(viewport->getActualHeight());
        camera->setAspectRatio(ratio);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitEvents(void){

	try {

		/* Add this object as a FrameListener (see frameRenderingQueued event) */
		ogre_root_->addFrameListener(this);

		/* Add this object as a WindowEventListener to handle the window resize event */
		Ogre::WindowEventUtilities::addWindowEventListener(ogre_window_, this);

	}
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
		throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitOIS(void){

	/* Initialize the Object Oriented Input System (OIS) */
	try {

		/* Initialize input manager */
		OIS::ParamList pl; // Parameter list passed to the input manager initialization
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		ogre_window_->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		pl.insert(std::make_pair(std::string("w32_mouse"), 
		std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), 
		std::string("DISCL_NONEXCLUSIVE")));
		input_manager_ = OIS::InputManager::createInputSystem(pl);

		/*size_t hWnd = 0;
		ogre_window_->getCustomAttribute("WINDOW", &hWnd);
		input_manager_ = OIS::InputManager::createInputSystem(hWnd);*/

		/* Initialize keyboard and mouse */
		keyboard_ = static_cast<OIS::Keyboard*>(input_manager_->createInputObject(OIS::OISKeyboard, false));

		mouse_ = static_cast<OIS::Mouse*>(input_manager_->createInputObject(OIS::OISMouse, false));
		unsigned int width, height, depth;
		int top, left;
		ogre_window_->getMetrics(width, height, depth, left, top);
		const OIS::MouseState &ms = mouse_->getMouseState();
		ms.width = width;
		ms.height = height;

	}
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::LoadMaterials(void){

    try {
		
		/* Load materials that can then be assigned to objects in the scene */
		Ogre::String resource_group_name = "MyGame";
		Ogre::ResourceGroupManager& resource_group_manager = Ogre::ResourceGroupManager::getSingleton();
		resource_group_manager.createResourceGroup(resource_group_name);
		bool is_recursive = false;
		resource_group_manager.addResourceLocation(material_directory_g, "FileSystem", resource_group_name, is_recursive);
		resource_group_manager.initialiseResourceGroup(resource_group_name);
		resource_group_manager.loadResourceGroup(resource_group_name);

	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::CreateParticleGeometry(Ogre::String object_name, int num_particles){

	try {
		/* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

        /* Create the 3D object */
        Ogre::ManualObject* object = NULL;
        object = scene_manager->createManualObject(object_name);
        object->setDynamic(false);

        /* Create point list for the object */
		object->begin("", Ogre::RenderOperation::OT_POINT_LIST);

		/* Initialize random numbers */
		std::srand(std::time(0));

		/* Create a set of points which will be the particles */
		/* This is similar to drawing a sphere: we will sample points on a sphere, but will allow them to also
		   deviate a bit from the sphere along the normal (change of radius) */
		float trad = 0.4; // Defines the starting point of the particles
        float maxspray = 0.5; // This is how much we allow the points to deviate from the sphere
		float u, v, w, theta, phi, spray; // Work variables
		for (int i = 0; i < num_particles; i++){
			
			// Randomly select three numbers to define a point in spherical coordinates
			u = ((double) rand() / (RAND_MAX));
            v = ((double) rand() / (RAND_MAX));
            w = ((double) rand() / (RAND_MAX));

			// Use u to define the angle theta along one direction of a sphere
            theta = u * 2.0 * 3.1416;
			// Use v to define the angle phi along the other direction of the sphere
			phi = acos(2.0*v - 1.0);
			// Use we to define how much we can deviate from the surface of the sphere (change of radius)
            spray = maxspray*pow((float) w, (float) (1.0/3.0)); // Cubic root of w

			// Define the normal and point based on theta, phi and the spray
            Ogre::Vector3 normal = Ogre::Vector3(spray*cos(theta)*sin(phi), spray*sin(theta)*sin(phi), spray*cos(phi));
			object->position(normal.x*trad, normal.y*trad, normal.z*trad);
			object->normal(normal);
			object->colour(Ogre::ColourValue(i/(float) num_particles, 0.0, 1.0 - (i/(float) num_particles))); // We can use the color for debug, if needed
			object->textureCoord(Ogre::Vector2(0.0, 0.0));
		}
		
		/* We finished the object */
        object->end();
		
        /* Convert triangle list to a mesh */
        object->convertToMesh(object_name);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::CreateParticleEntity(Ogre::String entity_name, Ogre::String object_name, Ogre::String material_name){

	try {
		/* Create one instance of the torus (one entity) */
		/* The same object can have multiple instances or entities */

		/* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

		/* Create entity */
        Ogre::Entity* entity = scene_manager->createEntity(object_name);

		/* Apply a material to the entity */
		entity->setMaterialName(material_name);
		particle_material_name_ = material_name;

		float x, y, z;
		 x = ((double) rand() / (RAND_MAX))-0.5;
         y = ((double) rand() / (RAND_MAX))-0.5;
         z = ((double) rand() / (RAND_MAX))-0.5;
		 
		 Ogre::Vector3 pos(x,y,z);
		
		/* Create a scene node for the entity */
		/* The scene node keeps track of the entity's position */
        Ogre::SceneNode* scene_node = root_scene_node->createChildSceneNode(entity_name);
        scene_node->attachObject(entity);
		scene_node->setPosition(pos);


		/* Scale the entity */
		scene_node->scale(0.5, 0.5, 0.5);
	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::MainLoop(void){

    try {

        /* Main loop to keep the application going */

        ogre_root_->clearEventTimes();

        while(!ogre_window_->isClosed()){
            ogre_window_->update(false);

			ogre_window_->swapBuffers();

			ogre_root_->renderOneFrame();

            Ogre::WindowEventUtilities::messagePump();
        }
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::SetupAnimation(Ogre::String entity_name){

	/* Retrieve scene manager and root scene node */
    Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
	Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

	/* Set up animation */
	Ogre::Real duration = 10;
	Ogre::Real num_steps = 36;
	Ogre::Real step = duration/num_steps;
	Ogre::Animation* animation = scene_manager->createAnimation("Animation", duration);
	animation->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	//Ogre::Node *object_scene_node = root_scene_node->getChild(0);
	Ogre::Node *object_scene_node = root_scene_node->getChild(entity_name);
	Ogre::NodeAnimationTrack* track = animation->createNodeTrack(0, object_scene_node);

	/* Set up frames for animation */
	Ogre::TransformKeyFrame* key;
	Ogre::Quaternion quat;
	for (int i = 0; i < num_steps; i++){
		Ogre::Real current_time = ((float) i) * step;
		key = track->createNodeKeyFrame(current_time);
		Ogre::Real current_rotation = ((float) i)/num_steps * Ogre::Math::TWO_PI;
		quat.FromAngleAxis(Ogre::Radian(-current_rotation), Ogre::Vector3(0, 1, 0));
		key->setRotation(quat);
	}

	/* Create animation state */
	animation_state_ = scene_manager->createAnimationState("Animation");
	animation_state_->setEnabled(true);
	animation_state_->setLoop(true);

	/* Turn on animating flag */
	animating_ = true;
}


bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent& fe){
  
	/* This event is called after a frame is queued for rendering */
	/* Do stuff in this event since the GPU is rendering and the CPU is idle */

	/* Keep animating if flag is on */
	if (animating_){
		animation_state_->addTime(fe.timeSinceLastFrame);
	}

	/* Update shader variables: timer */
	if (animating_particles_){
		timer_ += fe.timeSinceLastFrame;
		Ogre::MaterialPtr mat1 = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("ParticleMaterial1"));
		mat1->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_);

		Ogre::MaterialPtr mat2 = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("ParticleMaterial2"));
		mat2->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_+0.5f);

		Ogre::MaterialPtr mat3 = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("ParticleMaterial3"));
		mat3->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_+2.0f);

		Ogre::MaterialPtr mat4 = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("ParticleMaterial4"));
		mat4->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_+3.5f);

		Ogre::MaterialPtr mat5 = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("ParticleMaterial5"));
		mat5->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_+1.5f);

		Ogre::MaterialPtr mat6 = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("ParticleMaterial6"));
		mat6->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_+2.5f);

		Ogre::MaterialPtr mat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(particle_material_name_));
		mat->getBestTechnique()->getPass(0)->getVertexProgramParameters()->setNamedConstant("timer", timer_+1.2f);
	}

	/* Capture input */
	keyboard_->capture();
	mouse_->capture();

	/* Handle specific key events */
	if (keyboard_->isKeyDown(OIS::KC_SPACE)){
		space_down_ = true;
	}
	if ((!keyboard_->isKeyDown(OIS::KC_SPACE)) && space_down_){
		animating_ = !animating_;
		animating_particles_ = !animating_particles_;
		space_down_ = false;
	}
	if (keyboard_->isKeyDown(OIS::KC_RETURN)){
		animation_state_->setTimePosition(0);
		timer_ = 0.0;
	}
	if (keyboard_->isKeyDown(OIS::KC_ESCAPE)){
		ogre_root_->shutdown();
		ogre_window_->destroy();
		return false;
	}
 
    return true;
}


void OgreApplication::windowResized(Ogre::RenderWindow* rw){

	/* Update the window and aspect ratio when the window is resized */
	int width = rw->getWidth(); 
    int height = rw->getHeight();
      
    Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
    Ogre::Camera* camera = scene_manager->getCamera("MyCamera");

	if (camera != NULL){
		camera->setAspectRatio((double)width/height);
    }

	const OIS::MouseState &ms = mouse_->getMouseState();
    ms.width = width;
    ms.height = height;

	ogre_window_->resize(width, height);
	ogre_window_->windowMovedOrResized();
	ogre_window_->update();
}


} // namespace ogre_application;