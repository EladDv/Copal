#include "StdAfx.h"
#include "ForceBaseComponent.h"

namespace Copal
{
	void ForceBaseComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<ForceBaseComponent>()
				->Version(1)
				->Field("Force Name", &ForceBaseComponent::ForceName)
				->Field("Force Tag", &ForceBaseComponent::ForceTag)
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<ForceBaseComponent>("Force Source Component", "This component is the base for all force senders in this GEM.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &ForceBaseComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(0, &ForceBaseComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique

					;
			}
			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<ForceBaseComponent>()
					->Attribute(AZ::Script::Attributes::Category, "Physics")
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					;
			}
		}
	}
	void ForceBaseComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType & provided)
	{
		provided.push_back(AZ_CRC("ForceSourceService"));
	}

	void ForceBaseComponent::Activate()
	{
		LmbrCentral::PhysicsSystemEventBus::Handler::BusConnect();
	}

	void ForceBaseComponent::Deactivate()
	{
		LmbrCentral::PhysicsSystemEventBus::Handler::BusDisconnect();
		CopalPhysicsRequestsBus::Broadcast(&CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);
		CopalPhysicsRequestsBus::Broadcast(&CopalPhysicsRequestsBus::Events::RemoveTorque, ForceName);
	}

}