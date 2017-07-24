#pragma once

#include "CopalForceSourceComponent.h"

namespace Copal
{
	class SimpleSpringCopalComponent
		: public Copal::CopalForceSourceComponent
	{
	public:
		~SimpleSpringCopalComponent() override = default;
		AZ_COMPONENT(SimpleSpringCopalComponent, "{8FA22065-AA93-4534-9310-C2F5573D4438}");

		static void Reflect(AZ::ReflectContext* reflection);

		void OnPostPhysicsUpdate() override;

		AZ::EntityId GetAttachedEntity();
		void SetAttachedEntity(AZ::EntityId);

		float GetK_value(){ return k_value; }
		void SetK_value(float f) { k_value = f; }

		float GetRestLength(){ return restLength; }
		void SetRestLength(float f) { restLength = f; }


	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		void OnAttachedEntityChanged();

		AZ::EntityId m_AttachedEntity, m_PreviousAttachedEntity;
		
		float k_value;
		float restLength;

	};
}

