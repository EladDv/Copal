
#include "StdAfx.h"

#include "CopalForceSourceComponent.h"

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <LmbrCentral\Physics\PhysicsComponentBus.h>
#include <MathConversion.h>

namespace Copal
{
	void CopalForceSourceComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<CopalForceSourceComponent>()
				->Version(1)
				->Field("Force Name", &CopalForceSourceComponent::ForceName)
				->Field("Force Tag", &CopalForceSourceComponent::ForceTag)
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<CopalForceSourceComponent>("Force Source Component", "This component is the base for all force senders in this GEM.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/TriggerArea.png")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &CopalForceSourceComponent::ForceName, "Force Name", "Unique name of the force to be used.") //This is important, needs to be unique
					->DataElement(0, &CopalForceSourceComponent::ForceTag, "Force Tag", "Type of force to be applied, only applied to entities who are able to handle those types.") //This is important, needs to be unique

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
				behaviorContext->Class<CopalForceSourceComponent>()
					->Method("GetForceName", &CopalForceSourceComponent::GetForceName)
					->Method("SetForceNameAZString", &CopalForceSourceComponent::SetForceNameAZString)
					->Method("GetTag", &CopalForceSourceComponent::GetTag)
					->Method("SetTag", &CopalForceSourceComponent::SetTag)
					;
			}
		}
	}

	void CopalForceSourceComponent::Activate()
	{
		LmbrCentral::PhysicsSystemEventBus::Handler::BusConnect();
	}

	void CopalForceSourceComponent::Deactivate()
	{
		CopalForceSourceBus::Broadcast(&CopalForceSourceBus::Events::RemoveForce, GetEntityId(), ForceName); // Remove this force from everywhere before deactivating
		LmbrCentral::PhysicsSystemEventBus::Handler::BusDisconnect();
	}

	void CopalForceSourceComponent::OnPrePhysicsUpdate(){}; // Dont add any code here, this is the part where the force aggregators update themselves

	void CopalForceSourceComponent::OnPostPhysicsUpdate(){};// Add your forces in here.


	/*---------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	--------------------------------------------------------- FORCES ------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------*/

	void CopalForceSourceComponent::AddForce(AZ::EntityId eId, CopalForceSourceComponent::Force Force, AZStd::string name) // Add a new force to an entity. If it exists the force will be updated.
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::AddForce, eId, Force, name);
	}

	void CopalForceSourceComponent::RemoveForce(AZ::EntityId eId, AZStd::string name) // Remove your force from the entity
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::RemoveForce, eId, name);
	}

	void CopalForceSourceComponent::UpdateForce(AZ::EntityId eId, CopalForceSourceComponent::Force Force, AZStd::string name) // Update the force currently on the entity
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::UpdateForce, eId, Force, name);
	}

	CopalForceSourceComponent::Force* CopalForceSourceComponent::GetForce(AZ::EntityId eId, AZStd::string name) //Gets a force by name, not just current one.
	{
		Force* res;
		CopalForceSourceBus::EventResult(res, eId, &CopalForceSourceBus::Events::GetForce, eId, name);
		return res;
	}
	AZ::Vector3 CopalForceSourceComponent::GetSumOfForcesAtCenter(AZ::EntityId eId)
	{
		AZ::Vector3 res;
		CopalForceSourceBus::EventResult(res, eId, &CopalForceSourceBus::Events::GetSumOfForcesAtCenter, eId);
		return res;
	}

	/*---------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	--------------------------------------------------------- TORQUES -----------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------*/

	void CopalForceSourceComponent::AddTorque(AZ::EntityId eId, CopalForceSourceComponent::Force Torque, AZStd::string name) // Add a new torque to an entity. If it exists the torque will be updated.
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::AddTorque, eId, Torque, name);
	}


	void CopalForceSourceComponent::RemoveTorque(AZ::EntityId eId, AZStd::string name) // Remove your torque from the entity
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::RemoveTorque, eId, name);
	}

	void CopalForceSourceComponent::UpdateTorque(AZ::EntityId eId, CopalForceSourceComponent::Force Torque, AZStd::string name) // Update the torque currently on the entity
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::UpdateTorque, eId, Torque, name);
	}

	CopalForceSourceComponent::Force* CopalForceSourceComponent::GetTorque(AZ::EntityId eId, AZStd::string name) //Gets a torque by name, not just current one.
	{
		Force* res;
		CopalForceSourceBus::EventResult(res, eId, &CopalForceSourceBus::Events::GetTorque, eId, name);
		return res;
	}
	AZ::Vector3 CopalForceSourceComponent::GetSumOfTorquesAtCenter(AZ::EntityId eId)
	{
		AZ::Vector3 res;
		CopalForceSourceBus::EventResult(res, eId, &CopalForceSourceBus::Events::GetSumOfTorquesAtCenter, eId);
		return res;
	}

	/*---------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-------------------------------------------------------- MISCELLANEOUS ------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------*/

	void CopalForceSourceComponent::SetForceNameAZString(AZStd::string name)
	{
		ForceName = name;
	}
	void CopalForceSourceComponent::SetForceNameString(string name)
	{
		ForceName = AZStd::string(name);
	}

	void CopalForceSourceComponent::PrintVector3(string Prefix, AZ::Vector3 v)
	{

		CryLogAlways(Prefix + " %f , %f ,%f ", float(v.GetX()), float(v.GetY()), float(v.GetZ()));
	}

	void CopalForceSourceComponent::PrintVector3(string Prefix, Vec3 v)
	{
		CopalForceSourceComponent::PrintVector3(Prefix, LYVec3ToAZVec3(v));
	}

	void CopalForceSourceComponent::SetGlobalGravity(AZ::EntityId eId, AZ::Vector3 v)
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::SetGlobalGravity, eId, v);
	}
	AZ::Vector3 CopalForceSourceComponent::GetGlobalGravity(AZ::EntityId eId)
	{
		AZ::Vector3 res;
		CopalForceSourceBus::EventResult(res, eId, &CopalForceSourceBus::Events::GetGlobalGravity, eId);
		return res;
	}

	AZ::EBusAggregateResults<AZ::EntityId> CopalForceSourceComponent::GetAllEntityIds() // Gets all the Ids it can find on the bus.
	{
		AZ::EBusAggregateResults<AZ::EntityId> results;
		CopalForceSourceBus::BroadcastResult(results, &CopalForceSourceBus::Events::GetEntityIdEvent);
		return results;
	}


	AZ::EntityId CopalForceSourceComponent::GetEntityIdEvent() // Gets the last ID on the bus.
	{
		AZ::EntityId result;
		CopalForceSourceBus::BroadcastResult(result, &CopalForceSourceBus::Events::GetEntityIdEvent);
		return result;
	}

	void CopalForceSourceComponent::EnableGlobalGravity(AZ::EntityId eId)
	{ 
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::EnableGlobalGravity, eId);
	}
	void CopalForceSourceComponent::DisableGlobalGravity(AZ::EntityId eId)
	{
		CopalForceSourceBus::Event(eId, &CopalForceSourceBus::Events::DisableGlobalGravity, eId);
	}
}