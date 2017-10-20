#include "StdAfx.h"
#include "AggregatorBaseComponent.h"

namespace Copal
{
	void AggregatorBaseComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<AggregatorBaseComponent, AZ::Component>()
				->Version(1)
				->Field("Forces Map", &AggregatorBaseComponent::ForcesMap)
				->Field("Torques Map", &AggregatorBaseComponent::TorquesMap)
				->Field("Handled Tags", &AggregatorBaseComponent::HandledTags)
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<AggregatorBaseComponent>("Force Aggregator Base Component", "This component is the base for all force recievers in this GEM.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/TriggerArea.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/TriggerArea.png")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(AZ::Edit::UIHandlers::Default, &AggregatorBaseComponent::HandledTags, "Handled Force Tags", "List of tags handled by this aggregator")
					//->Attribute(AZ::Edit::Attributes::AddNotify, &AggregatorBaseComponent::PrintTags)
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
				behaviorContext->Class<AggregatorBaseComponent>()
					->Attribute(AZ::Script::Attributes::Category, "Copal")
					->RequestBus("CopalPhysicsRequestsBus")
					->Method("CalculateForces", &AggregatorBaseComponent::CalculateForces)
					->Method("ApplyOffCenterImpulsesOnMap", &AggregatorBaseComponent::ApplyOffCenterImpulsesOnMap)
					->Method("DeactivateDrag", &AggregatorBaseComponent::DeactivateDrag)
					->Method("AddTagToHandled", &AggregatorBaseComponent::AddTagToHandled)
					->Method("RemoveTagFromHandled", &AggregatorBaseComponent::RemoveTagFromHandled)
					->Property("HandledTags", BehaviorValueProperty(&AggregatorBaseComponent::HandledTags))
				;
			}
		}
	}
	void AggregatorBaseComponent::PrintTags()
	{
		for each (auto tag in HandledTags)
		{
			CryLogAlways("%s",tag);
		}
	}

	void AggregatorBaseComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType & provided)
	{
		provided.push_back(AZ_CRC("ForceAggregatorService"));
	}

	void AggregatorBaseComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType & required)
	{
		required.push_back(AZ_CRC("PhysicsService"));
	}

	void AggregatorBaseComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType & incompatible)
	{
		incompatible.push_back(AZ_CRC("ForceAggregatorService"));
	}

	void AggregatorBaseComponent::DeactivateDrag()
	{
		IPhysicalEntity* physBuddy = nullptr;
		LmbrCentral::CryPhysicsComponentRequestBus::EventResult(physBuddy, GetEntityId(), &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicalEntity);

		//Nullify all buoyancies, damping and world/entity gravity not from this GEM
		pe_simulation_params simParams, testParams;
		simParams.gravity = AZVec3ToLYVec3(AZ::Vector3::CreateZero());
		simParams.gravityFreefall = AZVec3ToLYVec3(AZ::Vector3::CreateZero());
		simParams.damping = 0;
		simParams.dampingFreefall = 0;

		pe_params_buoyancy buoyancyParams;
		buoyancyParams.waterDamping = 0.f;
		buoyancyParams.kwaterDensity = 0.f;
		buoyancyParams.kwaterResistance = 0.f;
		buoyancyParams.waterEmin = 0.f;

		pe_params_flags flagParams;
		flagParams.flagsOR = pef_ignore_areas;

		physBuddy->SetParams(&simParams);
		physBuddy->SetParams(&buoyancyParams);
		physBuddy->SetParams(&flagParams);
	}

	void AggregatorBaseComponent::Activate()
	{
		DeactivateDrag();
		Copal::CopalPhysicsRequestsBus::Handler::BusConnect(GetEntityId());
		LmbrCentral::PhysicsSystemEventBus::Handler::BusConnect();
	}

	void AggregatorBaseComponent::Deactivate()
	{
		Copal::CopalPhysicsRequestsBus::Handler::BusDisconnect();
		LmbrCentral::PhysicsSystemEventBus::Handler::BusDisconnect();
	}
	void AggregatorBaseComponent::OnPrePhysicsUpdate()
	{
		float dt = 0;
		EBUS_EVENT_RESULT(dt, AZ::TickRequestBus, GetTickDeltaTime);
		CalculateForces(dt);
	}

	void AggregatorBaseComponent::ApplyOffCenterImpulsesOnMap(CopalForceMap& map , float dt, bool isTorque = false)
	{
		for (CopalForceMap::iterator it = map.begin(); it != map.end(); it++)
		{
			if (it->second.isOffCenter) // If not off center already precalculated in the sum - saves on individual impulses on the bus.
			{
				pe_action_impulse impulseActionAtPoint;
				if (isTorque)
					impulseActionAtPoint.angImpulse = AZVec3ToLYVec3(it->second.strengthVector * dt);
				else
					impulseActionAtPoint.impulse = AZVec3ToLYVec3(it->second.strengthVector * dt);

				impulseActionAtPoint.point = AZVec3ToLYVec3(it->second.positionVector);
				impulseActionAtPoint.iApplyTime = 0;
				LmbrCentral::CryPhysicsComponentRequestBus::Event(	GetEntityId(),
																	&LmbrCentral::CryPhysicsComponentRequestBus::Events::ApplyPhysicsAction,
																	impulseActionAtPoint, false); // Apply the impulse to the object.
			}
		}
	}

	void AggregatorBaseComponent::CalculateForces(float dt)
	{
		pe_status_dynamics physicsStatus;
		LmbrCentral::CryPhysicsComponentRequestBus::Event(GetEntityId(), &LmbrCentral::CryPhysicsComponentRequestBus::Events::GetPhysicsStatus, physicsStatus);
		AZ::Vector3 ForceVec = AZ::Vector3::CreateZero();
		pe_action_impulse impulseActionForce, impulseActionTorque;
		AZ::Vector3 impulseToApply; // Apply Forces

		impulseToApply = AZ::Vector3::CreateZero();

		impulseToApply = dt * ForceSum;
		impulseActionForce.impulse = AZVec3ToLYVec3(impulseToApply);
		impulseActionForce.iApplyTime = 0;

		LmbrCentral::CryPhysicsComponentRequestBus::Event(	GetEntityId(),
															&LmbrCentral::CryPhysicsComponentRequestBus::Events::ApplyPhysicsAction,
															impulseActionForce, false); // Apply the impulse to the object.

		impulseToApply = dt * TorqueSum;
		impulseActionTorque.angImpulse = AZVec3ToLYVec3(impulseToApply);
		impulseActionTorque.iApplyTime = 0;
		LmbrCentral::CryPhysicsComponentRequestBus::Event(	GetEntityId(),
															&LmbrCentral::CryPhysicsComponentRequestBus::Events::ApplyPhysicsAction,
															impulseActionTorque, false); // Apply the impulse to the object.

		ApplyOffCenterImpulsesOnMap(ForcesMap, dt, false);
		ApplyOffCenterImpulsesOnMap(TorquesMap, dt, true);
	}

	/**
	* Adds the force to the forces map of the aggregator,
	* If the force is not off center adds to the running force total
	* \param name - unique force name for map storage MUST BE UNIQUE!
	* \param force - force structs to store in the map as the value for the name
	*/
	void AggregatorBaseComponent::AddForce(const AZStd::string name, const Force& force)
	{
		for each (auto tag in HandledTags)
		{	// Search for force tag if has one.
			if (force.tag == tag || force.tag == "")
			{
				if (!force.isOffCenter)
				{
					if (ForcesMap.count(name) != 0) ForceSum -= ForcesMap.at(name).strengthVector;
					ForceSum += force.strengthVector;
				}
				ForcesMap[name] = force;
				//CryLogAlways("Added Force %s", name);
				return;
			}
		}
	}

	/**
	* Removes the a force from the forces map of the aggregator,
	* If the force is not off center subtracts from the running force total
	* \param name - unique force name for map search and removal
	*/
	void AggregatorBaseComponent::RemoveForce(const AZStd::string name)
	{
		auto found = ForcesMap.find(name);
		if (found != ForcesMap.end())
		{
			if (!(found->second.isOffCenter))
				ForceSum -= found->second.strengthVector;
			ForcesMap.erase(name);
		}
	}

	/**
	* Returns a pointer to a force from the forces map of the aggregator,
	* \param name - unique force name for map search and retrival
	* \returns a pointer to a force struct that is the value for the name key in the
	* forces map in the aggregator is one exists, otherwise returns nullptr
	*/
	Force* AggregatorBaseComponent::GetForce(const AZStd::string name)
	{
		auto found = ForcesMap.find(name);
		if (found != ForcesMap.end())
			return &ForcesMap[name];
		return nullptr;
	}

	/**
	* Adds the torque to the torques map of the aggregator,
	* If the torque is not off center adds to the running torque total
	* \param name - unique torque name for map storage MUST BE UNIQUE!
	* \param torque - force structs to store in the map as the value for the name
	*/
	void AggregatorBaseComponent::AddTorque(const AZStd::string name, const Force& torque)
	{
		for each (auto tag in HandledTags)
		{	// Search for force tag if has one.
			if (torque.tag == tag || torque.tag == "")
			{
				if (!torque.isOffCenter)
				{
					if (TorquesMap.count(name) != 0) TorqueSum -= TorquesMap.at(name).strengthVector;
					TorqueSum += torque.strengthVector;
				}
				TorquesMap[name] = torque;
			}
		}
	}

	/**
	* Removes the a torque from the torques map of the aggregator,
	* If the torque is not off center subtracts from the running torque total
	* \param name - unique torque name for map search and removal
	* \returns a pointer to a force struct that is the value for the name key in the 
	* torques map in the aggregator is one exists, otherwise returns nullptr
	*/
	void AggregatorBaseComponent::RemoveTorque(const AZStd::string name)
	{
		auto found = TorquesMap.find(name);
		if (found != TorquesMap.end())
		{
			if (!(found->second.isOffCenter))
				TorqueSum -= found->second.strengthVector;

			TorquesMap.erase(name);
		}
	}

	/**
	* Returns a pointer to a torque from the torques map of the aggregator,
	* \param name - unique torque name for map search and retrival
	*/
	Force* AggregatorBaseComponent::GetTorqe(const AZStd::string name)
	{
		auto found = TorquesMap.find(name);
		if (found != TorquesMap.end())
			return &TorquesMap[name];
		return nullptr;
	}

}