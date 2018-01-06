
#pragma once

#include <LmbrCentral/Physics/PhysicsComponentBus.h>
#include <LmbrCentral/Physics/PhysicsSystemComponentBus.h>
#include <LmbrCentral/Physics/CryPhysicsComponentRequestBus.h>

namespace Copal
{
	/*--------------------------------------------------------------------------------------------+
	| Uncomment below according to which Lumberyard version you are using.The default is for LY12 |
	+--------------------------------------------------------------------------------------------*/
	#define LY_VERSION_12
  //#define LY_VERSION_11

	/*--------------------------------------------------------------------------------------------+
	| PhysicsComponentBus is deprecated in LY12, so we have to use CryPhysics instead             |
	+--------------------------------------------------------------------------------------------*/

	#ifdef LY_VERSION_12
		typedef LmbrCentral::CryPhysicsComponentRequestBus CopalCryPhysicsComponentRequestBus;
	#endif
	
	#ifdef LY_VERSION_11
		typedef LmbrCentral::PhysicsComponentRequestBus CopalCryPhysicsComponentRequestBus;
	#endif 

}
