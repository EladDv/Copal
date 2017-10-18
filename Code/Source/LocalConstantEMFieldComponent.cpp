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
				->Field("EFEnabled", &LocalConstantEMFieldComponent::EFEnabled)
				->Field("EFVector", &LocalConstantEMFieldComponent::EFVector)
				->Field("MFEnabled", &LocalConstantEMFieldComponent::MFEnabled)
				->Field("MFVector", &LocalConstantEMFieldComponent::MFVector)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<LocalConstantEMFieldComponent>("Constant EM Field Component", "Adds a simple constant force in specified vector")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &LocalConstantEMFieldComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &LocalConstantEMFieldComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &LocalConstantEMFieldComponent::EFEnabled, "Electric Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &LocalConstantEMFieldComponent::EFVector, "Electric Field Vector", "The Force Vector applied to the entity.")
					->Attribute(AZ::Edit::Attributes::Visibility, &LocalConstantEMFieldComponent::EFEnabled)
					->Attribute(AZ::Edit::Attributes::ChangeNotify, &LocalConstantEMFieldComponent::Print)
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &LocalConstantEMFieldComponent::MFEnabled, "Magnetic Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &LocalConstantEMFieldComponent::MFVector, "Magnetic Field Vector", "The Force Vector applied to the entity.")
					->Attribute(AZ::Edit::Attributes::Visibility, &LocalConstantEMFieldComponent::MFEnabled)
					->Attribute(AZ::Edit::Attributes::ChangeNotify, &LocalConstantEMFieldComponent::Print)
					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<LocalConstantEMFieldComponent>()
					->Method("GetForceName", &LocalConstantEMFieldComponent::GetForceName)
					->Method("SetForceName", &LocalConstantEMFieldComponent::SetForceName)
					->Method("GetForceTag", &LocalConstantEMFieldComponent::GetForceTag)
					->Method("SetForceTag", &LocalConstantEMFieldComponent::SetForceTag)
					;
			}
		}
	}

	void LocalConstantEMFieldComponent::Activate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusConnect(GetEntityId());
		Print();
		Copal::ForceBaseComponent::Activate();
	}

	void LocalConstantEMFieldComponent::Print()
	{
		CryLogAlways("E Vector: %f %f %f %s", float(EFVector.GetX()), float(EFVector.GetY()), float(EFVector.GetZ()), ForceName);
		CryLogAlways("M Vector: %f %f %f %s", float(MFVector.GetX()), float(MFVector.GetY()), float(MFVector.GetZ()), ForceName);
	}
	void LocalConstantEMFieldComponent::PrintV(AZ::Vector3 v)
	{
		CryLogAlways("Vector: %f %f %f", float(v.GetX()), float(v.GetY()), float(v.GetZ()) );
	}
	void LocalConstantEMFieldComponent::Deactivate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusDisconnect(GetEntityId());
		__super::Deactivate();
	}

	void LocalConstantEMFieldComponent::OnTriggerAreaEntered(AZ::EntityId e)
	{
		AZ::Entity* CurrentEntity = nullptr;
		AZ::ComponentApplicationBus::BroadcastResult(CurrentEntity, &AZ::ComponentApplicationRequests::FindEntity, e);

		if (!CurrentEntity)
			return;
		Copal::EMAggregatorComponent* EMAggregator = CurrentEntity->FindComponent <Copal::EMAggregatorComponent>();
		if (EMAggregator)
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

			if (!MFEnabled && !EFEnabled)
				CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);
			else
			{
				EMForce.tag = ForceTag;
				AZ::Entity* CurrentEntity = nullptr;
				EBUS_EVENT_RESULT(CurrentEntity, AZ::ComponentApplicationBus, FindEntity, EntityId);
				
				Copal::EMAggregatorComponent* EntityEMAggregator = CurrentEntity->FindComponent <Copal::EMAggregatorComponent>();
				if (!EntityEMAggregator)
					continue;

				AZ::Vector3 tempV = AZ::Vector3(EntityEMAggregator->GetCharge(), EntityEMAggregator->GetCharge(), EntityEMAggregator->GetCharge());

				if (EFEnabled)
				{
					EMForce.strengthVector = EMForce.strengthVector + (EFVector * tempV);
				}
				if (MFEnabled)
				{
					pe_status_dynamics physicsStatus;
					LmbrCentral::CryPhysicsComponentRequestBus::Event(EntityId, &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);

					EMForce.strengthVector = EMForce.strengthVector + ((LYVec3ToAZVec3(physicsStatus.v).Cross(MFVector))*tempV);
				}
				//PrintV(EMForce.strengthVector);
				CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::AddForce, ForceName,EMForce);
			}
			
		}
	}

}