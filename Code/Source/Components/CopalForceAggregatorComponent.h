# pragma once

#include "CopalForceSourceComponent.h"

namespace Copal
{
	class CopalForceAggregatorComponent
		: public CopalForceSourceBus::Handler
	{
	public:
		~CopalForceAggregatorComponent() override = default;
		AZ_COMPONENT(CopalForceAggregatorComponent, "{A4DD66BF-6FEF-46F6-9629-7E0010BA1B4C}");

		static void Reflect(AZ::ReflectContext* reflection);

		void Init() override {};
		void Activate() override;
		void Deactivate() override;

		void OnPrePhysicsUpdate() override;
		void OnPostPhysicsUpdate() override;

		CopalForceSourceComponent::Force* GetForce(AZ::EntityId, AZStd::string) override;
		CopalForceSourceComponent::Force* GetTorque(AZ::EntityId, AZStd::string) override;

		AZ::Vector3 GetSumOfForcesAtCenter(AZ::EntityId) override;
		AZ::Vector3 GetSumOfTorquesAtCenter(AZ::EntityId) override;

		AZ::EntityId GetEntityIdEvent() override { return GetEntityId(); }

		virtual void EnableGlobalGravity(AZ::EntityId) override { m_UseGlobalGravity = true; }
		virtual void DisableGlobalGravity(AZ::EntityId) override { m_UseGlobalGravity = false; }

		virtual void SetGlobalGravity(AZ::EntityId, AZ::Vector3) override;
		virtual  AZ::Vector3 GetGlobalGravity(AZ::EntityId) override;

		virtual bool GetGlobalGravityEnabled() { return m_UseGlobalGravity; }

		void AddForce(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string) override;
		void AddTorque(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string) override;


		void RemoveForce(AZ::EntityId, AZStd::string) override;
		void RemoveTorque(AZ::EntityId, AZStd::string) override;

		void UpdateForce(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string) override;
		void UpdateTorque(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string) override;

		void CopalForceAggregatorComponent::ApplyForces(float);

		std::map<AZStd::string, CopalForceSourceComponent::Force> m_Forces;

		AZ::Vector3 m_ForceSum;
		AZ::Vector3 m_TorqueSum;

		bool m_UseGlobalGravity;
		bool m_AddedGlobalGravity;
		AZ::Vector3 m_GlobalGravity = AZ::Vector3(0,0,-9.8f);

		float lastTime = 0.f;
		float currentTime = 0.f;
		float TotalTime = 0.f;

		AZStd::list<AZStd::string> handledTags;

		AZ::Vector3 LastV,AVC;
	};
}