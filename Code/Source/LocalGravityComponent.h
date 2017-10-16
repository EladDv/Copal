#pragma once
#include "StdAfx.h"
#include <LmbrCentral\Scripting\TriggerAreaComponentBus.h>
#include "ForceBaseComponent.h"
namespace Copal
{
	class LocalGravityComponent
		: public Copal::ForceBaseComponent
		, public LmbrCentral::TriggerAreaNotificationBus::Handler
	{
	public:
		AZ_COMPONENT(LocalGravityComponent, "{5A94B8BC-D7B8-4392-BA07-F52338AB18FE}");

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType & required)
		{
			required.push_back(AZ_CRC("ProximityTriggerService"));
			required.push_back(AZ_CRC("ShapeService"));
		}

		void Activate() override;
		void Deactivate() override;

		void OnPostPhysicsUpdate() override;

		void OnTriggerAreaEntered(AZ::EntityId) override;
		void OnTriggerAreaExited(AZ::EntityId) override;

		bool GetGravityEnabled() { return GravityEnabled; }
		void SetGravityEnabled(bool enabled) { GravityEnabled = enabled; ForceUpdated = false; }


		AZ::Vector3 GetGravitationalAcceleration() { return GravitationalAcceleration; }
		void SetGravitationalAcceleration(AZ::Vector3 fieldVector) { GravitationalAcceleration = fieldVector; ForceUpdated = false; }

	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		AZStd::vector<Copal::CopalPhysicsRequests*> AffectedEntityChannels;

		bool GravityEnabled = false;
		bool ForceUpdated = false; // Save CPU cycles on already updated forces

		AZ::Vector3 GravitationalAcceleration = AZ::Vector3(0, 0, 0);

	};
}
