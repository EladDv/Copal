#pragma once
#include "StdAfx.h"
#include "ForceBaseComponent.h"
#include <AzCore\Component\TransformBus.h>
namespace Copal
{
	class GlobalRadialGravityComponent
		: public Copal::ForceBaseComponent
	{
	public:
		AZ_COMPONENT(GlobalRadialGravityComponent, "{4A8E93A4-F752-4234-9AF0-5EBB0985E116}");

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		void OnPostPhysicsUpdate() override;

		bool GetForceEnabled() { return ForceEnabled; }
		void SetForceEnabled(bool e) { ForceEnabled = e; }

		float GetSurfaceGravity() { return SurfaceGravity; }
		void SetSurfaceGravity(float f) { SurfaceGravity = f; }


	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		bool ForceEnabled = true;
		float SurfaceGravity;
		float BodyRadius = 0;

		float GravitationalFactor = 0;

	};
}

