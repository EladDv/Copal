#pragma once

#include "CopalForceSourceComponent.h"
#include <LmbrCentral\Scripting\TriggerAreaComponentBus.h>

namespace Copal
{
	class LocalGravityCopalComponent
		: public Copal::CopalForceSourceComponent
		, public LmbrCentral::TriggerAreaNotificationBus::Handler
	{
	public:
		~LocalGravityCopalComponent() override = default;
		AZ_COMPONENT(LocalGravityCopalComponent, "{66EC261A-4B8C-49BF-A098-5849534E5841}");

		static void Reflect(AZ::ReflectContext* reflection);

		void Activate() override;
		void Deactivate() override;

		void OnPostPhysicsUpdate() override;

		void OnTriggerAreaEntered(AZ::EntityId) override;
		void OnTriggerAreaExited(AZ::EntityId) override;

		bool GetGravityEnabled() { return EnableGravity; }
		void SetGravityEnabled(bool b) { EnableGravity = b; }

		AZ::Vector3 GetGravitationalAcceleration() { return GravitationalAcceleration; }
		void SetGravitationalAcceleration(AZ::Vector3 v) { GravitationalAcceleration = v; }

		AZStd::vector<AZ::EntityId> GetAffectedEntities() { return AffectedEntities; }

	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		AZStd::vector<AZ::EntityId> AffectedEntities;

		bool EnableGravity;
		AZ::Vector3 GravitationalAcceleration;

	};
}

