#include <iostream>
#include <exception>
#include "ogre_application.h"

/* Macro for printing exceptions */
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

/* Main function that builds and runs the application */
int main(void){
    ogre_application::OgreApplication application;

	try {
		application.Init();
		application.CreateParticleGeometry("Particles", 3000);

		application.CreateParticleEntity("ParticleEnt1", "Particles", "ParticleMaterial1");
		application.CreateParticleEntity("ParticleEnt2", "Particles", "ParticleMaterial2");
		application.CreateParticleEntity("ParticleEnt3", "Particles", "ParticleMaterial3");
		application.CreateParticleEntity("ParticleEnt4", "Particles", "ParticleMaterial4");
		application.CreateParticleEntity("ParticleEnt5", "Particles", "ParticleMaterial5");
		application.CreateParticleEntity("ParticleEnt6", "Particles", "ParticleMaterial6");

		application.CreateParticleEntity("ParticleEnt", "Particles", "ParticleMaterial");
		//application.SetupAnimation("ParticleEnt");
		application.MainLoop();
	}
	catch (std::exception &e){
		PrintException(e);
	}

    return 0;
}
