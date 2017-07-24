
#include "StdAfx.h"

#include "CopalForceAggregatorComponent.h"

#include <MathConversion.h>

#include <AzCore\Serialization\SerializeContext.h>
#include <AzCore\Serialization\EditContext.h>

#include "..\..\..\Code\Engine\LmbrCentral\include\LmbrCentral\Physics\PhysicsComponentBus.h"

#include <physinterface.h>

namespace Copal
{
	void CopalForceAggregatorComponent::Reflect(AZ::ReflectContext* reflection)
	{
		AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
		if (serializeContext)
		{
			serializeContext->Class<CopalForceAggregatorComponent>()
				->Version(1)
				->Field("Global gravity enabled", &CopalForceAggregatorComponent::m_UseGlobalGravity)
				->Field("Global gravity acceleration", &CopalForceAggregatorComponent::m_GlobalGravity) // A simple shortcut for simple gravity.
				->Field("Handled Force Tags", &CopalForceAggregatorComponent::handledTags) 
				;

			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext)
			{
				editContext->Class<CopalForceAggregatorComponent>("Force Handler Component", "This component aggregates the forces on the entity and sends them to the physics bus.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Copal Physics")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/RigidPhysics.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/RigidPhysics.png")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(0, &CopalForceAggregatorComponent::handledTags, "Handled Force Tags", "List of tags handled by this aggregator")

					->DataElement(0, &CopalForceAggregatorComponent::m_UseGlobalGravity, "Enable Global Gravity", "Enable the use of a global, constant gravitational acceleration")
					
					->DataElement(0, &CopalForceAggregatorComponent::m_GlobalGravity, "Gravity", "Gravitational acceleration") // A simple shortcut for simple gravity.
						->Attribute(AZ::Edit::Attributes::Visibility, &CopalForceAggregatorComponent::m_UseGlobalGravity) 
					;
			}
		}
	}

	void CopalForceAggregatorComponent::Activate()
	{
		CopalForceSourceBus::Handler::BusConnect(GetEntityId());
		LmbrCentral::PhysicsSystemEventBus::Handler::BusConnect();

		//gEnv->pConsole->ExecuteString("t_FixedStep 0.0166666667");
		//gEnv->pTimer->EnableFixedTimeMode(true, (1.f / 30.f));

		m_ForceSum = AZ::Vector3::CreateZero();
		m_TorqueSum = AZ::Vector3::CreateZero();
		LastV = AZ::Vector3::CreateZero();
		AVC = AZ::Vector3::CreateZero();
		m_AddedGlobalGravity = false;
		IPhysicalEntity* physBuddy = nullptr;
		EBUS_EVENT_ID_RESULT(physBuddy, GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicalEntity);
		
		//Nullify all buoyancies, damping and world/entity gravity not from this GEM
		pe_simulation_params simParams,testParams;
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

		/*
		IPhysicalEntity* GravityArea = gEnv->pSystem->GetGlobalEnvironment()->pPhysicalWorld->AddGlobalArea();
		GravityArea->GetParams(&testParams);
		GravityArea->SetParams(&simParams);
		GravityArea->SetParams(&buoyancyParams);
		*/

	}



	void CopalForceAggregatorComponent::Deactivate()
	{
		CopalForceSourceBus::Handler::BusDisconnect(GetEntityId());
		LmbrCentral::PhysicsSystemEventBus::Handler::BusDisconnect();
	}


	void CopalForceAggregatorComponent::OnPrePhysicsUpdate() // Used to push the impulses to the entities - deals with the mass of the objects.
	{
		
		pe_status_dynamics physicsStatus;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);
		AZ::Vector3 Gvec = AZ::Vector3::CreateZero();
		if (GetGlobalGravityEnabled())
		{
			pe_action_impulse impulseAction;
			pe_status_dynamics physicsStatus;
			EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);
			Gvec = m_GlobalGravity * physicsStatus.mass;
			//PrintVector3("Gravity", m_GlobalGravity);
			//PrintVector3("Gravity", Gvec);
			//if (m_UseGlobalGravity)
			//	CryLogAlways("Gravity enabled");

			impulseAction.impulse = AZVec3ToLYVec3(Gvec * (currentTime - lastTime));
			impulseAction.iApplyTime = 0;
			EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseAction, false); // Apply the impulse to the object.

		}
		TotalTime += currentTime - lastTime;

		//CryLogAlways("delta time %f , Frames per second %f", gEnv->pTimer->GetFrameTime(), 1 / gEnv->pTimer->GetFrameTime());
		//CryLogAlways("Total time %f ", TotalTime);
		//CryLogAlways("delta time %f , Frames per second %f", currentTime - lastTime, 1 / (currentTime - lastTime));
		//CryLogAlways("Total time %f ", TotalTime);

		//PrintVector3("Vel: ", physicsStatus.v);
		//PrintVector3("Vel diff: ", LYVec3ToAZVec3(physicsStatus.v) - LastV);
		//PrintVector3("Mismatch: ", AVC - (LYVec3ToAZVec3(physicsStatus.v) - LastV));

		

		ApplyForces(currentTime - lastTime);

		//LastV = LYVec3ToAZVec3(physicsStatus.v);

		lastTime = gEnv->pTimer->GetAsyncCurTime();
	}

	void CopalForceAggregatorComponent::OnPostPhysicsUpdate()
	{
		currentTime = gEnv->pTimer->GetAsyncCurTime();
	}

	void CopalForceAggregatorComponent::ApplyForces(float dt)
	{
		pe_action_impulse impulseActionForce, impulseActionTorque;
		AZ::Vector3 impulseToApply;

		impulseToApply = AZ::Vector3::CreateZero();
		impulseToApply = dt * m_ForceSum;


		/*
		if (impulseToApply != AZ::Vector3::CreateZero())
		{
		pe_status_dynamics physicsStatus;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, physicsStatus);
		CryLogAlways("--------------------------------------------");
		PrintVector3("applied impulse: ", impulseToApply);
		AVC = impulseToApply / physicsStatus.mass;
		PrintVector3("applied vel change: ", AVC);
		PrintVector3("applied force: ", m_ForceSum);
		}
		*/
		impulseActionForce.impulse = AZVec3ToLYVec3(impulseToApply);
		impulseActionForce.iApplyTime = 0;

		for (std::map<AZStd::string, CopalForceSourceComponent::Force>::iterator it = m_Forces.begin(); it != m_Forces.end(); it++)
		{
			AZStd::list<AZStd::string>::iterator found = AZStd::find(handledTags.begin(), handledTags.end(), it->second.tag);
			if (!(it->second.isOffCenter) || found != handledTags.end())
				continue;

			pe_action_impulse impulseActionAtPoint;
			if (!(it->second.isTorque))
			{
				impulseActionAtPoint.impulse = AZVec3ToLYVec3(it->second.strengthVector * dt);
				impulseActionAtPoint.point = AZVec3ToLYVec3(it->second.positionVector);
				impulseActionAtPoint.iApplyTime = 0;
				EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseActionAtPoint, false); // Apply the impulse to the object.
			}
			else
			{
				impulseActionAtPoint.angImpulse = AZVec3ToLYVec3(it->second.strengthVector * dt);
				impulseActionAtPoint.point = AZVec3ToLYVec3(it->second.positionVector);
				impulseActionAtPoint.iApplyTime = 0;
				EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseActionAtPoint, false); // Apply the impulse to the object.
			}

		}

		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseActionForce, false); // Apply the impulse to the object.

		impulseToApply = AZ::Vector3::CreateZero();
		impulseToApply = dt * m_TorqueSum;
		impulseActionTorque.angImpulse = AZVec3ToLYVec3(impulseToApply);
		impulseActionTorque.iApplyTime = 0;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseActionTorque, false); // Apply the impulse to the object.

	}

	/*---------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	--------------------------------------------------------- FORCES ------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------*/

	

	void CopalForceAggregatorComponent::AddForce(AZ::EntityId eId, CopalForceSourceComponent::Force v, AZStd::string name)
	{
		//CryLogAlways(" Got a Force from ");
		AZStd::list<AZStd::string>::iterator found = AZStd::find(handledTags.begin(), handledTags.end(), v.tag);
		if (found != handledTags.end())
			return;

		if (m_Forces.find(name) == m_Forces.end())
		{
			if (!v.isOffCenter)
			{
				m_Forces[name] = v;
				m_ForceSum += v.strengthVector;
			}//PrintVector3("MY FORCE:", v);
		}
		else // in case we already have a force like that accidentally, cover for the user.
			UpdateForce(eId, v, name);
	}
	void CopalForceAggregatorComponent::RemoveForce(AZ::EntityId eId, AZStd::string name)
	{
		if (m_Forces.find(name) != m_Forces.end())
		{
			if (!m_Forces[name].isOffCenter)
				m_ForceSum -= m_Forces[name].strengthVector;

			m_Forces.erase(name);
		}

	}

	void CopalForceAggregatorComponent::UpdateForce(AZ::EntityId eId, CopalForceSourceComponent::Force v, AZStd::string name)
	{
		AZStd::list<AZStd::string>::iterator found = AZStd::find(handledTags.begin(), handledTags.end(), v.tag);
		if (found != handledTags.end())
			return;

		if (m_Forces.find(name) != m_Forces.end())
		{

			if (!m_Forces[name].isOffCenter)
				m_ForceSum -= m_Forces[name].strengthVector;
			if (!v.isOffCenter)
				m_ForceSum += v.strengthVector;

			m_Forces[name] = v;
		}
	}

	CopalForceSourceComponent::Force* CopalForceAggregatorComponent::GetForce(AZ::EntityId eId, AZStd::string name)
	{
		if (m_Forces.find(name) != m_Forces.end())
			return &m_Forces[name];

		return nullptr;
	}

	AZ::Vector3 CopalForceAggregatorComponent::GetSumOfForcesAtCenter(AZ::EntityId eId)
	{
		return m_ForceSum;
	}

	/*---------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	--------------------------------------------------------- TORQUES -----------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------*/

	void CopalForceAggregatorComponent::AddTorque(AZ::EntityId eId, CopalForceSourceComponent::Force v, AZStd::string name)
	{
		//CryLogAlways(" Got a Force from ");
		AZStd::list<AZStd::string>::iterator found = AZStd::find(handledTags.begin(), handledTags.end(), v.tag);
		if (found != handledTags.end())
			return;

		if (m_Forces.find(name) == m_Forces.end())
		{
			if (!v.isOffCenter)
			{
				m_Forces[name] = v;
				m_TorqueSum += v.strengthVector;
			}//PrintVector3("MY FORCE:", v);
		}
		else // in case we already have a force like that accidentally, cover for the user.
			UpdateTorque(eId, v, name);
	}
	void CopalForceAggregatorComponent::RemoveTorque(AZ::EntityId eId, AZStd::string name)
	{
		if (m_Forces.find(name) != m_Forces.end())
		{
			if (!m_Forces[name].isOffCenter)
				m_TorqueSum -= m_Forces[name].strengthVector;

			m_Forces.erase(name);
		}
	}

	void CopalForceAggregatorComponent::UpdateTorque(AZ::EntityId eId, CopalForceSourceComponent::Force v, AZStd::string name)
	{
		AZStd::list<AZStd::string>::iterator found = AZStd::find(handledTags.begin(), handledTags.end(), v.tag);
		if (found != handledTags.end())
			return;

		if (m_Forces.find(name) != m_Forces.end())
		{

			if (!m_Forces[name].isOffCenter)
				m_TorqueSum -= m_Forces[name].strengthVector;
			if (!v.isOffCenter)
				m_TorqueSum += v.strengthVector;

			m_Forces[name] = v;
		}

	}
	CopalForceSourceComponent::Force* CopalForceAggregatorComponent::GetTorque(AZ::EntityId eId, AZStd::string name)
	{

		if (m_Forces.find(name) != m_Forces.end())
			return &m_Forces[name];

		return nullptr;
	}

	AZ::Vector3 CopalForceAggregatorComponent::GetSumOfTorquesAtCenter(AZ::EntityId eId)
	{
		return m_TorqueSum;
	}

	/*---------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-------------------------------------------------------- MISCELLANEOUS ------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------------------------------------------------------------------------------------*/

	void CopalForceAggregatorComponent::SetGlobalGravity(AZ::EntityId eId, AZ::Vector3 v)
	{
		m_GlobalGravity = v;
	}
	AZ::Vector3 CopalForceAggregatorComponent::GetGlobalGravity(AZ::EntityId eId)
	{
		if (m_UseGlobalGravity)
			return m_GlobalGravity;
		return AZ::Vector3::CreateZero();
	}
}