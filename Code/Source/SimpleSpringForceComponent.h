#pragma once
#include "ForceBaseComponent.h"
#include <AzCore\Component\TransformBus.h>
namespace Copal
{
	class SimpleSpringForceComponent
		: public Copal::ForceBaseComponent
	{
	public:
		AZ_COMPONENT(SimpleSpringForceComponent, "{BEA11C0A-B717-4EA0-8F5A-57F4016FAAE9}");

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		void OnPostPhysicsUpdate() override;

		bool GetForceEnabled() { return ForceEnabled; }
		void SetForceEnabled(bool e) { ForceEnabled = e; }

		float GetK() { return K; }
		void SetK(float newK) { K = newK; }

		float GetRestLength() { return RestLength; }
		void SetRestLength(float l) { RestLength = l; }
		AZ::EntityId GetAttachedEntity() { return AttachedEntity; }


	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		AZ::EntityId AttachedEntity;

		bool ForceEnabled = false;
		float K = 0;
		float RestLength= 0;

	};
}

