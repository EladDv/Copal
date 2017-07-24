
#include "StdAfx.h"

#include "LocalGravityCopalComponent.h"

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Copal
{
	void LocalGravityCopalComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<LocalGravityCopalComponent>()
				->Version(1)
				->Field("Force Name", &LocalGravityCopalComponent::ForceName)
				->Field("Force Tag", &LocalGravityCopalComponent::ForceTag)
				->Field("Enable Gravity", &LocalGravityCopalComponent::EnableGravity)
				->Field("Gravitational Acceleration", &LocalGravityCopalComponent::GravitationalAcceleration)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<LocalGravityCopalComponent>("Local Gravity Component", "Adds a constant acceleration to all entities within it's trigger area")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/Decal.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/Decal.png")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &LocalGravityCopalComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(0, &LocalGravityCopalComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(0, &LocalGravityCopalComponent::EnableGravity, "Enable Gravity", "Is the gravity applied.")
					->DataElement(0, &LocalGravityCopalComponent::GravitationalAcceleration, "Gravitational Acceleration", "The acceleration of the local gravity.")
						->Attribute(AZ::Edit::Attributes::Visibility, &LocalGravityCopalComponent::EnableGravity)

					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->EBus<CopalForceSourceBus>("CopalForceSourceBus")
					->Event("GetForce", &CopalForceSourceBus::Events::GetForce)
					->Event("AddForce", &CopalForceSourceBus::Events::AddForce)
					->Event("RemoveForce", &CopalForceSourceBus::Events::RemoveForce)
					->Event("UpdateForce", &CopalForceSourceBus::Events::UpdateForce)
					//-----------------------------------------------------------------------------------
					->Event("GetTorque", &CopalForceSourceBus::Events::GetTorque)
					->Event("AddTorque", &CopalForceSourceBus::Events::AddTorque)
					->Event("RemoveTorque", &CopalForceSourceBus::Events::RemoveTorque)
					->Event("UpdateTorque", &CopalForceSourceBus::Events::UpdateTorque)
					//-----------------------------------------------------------------------------------
					->Event("EnableGlobalGravity", &CopalForceSourceBus::Events::EnableGlobalGravity)
					->Event("DisableGlobalGravity", &CopalForceSourceBus::Events::DisableGlobalGravity)
					->Event("GetGlobalGravity", &CopalForceSourceBus::Events::GetGlobalGravity)
					->Event("SetGlobalGravity", &CopalForceSourceBus::Events::SetGlobalGravity)
					;
				behaviorContext->Class<LocalGravityCopalComponent>()
					->Method("GetForceName", &LocalGravityCopalComponent::GetForceName)
					->Method("SetForceNameAZString", &LocalGravityCopalComponent::SetForceNameAZString)
					->Method("GetAffectedEntities", &LocalGravityCopalComponent::GetAffectedEntities)
					->Method("GetGravityEnabled", &LocalGravityCopalComponent::GetGravityEnabled)
					->Method("GetGravityEnabled", &LocalGravityCopalComponent::SetGravityEnabled)
					->Method("GetGravitationalAcceleration", &LocalGravityCopalComponent::GetGravitationalAcceleration)
					->Method("SetGravitationalAcceleration", &LocalGravityCopalComponent::SetGravitationalAcceleration)
					->Method("GetTag", &LocalGravityCopalComponent::GetTag)
					->Method("SetTag", &LocalGravityCopalComponent::SetTag)
					;
			}
		}
	}

	void LocalGravityCopalComponent::Activate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusConnect(GetEntityId());
		CopalForceSourceComponent::Activate();
	}

	void LocalGravityCopalComponent::Deactivate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusDisconnect(GetEntityId());
		CopalForceSourceComponent::Deactivate();
	}

	void LocalGravityCopalComponent::OnPostPhysicsUpdate()
	{
		for (auto &Entity : AffectedEntities)
		{
			AZ::Transform m_AttachedEntityTransform = AZ::Transform::CreateIdentity(); // Necessary for GetWorldTM to return a meaninful and correct Transform

			pe_status_dynamics physicsStatus;
			EBUS_EVENT_ID(Entity, LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);
			if (EnableGravity)
			{
				AZ::Vector3 ForceToApply = GravitationalAcceleration * physicsStatus.mass; // F = m * a
				CopalForceSourceComponent::Force MyForce;
				MyForce.strengthVector = ForceToApply;
				MyForce.tag = ForceTag;
				AddForce(Entity, MyForce, ForceName);
			}
		}
	}

	void LocalGravityCopalComponent::OnTriggerAreaEntered(AZ::EntityId e)
	{
		AffectedEntities.push_back(e);
	}

	void LocalGravityCopalComponent::OnTriggerAreaExited(AZ::EntityId e)
	{
		AZStd::vector<AZ::EntityId>::iterator position = AZStd::find(AffectedEntities.begin(), AffectedEntities.end(), e);
		if (position != AffectedEntities.end()) // Just in case, It dosnt sound possible that an object will exit an area it hasnt entered.
			AffectedEntities.erase(position);
	}
	
}