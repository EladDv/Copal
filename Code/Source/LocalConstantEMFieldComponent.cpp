#include "StdAfx.h"
#include "LocalConstantEMFieldComponent.h"
#include "EMAggregatorComponent.h"
namespace Copal
{
	void LocalConstantEMFieldComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<LocalConstantEMFieldComponent>()
				->Version(2)
				->Field("ForceName", &LocalConstantEMFieldComponent::ForceName)
				->Field("ForceTag", &LocalConstantEMFieldComponent::ForceTag)
				->Field("AffectedEntities", &LocalConstantEMFieldComponent::AffectedEntities)
				->Field("ElectricForceEnabled", &LocalConstantEMFieldComponent::ElectricForceEnabled)
				->Field("ElectricFieldVector", &LocalConstantEMFieldComponent::ElectricFieldVector)
				->Field("MagneticForceEnabled", &LocalConstantEMFieldComponent::MagneticForceEnabled)
				->Field("MagneticFieldVector", &LocalConstantEMFieldComponent::MagneticFieldVector)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<LocalConstantEMFieldComponent>("Local Constant EM Field Component", "Adds a constant EM field (and applies Lorenz forces) inside the trigger areas of the entity")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &LocalConstantEMFieldComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &LocalConstantEMFieldComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &LocalConstantEMFieldComponent::ElectricForceEnabled, "Electric Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &LocalConstantEMFieldComponent::ElectricFieldVector, "Electric Field Vector", "The Force Vector applied to the entity.")
					->Attribute(AZ::Edit::Attributes::Visibility, &LocalConstantEMFieldComponent::ElectricForceEnabled)
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &LocalConstantEMFieldComponent::MagneticForceEnabled, "Magnetic Force Enabled", "Is the Magnetic force applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &LocalConstantEMFieldComponent::MagneticFieldVector, "Magnetic Field Vector", "The Magnetic Field Vector applied in the area.")
					->Attribute(AZ::Edit::Attributes::Visibility, &LocalConstantEMFieldComponent::MagneticForceEnabled)

					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<LocalConstantEMFieldComponent>()
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					->Property("ElectricForceEnabled", BehaviorValueProperty(&LocalConstantEMFieldComponent::ElectricForceEnabled))
					->Property("ElectricFieldVector", BehaviorValueProperty(&LocalConstantEMFieldComponent::ElectricFieldVector))
					->Property("MagneticForceEnabled", BehaviorValueProperty(&LocalConstantEMFieldComponent::MagneticForceEnabled))
					->Property("MagneticFieldVector", BehaviorValueProperty(&LocalConstantEMFieldComponent::MagneticFieldVector))
					;
			}
		}
	}

	void LocalConstantEMFieldComponent::Activate()
	{
		__super::Activate();
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusConnect(GetEntityId());
	}

	void LocalConstantEMFieldComponent::Deactivate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusDisconnect(GetEntityId());
		__super::Deactivate();
	}

	void LocalConstantEMFieldComponent::OnTriggerAreaEntered(AZ::EntityId e)
	{
		AffectedEntities.push_back(e);
	}

	void LocalConstantEMFieldComponent::OnTriggerAreaExited(AZ::EntityId e)
	{
		AZStd::vector<AZ::EntityId>::iterator position = AZStd::find(AffectedEntities.begin(), AffectedEntities.end(), e);
		if (position != AffectedEntities.end()) // Just in case, It dosnt sound possible that an object will exit an area it hasnt entered.
			AffectedEntities.erase(position);
		CopalPhysicsRequestsBus::Event(e, &CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);

	}

	void LocalConstantEMFieldComponent::OnPostPhysicsUpdate()
	{
		Force EMForce;
		for (auto &EntityId : AffectedEntities) // Check through all entities in 
		{
			AZ::Entity* CurrentEntity = nullptr;
			AZ::ComponentApplicationBus::BroadcastResult(CurrentEntity, &AZ::ComponentApplicationRequests::FindEntity, EntityId);
			Copal::EMAggregatorComponent *EntityEMAggregator = CurrentEntity->FindComponent<Copal::EMAggregatorComponent>();
			if (EntityEMAggregator != nullptr)
			{
				if (!MagneticForceEnabled && !ElectricForceEnabled)
					CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);

				else
				{
					EMForce.tag = ForceTag;
					if (ElectricForceEnabled && ElectricFieldVector != AZ::Vector3(0, 0, 0))
						EMForce.strengthVector += EntityEMAggregator->GetCharge()*ElectricFieldVector;

					if (MagneticForceEnabled && MagneticFieldVector != AZ::Vector3(0, 0, 0))
					{
						pe_status_dynamics physicsStatus;
						LmbrCentral::CryPhysicsComponentRequestBus::Event(GetEntityId(), &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);
						EMForce.strengthVector += EntityEMAggregator->GetCharge()*(LYVec3ToAZVec3(physicsStatus.v).Cross(MagneticFieldVector));
					}
					CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::AddForce, ForceName, EMForce);
				}
			}
		}
	}

}