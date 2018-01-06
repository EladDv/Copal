#include "StdAfx.h"
#include "LocalGravityComponent.h"
#include "AggregatorBaseComponent.h"
namespace Copal
{
	void LocalGravityComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<LocalGravityComponent>()
				->Version(2)
				->Field("ForceName", &LocalGravityComponent::ForceName)
				->Field("ForceTag", &LocalGravityComponent::ForceTag)
				->Field("GravityEnabled", &LocalGravityComponent::GravityEnabled)
				->Field("GravitationalAcceleration", &LocalGravityComponent::GravitationalAcceleration)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<LocalGravityComponent>("Local Gravity Component", "Adds a local gravity field inside the trigger areas of the entity")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &LocalGravityComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &LocalGravityComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &LocalGravityComponent::GravityEnabled, "Gravity Enabled", "Is the acceleration applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &LocalGravityComponent::GravitationalAcceleration, "Gravitational Acceleration", "The acceleration Vector applied to the entity.")
					->Attribute(AZ::Edit::Attributes::Visibility, &LocalGravityComponent::GravityEnabled)
					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<LocalGravityComponent>()
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					->Property("GravityEnabled", BehaviorValueProperty(&LocalGravityComponent::GravityEnabled))
					->Property("GravitationalAcceleration", BehaviorValueProperty(&LocalGravityComponent::GravitationalAcceleration))
					;
			}
		}
	}

	void LocalGravityComponent::Activate()
	{
		__super::Activate();
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusConnect(GetEntityId());
	}

	void LocalGravityComponent::Deactivate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusDisconnect(GetEntityId());
		__super::Deactivate();
	}

	void LocalGravityComponent::OnTriggerAreaEntered(AZ::EntityId e)
	{

		AZ::Entity* CurrentEntity = nullptr;
		AZ::ComponentApplicationBus::BroadcastResult(CurrentEntity, &AZ::ComponentApplicationRequests::FindEntity, e);
		if (!CurrentEntity)
			return;
		AffectedEntities.push_back(e);

		ForceUpdated = false;
	}

	void LocalGravityComponent::OnTriggerAreaExited(AZ::EntityId e)
	{
		AZStd::vector<AZ::EntityId>::iterator position = AZStd::find(AffectedEntities.begin(), AffectedEntities.end(), e);
		if (position != AffectedEntities.end()) // Just in case, It dosnt sound possible that an object will exit an area it hasnt entered.
			AffectedEntities.erase(position);

		CopalPhysicsRequestsBus::Event(e, &CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);
		ForceUpdated = false;
	}

	void LocalGravityComponent::OnPostPhysicsUpdate()
	{
		if (ForceUpdated) return;

		for (auto &EntityId : AffectedEntities) // Check through all entities in 
		{

			if (!GravityEnabled || GravitationalAcceleration == AZ::Vector3(0, 0, 0))
				CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);

			else
			{
				Force GravForce;
				GravForce.tag = ForceTag;
				pe_status_dynamics physicsStatus;
				CopalCryPhysicsComponentRequestBus::Event(EntityId, &CopalCryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);
				GravForce.strengthVector = GravitationalAcceleration * AZ::Vector3(physicsStatus.mass, physicsStatus.mass, physicsStatus.mass);
				CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::AddForce, ForceName, GravForce);
			}

		}
		ForceUpdated = true;
	}
}