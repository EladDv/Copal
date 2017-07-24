
#include "StdAfx.h"

#include "SimpleSpringCopalComponent.h"

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Copal
{
	void SimpleSpringCopalComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<SimpleSpringCopalComponent>()
				->Version(1)
				->Field("Force Name", &SimpleSpringCopalComponent::ForceName)
				->Field("Force Tag", &SimpleSpringCopalComponent::ForceTag)
				->Field("Attached Entity", &SimpleSpringCopalComponent::m_AttachedEntity)
				->Field("k value", &SimpleSpringCopalComponent::k_value)
				->Field("Rest Length", &SimpleSpringCopalComponent::restLength)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<SimpleSpringCopalComponent>("Simple Spring Component", "Adds a force that would create a simple spring with a K value and resting length")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &SimpleSpringCopalComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(0, &SimpleSpringCopalComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(0, &SimpleSpringCopalComponent::m_AttachedEntity, "Attached Entity", "The entity attached to this spring.")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, &SimpleSpringCopalComponent::OnAttachedEntityChanged)

					->DataElement(0, &SimpleSpringCopalComponent::k_value, "Spring Constant", "Represents the stiffness of the spring. (F = -K * Distance)")
						->Attribute(AZ::Edit::Attributes::Max, 1000)
						->Attribute(AZ::Edit::Attributes::Min, 0.01)

					->DataElement(0, &SimpleSpringCopalComponent::restLength, "Rest Length", "Represents the distance of the spring when no force is applied.")
						->Attribute(AZ::Edit::Attributes::Max, 1000)
						->Attribute(AZ::Edit::Attributes::Min, 0)

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
				behaviorContext->Class<SimpleSpringCopalComponent>()
					->Method("GetForceName", &SimpleSpringCopalComponent::GetForceName)
					->Method("SetForceNameAZString", &SimpleSpringCopalComponent::SetForceNameAZString)
					->Method("GetAttachedEntity", &SimpleSpringCopalComponent::GetAttachedEntity)
					->Method("SetAttachedEntity", &SimpleSpringCopalComponent::SetAttachedEntity)
					->Method("GetPeriod", &SimpleSpringCopalComponent::GetK_value)
					->Method("SetPeriod", &SimpleSpringCopalComponent::SetK_value)
					->Method("GetRestLength", &SimpleSpringCopalComponent::GetRestLength)
					->Method("SetRestLength", &SimpleSpringCopalComponent::SetRestLength)
					->Method("GetTag", &SimpleSpringCopalComponent::GetTag)
					->Method("SetTag", &SimpleSpringCopalComponent::SetTag);
			}
		}
	}

	void SimpleSpringCopalComponent::OnPostPhysicsUpdate()
	{
		AZ::Transform m_AttachedEntityTransform = AZ::Transform::CreateIdentity(); // Necessary for GetWorldTM to return a meaninful and correct Transform
		AZ::Transform m_EntityTransform = AZ::Transform::CreateIdentity(); // Necessary for GetWorldTM to return a meaninful and correct Transform
		EBUS_EVENT_ID_RESULT(m_AttachedEntityTransform, GetAttachedEntity(), AZ::TransformBus, GetWorldTM);
		EBUS_EVENT_ID_RESULT(m_EntityTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

		AZ::Vector3 DistanceToTarget = (m_AttachedEntityTransform.GetPosition() -m_EntityTransform.GetPosition() ).GetNormalizedExact() * restLength; // Get position where the spring ends while at rest relative to the start.
		DistanceToTarget += m_EntityTransform.GetPosition(); // add that to the actual location, get the world coordinates of the end of the resting spring
		AZ::Vector3 ForceToApply = k_value * (DistanceToTarget - m_AttachedEntityTransform.GetPosition()); // compute the delta X and multiply by k (delta X is always towards the end of the spring when doing this calculation)
		CopalForceSourceComponent::Force MyForce;
		MyForce.strengthVector = ForceToApply;
		MyForce.tag = ForceTag;
		AddForce(GetAttachedEntity(), MyForce, ForceName);

	}


	AZ::EntityId SimpleSpringCopalComponent::GetAttachedEntity()
	{
		return m_AttachedEntity;
	}

	void SimpleSpringCopalComponent::OnAttachedEntityChanged()
	{
		SimpleSpringCopalComponent::RemoveForce(m_PreviousAttachedEntity, ForceName);
		m_PreviousAttachedEntity = m_AttachedEntity;
	}

	void SimpleSpringCopalComponent::SetAttachedEntity(AZ::EntityId newEntity)
	{
		SimpleSpringCopalComponent::RemoveForce(m_AttachedEntity, ForceName);
		m_AttachedEntity = newEntity;
		m_PreviousAttachedEntity = newEntity;
	}

}