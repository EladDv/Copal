#pragma once
#include "ForceBaseComponent.h"
#include <AzCore\Component\TransformBus.h>
namespace Copal
{
	class HarmonicOscillatorComponent
		: public Copal::ForceBaseComponent
	{
	public:
		AZ_COMPONENT(HarmonicOscillatorComponent, "{84588228-119D-47E2-B44F-933B05FF0707}");

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		void OnPostPhysicsUpdate() override;

		bool GetForceEnabled() { return ForceEnabled; }
		void SetForceEnabled(bool e) { ForceEnabled = e; }

		float GetPeriod() { return Period; }
		float SetPeriod(float f) { Period = f; }
		AZ::EntityId GetAttachedEntity() { return AttachedEntity; }
		void SetAttachedEntity(AZ::EntityId e)
		{
			AttachedEntity = e;
			AttachedHandler = Copal::CopalPhysicsRequestsBus::FindFirstHandler(e);
		}


	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		AZ::EntityId AttachedEntity;
		Copal::CopalPhysicsRequests* AttachedHandler;

		bool ForceEnabled = false;
		float Period = 0;

	};
}

