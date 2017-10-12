# pragma once

#include "StdAfx.h"

// Component utilization
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/Component.h>
#include <AzCore\Component\ComponentBus.h>

// Other buses used
#include <AzCore\Component\TransformBus.h>
#include <AzCore\Component\EntityBus.h>
#include <AzCore\Math\Transform.h>
#include <AzCore\Component\TickBus.h>
#include <LmbrCentral\Physics\PhysicsSystemComponentBus.h>

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <LmbrCentral\Physics\PhysicsComponentBus.h>
#include <MathConversion.h>
#include <ISystem.h>
#include <ITimer.h>
#include <physinterface.h>

namespace Copal
{
	class CopalForceSourceComponent
		: public AZ::Component
		, public LmbrCentral::PhysicsSystemEventBus::Handler

	{
	public:
		struct Force
		{
			AZ_TYPE_INFO(Force, "{91D92F7C-27E8-4CA0-BD09-3CE5093BDF85}");

			Force() { strengthVector = AZ::Vector3::CreateZero(); positionVector = AZ::Vector3::CreateZero(); isTorque = false; isOffCenter = false; MARK_UNUSED tag; }

			AZ::Vector3 strengthVector;
			AZ::Vector3 positionVector;

			bool isTorque = false;
			bool isOffCenter = false;

			AZStd::string tag;

		};
		~CopalForceSourceComponent() override = default;
		AZ_COMPONENT(CopalForceSourceComponent, "{219CB3AF-CCE5-4484-8DB3-CD739E8F415E}");

		static void Reflect(AZ::ReflectContext* reflection);

		void Init() override {};
		void Activate() override;
		void Deactivate() override;

		void OnPrePhysicsUpdate() override;
		void OnPostPhysicsUpdate() override;
		
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
		using BusIdType = AZ::EntityId;

		void PrintVector3(string, AZ::Vector3);
		void PrintVector3(string, Vec3);

		AZStd::string GetForceName() { return ForceName; }
		string GetForceNameStringChar() { return ForceName.c_str(); }
		
		void SetForceNameAZString(AZStd::string);
		void SetForceNameString(string);

		AZ::EBusAggregateResults<AZ::EntityId> GetAllEntityIds();

		virtual AZ::EntityId GetEntityIdEvent();

		virtual CopalForceSourceComponent::Force* GetForce(AZ::EntityId, AZStd::string);
		virtual CopalForceSourceComponent::Force* GetTorque(AZ::EntityId, AZStd::string);

		virtual AZ::Vector3 GetSumOfForcesAtCenter(AZ::EntityId);
		virtual AZ::Vector3 GetSumOfTorquesAtCenter(AZ::EntityId);

		virtual void EnableGlobalGravity(AZ::EntityId);
		virtual void DisableGlobalGravity(AZ::EntityId);

		virtual void SetGlobalGravity(AZ::EntityId, AZ::Vector3);
		virtual AZ::Vector3 GetGlobalGravity(AZ::EntityId);

		virtual void AddForce(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string);
		virtual void AddTorque(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string);


		virtual void RemoveForce(AZ::EntityId, AZStd::string);
		virtual void RemoveTorque(AZ::EntityId, AZStd::string);

		virtual void UpdateForce(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string);
		virtual void UpdateTorque(AZ::EntityId, CopalForceSourceComponent::Force, AZStd::string);

		void SetTag(AZStd::string tag) { ForceTag = tag; }
		AZStd::string GetTag() { return ForceTag; }


	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag;


	};
	using CopalForceSourceBus = AZ::EBus<CopalForceSourceComponent> ;

}