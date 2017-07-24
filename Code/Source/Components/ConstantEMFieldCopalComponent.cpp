
#include "StdAfx.h"

#include "ConstantEMFieldCopalComponent.h"
#include "CopalChargedForceAggregatorComponent.h"

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Copal
{
	void ConstantEMFieldCopalComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<ConstantEMFieldCopalComponent>()
				->Version(1)
				->Field("Force Name", &ConstantEMFieldCopalComponent::ForceName)
				->Field("Force Tag", &ConstantEMFieldCopalComponent::ForceTag)
				->Field("Enable Electric Field", &ConstantEMFieldCopalComponent::EnableElectricField)
				->Field("Electric Field", &ConstantEMFieldCopalComponent::ElectricField)
				->Field("Enable Magnetic Field", &ConstantEMFieldCopalComponent::EnableMagneticField)
				->Field("Magnetic Field", &ConstantEMFieldCopalComponent::MagneticField)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<ConstantEMFieldCopalComponent>("Constant Local Electro Magnetic Field Component", "Adds a constant EM field to all entities within it's trigger area")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/Decal.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/Decal.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &ConstantEMFieldCopalComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(0, &ConstantEMFieldCopalComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(0, &ConstantEMFieldCopalComponent::EnableElectricField, "Enable Electric Field", "Is the electric force applied.")
					->DataElement(0, &ConstantEMFieldCopalComponent::ElectricField, "Electric Field", "The strength and direction of the field at each point.")
					->Attribute(AZ::Edit::Attributes::Visibility, &ConstantEMFieldCopalComponent::EnableElectricField)

					->DataElement(0, &ConstantEMFieldCopalComponent::EnableMagneticField, "Enable Magnetic Field", "Is the magnetic force applied.")
					->DataElement(0, &ConstantEMFieldCopalComponent::MagneticField, "Magnetic Field", "The strength and direction of the field at each point.")
					->Attribute(AZ::Edit::Attributes::Visibility, &ConstantEMFieldCopalComponent::EnableMagneticField)
					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->EBus<CopalForceSourceBus>("CopalForceSourceBus")
					->Event("GetForce", &CopalForceSourceBus::Events::GetForce)
					->Event("GetSumOfForces", &CopalForceSourceBus::Events::GetSumOfForcesAtCenter)
					->Event("AddForce", &CopalForceSourceBus::Events::AddForce)
					->Event("RemoveForce", &CopalForceSourceBus::Events::RemoveForce)
					->Event("UpdateForce", &CopalForceSourceBus::Events::UpdateForce)
					//-----------------------------------------------------------------------------------
					->Event("GetTorque", &CopalForceSourceBus::Events::GetTorque)
					->Event("GetSumOfTorques", &CopalForceSourceBus::Events::GetSumOfTorquesAtCenter)
					->Event("AddTorque", &CopalForceSourceBus::Events::AddTorque)
					->Event("RemoveTorque", &CopalForceSourceBus::Events::RemoveTorque)
					->Event("UpdateTorque", &CopalForceSourceBus::Events::UpdateTorque)
					//-----------------------------------------------------------------------------------
					->Event("EnableGlobalGravity", &CopalForceSourceBus::Events::EnableGlobalGravity)
					->Event("DisableGlobalGravity", &CopalForceSourceBus::Events::DisableGlobalGravity)
					->Event("GetGlobalGravity", &CopalForceSourceBus::Events::GetGlobalGravity)
					->Event("SetGlobalGravity", &CopalForceSourceBus::Events::SetGlobalGravity)
					;
				behaviorContext->Class<ConstantEMFieldCopalComponent>()
					->Method("GetForceName", &ConstantEMFieldCopalComponent::GetForceName)
					->Method("SetForceNameAZString", &ConstantEMFieldCopalComponent::SetForceNameAZString)
					->Method("GetAffectedEntities", &ConstantEMFieldCopalComponent::GetAffectedEntities)
					->Method("GetMagneticFieldEnabled", &ConstantEMFieldCopalComponent::GetMagneticFieldEnabled)
					->Method("SetMagneticFieldEnabled", &ConstantEMFieldCopalComponent::SetMagneticFieldEnabled)
					->Method("GetElectricFieldEnabled", &ConstantEMFieldCopalComponent::GetElectricFieldEnabled)
					->Method("SetElectricFieldEnabled", &ConstantEMFieldCopalComponent::SetElectricFieldEnabled)
					->Method("GetElectricField", &ConstantEMFieldCopalComponent::GetElectricField)
					->Method("SetElectricField", &ConstantEMFieldCopalComponent::SetElectricField)
					->Method("GetMagneticField", &ConstantEMFieldCopalComponent::GetMagneticField)
					->Method("SetMagneticField", &ConstantEMFieldCopalComponent::SetMagneticField)
					->Method("GetTag", &CopalForceSourceComponent::GetTag)
					->Method("SetTag", &CopalForceSourceComponent::SetTag)
					;
			}
		}
	}

	void ConstantEMFieldCopalComponent::Activate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusConnect(GetEntityId());
		CopalForceSourceComponent::Activate();
	}

	void ConstantEMFieldCopalComponent::Deactivate()
	{
		LmbrCentral::TriggerAreaNotificationBus::Handler::BusDisconnect(GetEntityId());
		CopalForceSourceComponent::Deactivate();
	}

	void ConstantEMFieldCopalComponent::OnPostPhysicsUpdate()
	{
		for (auto &EntityId : AffectedEntities)
		{
			AZ::Transform m_AttachedEntityTransform = AZ::Transform::CreateIdentity(); // Necessary for GetWorldTM to return a meaninful and correct Transform

			pe_status_dynamics physicsStatus;
			EBUS_EVENT_ID(EntityId, LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);

			// TODO: Test viabillity of an EBus extension via inheriting a ThatEBus::Sender in another class and creating new events.
			AZ::Entity* CurrentEntity = nullptr;
			AZ::ComponentApplicationBus::BroadcastResult(CurrentEntity, &AZ::ComponentApplicationRequests::FindEntity, EntityId);
			CopalChargedForceAggregatorComponent* EntityEMAggregator = CurrentEntity->FindComponent<Copal::CopalChargedForceAggregatorComponent>();
			if (EntityEMAggregator != nullptr)
			{
				AZ::Vector3 ForceToApply = AZ::Vector3::CreateZero();
				Force MyForce;
				if (EnableElectricField)
					ForceToApply += EntityEMAggregator->GetCharge()*ElectricField;
				if (EnableMagneticField)
					ForceToApply += EntityEMAggregator->GetCharge()*(LYVec3ToAZVec3(physicsStatus.v).Cross(MagneticField));
				//PrintVector3("Velocity",physicsStatus.v);
				//PrintVector3("EM force",ForceToApply);
				MyForce.strengthVector = ForceToApply;
				MyForce.tag = ForceTag;
				AddForce(EntityId, MyForce, ForceName);
			}
		}
	}

	void ConstantEMFieldCopalComponent::OnTriggerAreaEntered(AZ::EntityId e)
	{
		AffectedEntities.push_back(e);
	}

	void ConstantEMFieldCopalComponent::OnTriggerAreaExited(AZ::EntityId e)
	{
		AZStd::vector<AZ::EntityId>::iterator position = AZStd::find(AffectedEntities.begin(), AffectedEntities.end(), e);
		if (position != AffectedEntities.end()) // Just in case, It dosnt sound possible that an object will exit an area it hasnt entered.
			AffectedEntities.erase(position);
	}

}