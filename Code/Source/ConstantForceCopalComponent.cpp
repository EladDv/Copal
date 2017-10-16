#include "StdAfx.h"
#include "ConstantForceCopalComponent.h"


namespace Copal
{
	void ConstantForceComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<ConstantForceComponent>()
				->Version(2)
				->Field("ForceName", &ConstantForceComponent::ForceName)
				->Field("ForceTag", &ConstantForceComponent::ForceTag)
				->Field("AttachedEntity", &ConstantForceComponent::AttachedEntity)
				->Field("ForceEnabled", &ConstantForceComponent::ForceEnabled)
				->Field("ForceVector", &ConstantForceComponent::ForceVector)
			;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<ConstantForceComponent>("Constant Force Component", "Adds a simple constant force in specified vector")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &ConstantForceComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &ConstantForceComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::EntityId, &ConstantForceComponent::AttachedEntity, "Attached Entity", "The entity this force will act upon.")
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &ConstantForceComponent::ForceEnabled, "Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &ConstantForceComponent::ForceVector, "Force Vector", "The Force Vector applied to the entity.")
						->Attribute(AZ::Edit::Attributes::Visibility, &ConstantForceComponent::ForceEnabled)
				;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<ConstantForceComponent>()
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					->Property("AttachedEntity", BehaviorValueProperty(&ConstantForceComponent::AttachedEntity))
					->Property("ForceEnabled", BehaviorValueProperty(&ConstantForceComponent::ForceEnabled))
					->Property("ForceVector", BehaviorValueProperty(&ConstantForceComponent::ForceVector))
				;
			}
		}
	}

	void ConstantForceComponent::OnPostPhysicsUpdate()
	{
		if (ForceUpdated) return;

		if (AttachedEntity.IsValid() && AttachedHandler == nullptr)
			AttachedHandler = Copal::CopalPhysicsRequestsBus::FindFirstHandler(AttachedEntity);

		if (AttachedHandler != nullptr) // Always check for nullptr AttachedHandler, even though unlikely it could crash your application!
		{
			if (ForceEnabled)
			{
				Force constForce;
				constForce.strengthVector = ForceVector;
				AttachedHandler->AddForce(ForceName, constForce); // Channels are pretty much pointers to the connected entity. They only expose bus methods
			}
			else
				AttachedHandler->RemoveForce(ForceName); // Channels are pretty much pointers to the connected entity. They only expose bus methods
		}
		ForceUpdated = true;
	}

}