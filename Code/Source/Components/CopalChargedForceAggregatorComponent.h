# pragma once

#include "CopalForceAggregatorComponent.h"

namespace Copal
{
	class CopalChargedForceAggregatorComponent
		: public CopalForceAggregatorComponent
	{
	public:
		~CopalChargedForceAggregatorComponent() override = default;
		AZ_COMPONENT(CopalChargedForceAggregatorComponent, "{A509DFE8-8C29-427F-8085-8F1695D5A651}");

		static void Reflect(AZ::ReflectContext* reflection);
		
		void EnableGlobalGravity(AZ::EntityId) override {};
		void DisableGlobalGravity(AZ::EntityId) override {};

		virtual bool GetGlobalGravityEnabled() { return false; } // IMPORTANT! need to override this in order to get no global gravity 

		void OnPrePhysicsUpdate() override;
		enum class ChargeOrChargeToMass : uint32_t
		{
			Charge,
			ChargeToMass,
		};

		bool UsingCharge() const
		{
			return charge_or_chargeToMass == ChargeOrChargeToMass::Charge;
		}

		bool UsingChargeToMass() const
		{
			return charge_or_chargeToMass == ChargeOrChargeToMass::ChargeToMass;
		}

		float GetCharge();
		float GetChargeToMass();

		void SetCharge(float);
		void SetChargeToMass(float);

		void SetUsingCharge(bool);
		void SetUsingChargeToMass(bool);

		bool m_UseGlobalGravity = false;
		AZ::Vector3 m_GlobalGravity = AZ::Vector3(0, 0, 0);

		AZStd::list<AZStd::string> handledTags;

		ChargeOrChargeToMass charge_or_chargeToMass;
		float m_charge;
		float m_chargeToMass;
		
	};
}