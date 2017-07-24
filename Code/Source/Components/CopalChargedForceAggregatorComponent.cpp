
#include "StdAfx.h"

#include "CopalChargedForceAggregatorComponent.h"

#include <MathConversion.h>

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>

#include "..\..\..\Code\Engine\LmbrCentral\include\LmbrCentral\Physics\PhysicsComponentBus.h"

#include <physinterface.h>

namespace Copal
{
	void CopalChargedForceAggregatorComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<CopalChargedForceAggregatorComponent>()
				->Version(1)
				->Field("Handled Force Tags", &CopalChargedForceAggregatorComponent::handledTags)
				->Field("Charge Or Charge to Mass", &CopalChargedForceAggregatorComponent::charge_or_chargeToMass)
				->Field("Charge", &CopalChargedForceAggregatorComponent::m_charge)
				->Field("Charge to Mass", &CopalChargedForceAggregatorComponent::m_chargeToMass)
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<CopalChargedForceAggregatorComponent>("Electromagnetic Force Handler Component", "This component aggregates the forces (including EM forces) on the entity and sends them to the physics bus.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/RigidPhysics.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/RigidPhysics.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &CopalChargedForceAggregatorComponent::handledTags, "Handled Force Tags", "List of tags handled by this aggregator")

					->DataElement(AZ::Edit::UIHandlers::ComboBox, &CopalChargedForceAggregatorComponent::charge_or_chargeToMass,
					"Specify Charge or Charge to Mass", "Whether total Charge is specified, or calculated at spawn time based on mass and ratio")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
							->EnumAttribute(CopalChargedForceAggregatorComponent::ChargeOrChargeToMass::Charge, "Charge")
							->EnumAttribute(CopalChargedForceAggregatorComponent::ChargeOrChargeToMass::ChargeToMass, "Charge To Mass")

					->DataElement(0, &CopalChargedForceAggregatorComponent::m_charge, "Charge (Coulomb)",
					"Total charge of the objects in coulombs.")
						->Attribute(AZ::Edit::Attributes::Visibility, &CopalChargedForceAggregatorComponent::UsingCharge)
						->Attribute(AZ::Edit::Attributes::Min, 0.f)

					->DataElement(0, &CopalChargedForceAggregatorComponent::m_chargeToMass,
					"Charge to Mass ratio (coulomb/kg)", "The charge to mass ratio of the entity in coulomb/kg")
						->Attribute(AZ::Edit::Attributes::Visibility, &CopalChargedForceAggregatorComponent::UsingChargeToMass)
						->Attribute(AZ::Edit::Attributes::Min, 0.f)

					;
			}
		}
	}

	void CopalChargedForceAggregatorComponent::OnPrePhysicsUpdate()// Used to push the impulses to the entities - deals with the mass of the objects.
	{
		CopalForceAggregatorComponent::OnPrePhysicsUpdate();
	}

	void CopalChargedForceAggregatorComponent::SetCharge(float f)
	{
		pe_status_dynamics physicsStatus;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);
		m_charge = f;
		if (physicsStatus.mass != 0)
			m_chargeToMass = f / physicsStatus.mass;

	}
	void CopalChargedForceAggregatorComponent::SetChargeToMass(float f)
	{
		pe_status_dynamics physicsStatus;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);
		if (physicsStatus.mass != 0)
			m_charge = f * physicsStatus.mass;
		m_chargeToMass = f ;

	}

	void CopalChargedForceAggregatorComponent::SetUsingCharge(bool b)
	{ 
		charge_or_chargeToMass = b ? ChargeOrChargeToMass::Charge : ChargeOrChargeToMass::ChargeToMass; 
		b ? GetChargeToMass() : GetCharge(); // make sure to refresh both values when setting.
	}
	void CopalChargedForceAggregatorComponent::SetUsingChargeToMass(bool b) 
	{ 
		charge_or_chargeToMass = (!b) ? ChargeOrChargeToMass::Charge : ChargeOrChargeToMass::ChargeToMass; 
		(!b) ? GetChargeToMass() : GetCharge(); // make sure to refresh both values when setting.

	}


	float CopalChargedForceAggregatorComponent::GetCharge() 
	{
		if (UsingCharge())
			return m_charge;

		SetChargeToMass(m_chargeToMass); //By knowing the opposite one it will update the one we asked for as well
		return m_charge;
	}
	float CopalChargedForceAggregatorComponent::GetChargeToMass() 
	{
		if (UsingChargeToMass())
			return m_chargeToMass;

		SetCharge(m_charge); //By knowing the opposite one it will update the one we asked for as well
		return m_chargeToMass;
	}
}