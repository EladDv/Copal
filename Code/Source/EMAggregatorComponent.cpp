#include "StdAfx.h"
#include "EMAggregatorComponent.h"

namespace Copal
{
	void EMAggregatorComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<EMAggregatorComponent, AZ::Component>()
				->Version(1)
				->Field("Forces Map", &EMAggregatorComponent::ForcesMap)
				->Field("Torques Map", &EMAggregatorComponent::TorquesMap)
				->Field("Handled Tags", &EMAggregatorComponent::HandledTags)
				->Field("Charge", &EMAggregatorComponent::Charge)
				->Field("Charge Density", &EMAggregatorComponent::ChargeDensity)
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<EMAggregatorComponent>("Electromagnetic Force Aggregator Component", "This component is the base for all EM force recievers in this GEM.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(0, &EMAggregatorComponent::HandledTags, "Handled Force Tags", "List of tags handled by this aggregator")
					;
			}
			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->EBus<CopalPhysicsRequestsBus>("CopalPhysicsRequestsBus")
					->Attribute(AZ::Script::Attributes::Category, "Copal")
					->Event("GetForce", &CopalPhysicsRequestsBus::Events::GetForce)
					->Event("GetSumOfForces", &CopalPhysicsRequestsBus::Events::GetAllForces)
					->Event("AddForce", &CopalPhysicsRequestsBus::Events::AddForce)
					->Event("RemoveForce", &CopalPhysicsRequestsBus::Events::RemoveForce)

					->Event("GetTorqe", &CopalPhysicsRequestsBus::Events::GetTorqe)
					->Event("GetAllTorques", &CopalPhysicsRequestsBus::Events::GetAllTorques)
					->Event("AddTorque", &CopalPhysicsRequestsBus::Events::AddTorque)
					->Event("RemoveTorque", &CopalPhysicsRequestsBus::Events::RemoveTorque)
					;
				behaviorContext->Class<EMAggregatorComponent>()
					->Attribute(AZ::Script::Attributes::Category, "Copal")
					->RequestBus("CopalPhysicsRequestsBus")
					->Method("CalculateForces", &EMAggregatorComponent::CalculateForces)
					->Method("ApplyOffCenterImpulsesOnMap", &EMAggregatorComponent::ApplyOffCenterImpulsesOnMap)
					->Method("DeactivateDrag", &EMAggregatorComponent::DeactivateDrag)
					->Method("AddTagToHandled", &EMAggregatorComponent::AddTagToHandled)
					->Method("RemoveTagFromHandled", &EMAggregatorComponent::RemoveTagFromHandled)
					->Method("UseDensity", &EMAggregatorComponent::SetDensity)
					->Method("UseCharge", &EMAggregatorComponent::SetCharge)
					->Property("HandledTags", BehaviorValueProperty(&EMAggregatorComponent::HandledTags))
					->Property("Charge", BehaviorValueGetter(&EMAggregatorComponent::Charge), nullptr) // No setting manually to avoid it not matching!
					->Property("ChargeDensity", BehaviorValueGetter(&EMAggregatorComponent::ChargeDensity), nullptr) // No setting manually to avoid it not matching!
					;
			}
		}
	}
	void EMAggregatorComponent::SetDensity(float density)
	{
		pe_status_dynamics physicsStatus;
		LmbrCentral::CryPhysicsComponentRequestBus::Event(GetEntityId(), &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);

		ChargeDensity = density;
		Charge = density * physicsStatus.mass;
	}
	void EMAggregatorComponent::SetCharge(float charge)
	{
		pe_status_dynamics physicsStatus;
		LmbrCentral::CryPhysicsComponentRequestBus::Event(GetEntityId(), &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);
		if (physicsStatus.mass != 0)
			ChargeDensity = charge / physicsStatus.mass;
		else
			ChargeDensity = 0;

		Charge = charge;
	}


}