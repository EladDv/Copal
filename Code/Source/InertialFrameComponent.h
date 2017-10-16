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
#include <AzCore\Component\TransformBus.h>

#include <MathConversion.h>
#include <ISystem.h>
#include <ITimer.h>
#include <physinterface.h>

#include <Copal/CopalBus.h>

namespace Copal
{
	class InertialFrameComponent
		: public AZ::Component
	{
	public:
		AZ_COMPONENT(InertialFrameComponent, "{893CC06A-685C-4C63-B179-27CA3A869A41}");

		// AZ::Component interface implementation.
		void Init() override {}
		void Activate() override;
		void Deactivate() override;

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);
		// Optional functions for defining provided and dependent services.
		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
		{
			provided.push_back(AZ_CRC("InertialFrameService"));
		}

		void AttachNewEntity(AZ::EntityId);
		void DetachEntity(AZ::EntityId);

		AZStd::list<AZ::EntityId> GetAttacehdEntities() { return AttachedEntities; }

	protected:
		
		AZStd::list<AZ::EntityId> AttachedEntities;
		
	};
}