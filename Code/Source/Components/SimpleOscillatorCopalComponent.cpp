
#include "StdAfx.h"

#include "SimpleOscillatorCopalComponent.h"

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Copal
{
	void SimpleOscillatorCopalComponent::Reflect(AZ::ReflectContext* reflection)
	{
		
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<SimpleOscillatorCopalComponent>()
				->Version(1)
				->Field("Force Name", &SimpleOscillatorCopalComponent::ForceName)
				->Field("Force Tag", &SimpleOscillatorCopalComponent::ForceTag)
				->Field("Attached Entity", &SimpleOscillatorCopalComponent::m_AttachedEntity)
				->Field("Period", &SimpleOscillatorCopalComponent::Period)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<SimpleOscillatorCopalComponent>("Simple Oscillator Component", "Adds a force that would create a simple harmonic oscillator with a set period if the entity starts at rest")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &SimpleOscillatorCopalComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(0, &SimpleOscillatorCopalComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(0, &SimpleOscillatorCopalComponent::m_AttachedEntity, "Attached Entity", "The entity attached to this spring.")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, &SimpleOscillatorCopalComponent::OnAttachedEntityChanged)

					->DataElement(0, &SimpleOscillatorCopalComponent::Period, "Oscillation Period", "Period of the oscillation of the entity conneted to the spring in seconds.")
						->Attribute(AZ::Edit::Attributes::Max, 1000)
						->Attribute(AZ::Edit::Attributes::Min, 0.01)
					;
			}

		
			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->EBus<CopalForceSourceBus>("CopalForceSourceBus")
					->Event("AddForce", &CopalForceSourceBus::Events::AddForce)
					->Event("RemoveForce", &CopalForceSourceBus::Events::RemoveForce)
					->Event("UpdateForce", &CopalForceSourceBus::Events::UpdateForce)
					//-----------------------------------------------------------------------------------
					->Event("AddTorque", &CopalForceSourceBus::Events::AddTorque)
					->Event("RemoveTorque", &CopalForceSourceBus::Events::RemoveTorque)
					->Event("UpdateTorque", &CopalForceSourceBus::Events::UpdateTorque)
					//-----------------------------------------------------------------------------------
					->Event("EnableGlobalGravity", &CopalForceSourceBus::Events::EnableGlobalGravity)
					->Event("DisableGlobalGravity", &CopalForceSourceBus::Events::DisableGlobalGravity)
					->Event("GetGlobalGravity", &CopalForceSourceBus::Events::GetGlobalGravity)
					->Event("SetGlobalGravity", &CopalForceSourceBus::Events::SetGlobalGravity)
					;
				behaviorContext->Class<SimpleOscillatorCopalComponent>()
					->Method("GetForceName", &SimpleOscillatorCopalComponent::GetForceName)
					->Method("SetForceNameAZString", &SimpleOscillatorCopalComponent::SetForceNameAZString)
					->Method("GetAttachedEntity", &SimpleOscillatorCopalComponent::GetAttachedEntity)
					->Method("SetAttachedEntity", &SimpleOscillatorCopalComponent::SetAttachedEntity)
					->Method("GetPeriod", &SimpleOscillatorCopalComponent::GetPeriod)
					->Method("SetPeriod", &SimpleOscillatorCopalComponent::SetPeriod)
					->Method("GetTag", &SimpleOscillatorCopalComponent::GetTag)
					->Method("SetTag", &SimpleOscillatorCopalComponent::SetTag);
			}
		}
	}

	void SimpleOscillatorCopalComponent::OnPostPhysicsUpdate()
	{
		AZ::Transform m_AttachedEntityTransform = AZ::Transform::CreateIdentity(); // Necessary for GetWorldTM to return a meaninful and correct Transform
		AZ::Transform m_EntityTransform = AZ::Transform::CreateIdentity(); // Necessary for GetWorldTM to return a meaninful and correct Transform
		EBUS_EVENT_ID_RESULT(m_AttachedEntityTransform, GetAttachedEntity(), AZ::TransformBus, GetWorldTM);
		EBUS_EVENT_ID_RESULT(m_EntityTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

		pe_status_dynamics physicsStatus;
		EBUS_EVENT_ID(GetAttachedEntity(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);

		AZ::Vector3 ForceToApply = m_EntityTransform.GetPosition() - m_AttachedEntityTransform.GetPosition();
		ForceToApply = ForceToApply * (4 * pow(AZ::Constants::Pi, 2)) *  (physicsStatus.mass / pow(Period, 2));
		
		//CryLogAlways("Needed K = %f" ,((4 * pow(AZ::Constants::Pi, 2)) *  (physicsStatus.mass / pow(Period, 2))) );
		//SimpleOscillatorCopalComponent::PrintVector3("Distance: ",m_EntityTransform.GetPosition() - m_AttachedEntityTransform.GetPosition());
		CopalForceSourceComponent::Force MyForce;
		MyForce.strengthVector = ForceToApply;
		MyForce.tag = ForceTag;

		AddForce(GetAttachedEntity(), MyForce, ForceName);

	}


	AZ::EntityId SimpleOscillatorCopalComponent::GetAttachedEntity()
	{
		return m_AttachedEntity; 
	}

	void SimpleOscillatorCopalComponent::OnAttachedEntityChanged()
	{
		SimpleOscillatorCopalComponent::RemoveForce(m_PreviousAttachedEntity, ForceName);
		m_PreviousAttachedEntity = m_AttachedEntity;
	}

	void SimpleOscillatorCopalComponent::SetAttachedEntity(AZ::EntityId newEntity)
	{
		SimpleOscillatorCopalComponent::RemoveForce(m_AttachedEntity, ForceName);
		m_AttachedEntity = newEntity;
		m_PreviousAttachedEntity = newEntity;
	}


	float SimpleOscillatorCopalComponent::GetPeriod()
	{ 
		return Period; 
	}
	void SimpleOscillatorCopalComponent::SetPeriod(float newPeriod)
	{
		Period = newPeriod;
	}
}