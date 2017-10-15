#include "StdAfx.h"
#include "SimpleSpringForceComponent.h"

namespace Copal
{
	void SimpleSpringForceComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<SimpleSpringForceComponent>()
				->Version(2)
				->Field("ForceName", &SimpleSpringForceComponent::ForceName)
				->Field("ForceTag", &SimpleSpringForceComponent::ForceTag)
				->Field("AttachedEntity", &SimpleSpringForceComponent::AttachedEntity)
				->Field("ForceEnabled", &SimpleSpringForceComponent::ForceEnabled)
				->Field("K", &SimpleSpringForceComponent::K)
				->Field("RestLength", &SimpleSpringForceComponent::RestLength)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<SimpleSpringForceComponent>("Simple Spring Force Component", "Adds a force to a stationary entity to force it to move as if attached to a spring")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &SimpleSpringForceComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &SimpleSpringForceComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::EntityId, &SimpleSpringForceComponent::AttachedEntity, "Attached Entity", "The entity this force will act upon.")
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &SimpleSpringForceComponent::ForceEnabled, "Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Default, &SimpleSpringForceComponent::K, "K (spring constant)", "The spring constant (K) of the spring.")
					->Attribute(AZ::Edit::Attributes::Visibility, &SimpleSpringForceComponent::ForceEnabled)
					->DataElement(AZ::Edit::UIHandlers::Default, &SimpleSpringForceComponent::RestLength, "Rest Length", "The resting length of the spring.")
					->Attribute(AZ::Edit::Attributes::Visibility, &SimpleSpringForceComponent::ForceEnabled)
					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<SimpleSpringForceComponent>()
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					->Property("AttachedEntity", BehaviorValueGetter(&SimpleSpringForceComponent::AttachedEntity), nullptr) //Dont set attached entity
					->Property("ForceEnabled", BehaviorValueProperty(&SimpleSpringForceComponent::ForceEnabled))
					->Property("K", BehaviorValueProperty(&SimpleSpringForceComponent::K))
					->Property("RestLength", BehaviorValueProperty(&SimpleSpringForceComponent::RestLength))
					;
			}
		}
	}

	void SimpleSpringForceComponent::OnPostPhysicsUpdate()
	{
		AZ::Vector3 AttachedEntityLocation = AZ::Vector3(0, 0, 0); // Necessary for GetWorldTM to return a meaninful and correct Transform
		AZ::Vector3 CurrentEntityLocation = AZ::Vector3(0, 0, 0); // Necessary for GetWorldTM to return a meaninful and correct Transform
		AZ::TransformBus::EventResult(AttachedEntityLocation, AttachedEntity, &AZ::TransformBus::Events::GetWorldTranslation);
		AZ::TransformBus::EventResult(CurrentEntityLocation, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);

		pe_status_dynamics physicsStatus;
		LmbrCentral::CryPhysicsComponentRequestBus::Event(GetAttachedEntity(), &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);

		AZ::Vector3 RestPosition = (AttachedEntityLocation - CurrentEntityLocation).GetNormalizedExact() * RestLength; // Get position where the spring ends while at rest relative to the start.
		RestPosition += CurrentEntityLocation; // add that to the actual location, get the world coordinates of the end of the resting spring
		AZ::Vector3 ForceToApply = K * (RestPosition - AttachedEntityLocation); // compute the delta X and multiply by k (delta X is always towards the end of the spring when doing this calculation)

		Force SpringForce;
		SpringForce.strengthVector = ForceToApply;
		SpringForce.tag = ForceTag;

		CopalPhysicsRequestsBus::Event(AttachedEntity, &CopalPhysicsRequestsBus::Events::AddForce, ForceName, SpringForce);
	}

}