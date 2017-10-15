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
	class ForceBaseComponent
		: public AZ::Component
		, public LmbrCentral::PhysicsSystemEventBus::Handler
	{
		public:
			AZ_COMPONENT(ForceBaseComponent, "{4632648A-CD4F-4BC9-A9F3-33AB40E7F19B}");

			// AZ::Component interface implementation.
			void Init() override {}
			void Activate() override;
			void Deactivate() override;
			
			// Required Reflect function.
			static void Reflect(AZ::ReflectContext* context);
			// Optional functions for defining provided and dependent services.
			static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

			virtual void OnPostPhysicsUpdate() {}

			const AZStd::string& GetForceName() const { return ForceName; }
			void SetForceName(AZStd::string& name) { ForceName = name; }

			const AZStd::string& GetForceTag() const { return ForceTag; }
			void SetForceTag(AZStd::string& tag) { ForceTag = tag; }


	protected:
			AZStd::string ForceName;
			AZStd::string ForceTag;

	};
}

