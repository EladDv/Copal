#include "StdAfx.h"
#include "HarmonicOscillatorComponent.h"

namespace Copal
{
	void HarmonicOscillatorComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<HarmonicOscillatorComponent>()
				->Version(2)
				->Field("ForceName", &HarmonicOscillatorComponent::ForceName)
				->Field("ForceTag", &HarmonicOscillatorComponent::ForceTag)
				->Field("AttachedEntity", &HarmonicOscillatorComponent::AttachedEntity)
				->Field("ForceEnabled", &HarmonicOscillatorComponent::ForceEnabled)
				->Field("Period", &HarmonicOscillatorComponent::Period)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<HarmonicOscillatorComponent>("Harmonic Oscillator Force Component", "Adds a force to a stationary entity to force it to move as a harmonic oscillator")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &HarmonicOscillatorComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &HarmonicOscillatorComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::EntityId, &HarmonicOscillatorComponent::AttachedEntity, "Attached Entity", "The entity this force will act upon.")
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &HarmonicOscillatorComponent::ForceEnabled, "Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Default, &HarmonicOscillatorComponent::Period, "Period", "The period of the oscillator.")
					->Attribute(AZ::Edit::Attributes::Visibility, &HarmonicOscillatorComponent::ForceEnabled)
					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<HarmonicOscillatorComponent>()
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					->Property("AttachedEntity", BehaviorValueGetter(&HarmonicOscillatorComponent::AttachedEntity), nullptr) //Dont set attached entity
					->Property("ForceEnabled", BehaviorValueProperty(&HarmonicOscillatorComponent::ForceEnabled))
					->Property("Period", BehaviorValueProperty(&HarmonicOscillatorComponent::Period))
					;
			}
		}
	}

	void HarmonicOscillatorComponent::OnPostPhysicsUpdate()
	{

		if (!ForceEnabled)
		{
			CopalPhysicsRequestsBus::Event(AttachedEntity, &CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);
			return;
		}

		AZ::Vector3 AttachedEntityLocation = AZ::Vector3(0, 0, 0); // Necessary for GetWorldTM to return a meaninful and correct Transform
		AZ::Vector3 CurrentEntityLocation = AZ::Vector3(0, 0, 0); // Necessary for GetWorldTM to return a meaninful and correct Transform
		AZ::TransformBus::EventResult(AttachedEntityLocation, AttachedEntity, &AZ::TransformBus::Events::GetWorldTranslation);
		AZ::TransformBus::EventResult(CurrentEntityLocation, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);

		pe_status_dynamics physicsStatus;
		LmbrCentral::CryPhysicsComponentRequestBus::Event(AttachedEntity, &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);

		AZ::Vector3 ForceToApply = CurrentEntityLocation - AttachedEntityLocation;
		ForceToApply = ForceToApply * (4 * pow(AZ::Constants::Pi, 2)) *  (physicsStatus.mass / pow(Period, 2)); // Calculate force of oscillator

		Force OscillatorForce;
		OscillatorForce.strengthVector = ForceToApply;
		OscillatorForce.tag = ForceTag;

		CopalPhysicsRequestsBus::Event(AttachedEntity, &CopalPhysicsRequestsBus::Events::AddForce, ForceName, OscillatorForce);

	}

}