#include "StdAfx.h"
#include "InertialFrameComponent.h"

namespace Copal
{
	void InertialFrameComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<InertialFrameComponent>()
				->Version(1)
				->Field("AttachedEntities", &InertialFrameComponent::AttachedEntities)
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<InertialFrameComponent>("Inertial Frame Component", "This component tracks and manages all objects attached to an inertial frame .")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Inertial Frames")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &InertialFrameComponent::AttachedEntities, "Attached Entities", "A vector of entities to be considered as in the inertial frame.")
					;
			}
			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<InertialFrameComponent>()
					->Attribute(AZ::Script::Attributes::Category, "Physics")
					->Method("AttachNewEntity", &InertialFrameComponent::AttachNewEntity)
					->Method("DetachEntity", &InertialFrameComponent::DetachEntity)
					->Property("AttachedEntities", BehaviorValueGetter(&InertialFrameComponent::AttachedEntities), nullptr)
					;
			}
		}
	}

	void InertialFrameComponent::Activate()
	{
		for each (AZ::EntityId entityId in AttachedEntities)
		{
			AZ::TransformBus::Event(entityId, &AZ::TransformBus::Events::SetParent, GetEntityId());
		}
	}
	void InertialFrameComponent::Deactivate()
	{
		for each (AZ::EntityId entityId in AttachedEntities)
		{
			AZ::TransformBus::Event(entityId, &AZ::TransformBus::Events::SetParent, AZ::EntityId());
		}
		AttachedEntities.clear();
	}
	void InertialFrameComponent::AttachNewEntity(AZ::EntityId entityId)
	{
		AZ::TransformBus::Event(entityId, &AZ::TransformBus::Events::SetParent, GetEntityId());
		AttachedEntities.push_back(entityId);
	}
	void InertialFrameComponent::DetachEntity(AZ::EntityId entityId)
	{
		for(AZStd::list<AZ::EntityId>::iterator found = AttachedEntities.begin() ; found != AttachedEntities.end(); ++found)
		{ 
			if (*found == entityId)
				AttachedEntities.erase(found); // Not returning here just in case someone put it in twice. at worst O(n) anyways.
		}
		AZ::TransformBus::Event(entityId, &AZ::TransformBus::Events::SetParent, AZ::EntityId());
	}

}
