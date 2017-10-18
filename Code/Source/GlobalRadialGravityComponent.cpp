#include "StdAfx.h"
#include "GlobalRadialGravityComponent.h"


namespace Copal
{
	void GlobalRadialGravityComponent::Reflect(AZ::ReflectContext* reflection)
	{

		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<GlobalRadialGravityComponent>()
				->Version(2)
				->Field("ForceName", &GlobalRadialGravityComponent::ForceName)
				->Field("ForceTag", &GlobalRadialGravityComponent::ForceTag)
				->Field("ForceEnabled", &GlobalRadialGravityComponent::ForceEnabled)
				->Field("SurfaceGravity", &GlobalRadialGravityComponent::SurfaceGravity)
				->Field("BodyRadius", &GlobalRadialGravityComponent::BodyRadius)
				;
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<GlobalRadialGravityComponent>("Constant Force Component", "Adds a simple constant force in specified vector")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/SequenceAgent.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::LineEdit, &GlobalRadialGravityComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::LineEdit, &GlobalRadialGravityComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &GlobalRadialGravityComponent::ForceEnabled, "Force Enabled", "Is the force applied.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &GlobalRadialGravityComponent::SurfaceGravity, "Surface Gravity", "The gravity at the surface of the body.")
					->DataElement(AZ::Edit::UIHandlers::Vector3, &GlobalRadialGravityComponent::BodyRadius, "Body Radius", "The radius of the body (as if it's a perfect sphere) ")
					;
			}


			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
			if (behaviorContext)
			{
				behaviorContext->Class<GlobalRadialGravityComponent>()
					->Method("GetForceName", &ForceBaseComponent::GetForceName)
					->Method("SetForceName", &ForceBaseComponent::SetForceName)
					->Method("GetForceTag", &ForceBaseComponent::GetForceTag)
					->Method("SetForceTag", &ForceBaseComponent::SetForceTag)
					->Property("SurfaceGravity", BehaviorValueProperty(&GlobalRadialGravityComponent::SurfaceGravity))
					->Property("ForceEnabled", BehaviorValueProperty(&GlobalRadialGravityComponent::ForceEnabled))
					->Property("BodyRadius", BehaviorValueProperty(&GlobalRadialGravityComponent::BodyRadius))
					;
			}
		}
	}

	void GlobalRadialGravityComponent::OnPostPhysicsUpdate()
	{
		if (!ForceEnabled)
		{
			CopalPhysicsRequestsBus::Broadcast(&CopalPhysicsRequestsBus::Events::RemoveForce, ForceName);
			return;
		}
		GravitationalFactor = BodyRadius*BodyRadius*SurfaceGravity;
		AZ::EBusAggregateResults<AZ::EntityId> results;
		CopalPhysicsRequestsBus::BroadcastResult(results, &CopalPhysicsRequestsBus::Events::GetComponentEntityId );

		AZ::Vector3 ThisPosition = AZ::Vector3(0, 0, 0);
		AZ::TransformBus::EventResult(ThisPosition, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);

		for (auto EntityId: results.values)
		{
			Force GravForce;
			AZ::Vector3 EntityPosition = AZ::Vector3(0,0,0);
			AZ::TransformBus::EventResult(EntityPosition, EntityId, &AZ::TransformBus::Events::GetWorldTranslation);
			AZ::Vector3 DistanceVector = EntityPosition - ThisPosition; // Note that gravity is negative on the Z+ axis!
			GravForce.strengthVector = DistanceVector.GetNormalizedExact() * ( AZ::VectorFloat(GravitationalFactor) / DistanceVector.GetLengthSq() );
			pe_status_dynamics physicsStatus;
			LmbrCentral::CryPhysicsComponentRequestBus::Event(EntityId, &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);
			GravForce.strengthVector = GravForce.strengthVector * AZ::VectorFloat(physicsStatus.mass);
			CopalPhysicsRequestsBus::Event(EntityId, &CopalPhysicsRequestsBus::Events::AddForce, ForceName, GravForce);
		}
		
	}

}