#pragma once

#include "CopalForceSourceComponent.h"

namespace Copal
{
	class SimpleOscillatorCopalComponent
		: public Copal::CopalForceSourceComponent
	{
	public:
		~SimpleOscillatorCopalComponent() override = default;
		AZ_COMPONENT(SimpleOscillatorCopalComponent, "{578F1006-BB61-4795-A7D6-09467964CAA2}");

		static void Reflect(AZ::ReflectContext* reflection);

		void OnPostPhysicsUpdate() override;

		AZ::EntityId GetAttachedEntity();
		void SetAttachedEntity(AZ::EntityId);

		float GetPeriod();
		void SetPeriod(float);

	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;

		void OnAttachedEntityChanged();

		AZ::EntityId m_AttachedEntity , m_PreviousAttachedEntity;
		float Period = 1.f;

	};
}

