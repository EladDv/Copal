#pragma once
#include "StdAfx.h"
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/Component.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <AzCore/Component/EntityBus.h>
#include <AzCore/std/smart_ptr/shared_ptr.h>
#include <LmbrCentral/Physics/PhysicsComponentBus.h>
#include <LmbrCentral/Physics/PhysicsSystemComponentBus.h>

#include <MathConversion.h>
#include <ISystem.h>
#include <ITimer.h>
#include <physinterface.h>

#include <Copal/CopalBus.h>

namespace Copal
{
	class AggregatorBaseComponent
		: public AZ::Component
		, public LmbrCentral::PhysicsSystemEventBus::Handler
		, public Copal::CopalPhysicsRequestsBus::Handler
	{
		public:
			AZ_COMPONENT(AggregatorBaseComponent, "{D8B1826F-A848-412E-92F1-34F88E5F30CF}");
			// AZ::Component interface implementation.
			void Init() override {}
			void Activate() override;
			void Deactivate() override;

			// Required Reflect function.
			static void Reflect(AZ::ReflectContext* context);
			// Optional functions for defining provided and dependent services.
			static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
			static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
			static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

			void OnPrePhysicsUpdate() override;
			void OnPostPhysicsUpdate() override;

			void DeactivateDrag();

			void CalculateForces(float);
			void ApplyOffCenterImpulsesOnMap(CopalForceMap&, float, bool);
			void AddForce(const AZStd::string, const Force&) override;
			void RemoveForce(const AZStd::string) override;

			Force* GetForce(const AZStd::string) override;
			void GetAllForces(CopalForceMap* outParam) override { outParam = &ForcesMap; }

			void AddTorque(const AZStd::string, const Force&) override;
			void RemoveTorque(const AZStd::string) override;

			Force* GetTorqe(const AZStd::string) override;
			void GetAllTorques(CopalForceMap* outParam) override { outParam = &TorquesMap; }

			void GetComponentEntityId(AZ::EntityId id) override { id = GetEntityId(); }

			AZStd::list < AZStd::string >* GetHandledTags() { return &HandledTags; }
			void GetHandledTags(AZStd::list < AZStd::string >* TagList) { HandledTags = *TagList; }

			void AddTagToHandled(AZStd::string tag) { HandledTags.push_back(tag); }
			void RemoveTagFromHandled(AZStd::string tag) { HandledTags.remove(tag); }
		protected:
			CopalForceMap ForcesMap;
			CopalForceMap TorquesMap;

			AZStd::list < AZStd::string > HandledTags;

			float lastTime = 0.f;
			float currentTime = 0.f;

			AZ::Vector3 ForceSum = AZ::Vector3(0, 0, 0);
			AZ::Vector3 TorqueSum = AZ::Vector3(0, 0, 0);
	};
}