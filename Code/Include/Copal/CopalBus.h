
#pragma once

#include <AzCore/EBus/EBus.h>
#include <MathConversion.h>
#include <ISystem.h>
#include <ITimer.h>

namespace Copal
{
	struct Force
	{
		AZ_TYPE_INFO(Force, "{A593BFD9-62C1-4812-9CDB-55077A125FEC}");

		AZ::Vector3 strengthVector;
		AZ::Vector3 positionVector;

		bool isTorque = false;
		bool isOffCenter = false;

		AZStd::string tag;

		Force() {	
					strengthVector = AZ::Vector3::CreateZero(); 
					positionVector = AZ::Vector3::CreateZero(); 
					isTorque = false; 
					isOffCenter = false; 
					tag = ""; 
				}
	};
	using CopalForceMap = AZStd::unordered_map<AZStd::string, Copal::Force>;

    class CopalPhysicsRequests
        : public AZ::ComponentBus
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        //////////////////////////////////////////////////////////////////////////

        // Put your public methods here

		/**
		* Adds the force to the forces map of the aggregator,
		* If the force is not off center adds to the running force total
		* \param name - unique force name for map storage MUST BE UNIQUE!
		* \param force - force structs to store in the map as the value for the name
		*/
		virtual void AddForce(const AZStd::string, const Force&) {}

		/**
		* Removes the a force from the forces map of the aggregator,
		* If the force is not off center subtracts from the running force total
		* \param name - unique force name for map search and removal
		*/		
		virtual void RemoveForce(const AZStd::string) {}

		/**
		* Returns a pointer to a force from the forces map of the aggregator,
		* \param name - unique force name for map search and retrival
		* \returns a pointer to a force struct that is the value for the name key in the
		* forces map in the aggregator is one exists, otherwise returns nullptr
		*/
		virtual Force* GetForce(const AZStd::string) { return nullptr; }
		
		/**
		* Returns a pointer to a the forces map of the aggregator,
		* \param outParam - a pointer to a CopalForceMap, will be assigned the forces map
		*/
		virtual void GetAllForces(CopalForceMap*) {}

		/**
		* Adds the torque to the torques map of the aggregator,
		* If the torque is not off center adds to the running torque total
		* \param name - unique torque name for map storage MUST BE UNIQUE!
		* \param torque - force structs to store in the map as the value for the name
		*/
		virtual void AddTorque(const AZStd::string, const Force&) {}
		
		/**
		* Removes the a torque from the torques map of the aggregator,
		* If the torque is not off center subtracts from the running torque total
		* \param name - unique torque name for map search and removal
		* \returns a pointer to a force struct that is the value for the name key in the
		* torques map in the aggregator is one exists, otherwise returns nullptr
		*/
		virtual void RemoveTorque(const AZStd::string) {}

		/**
		* Returns a pointer to a torque from the torques map of the aggregator,
		* \param name - unique torque name for map search and retrival
		*/
		virtual Force* GetTorqe(const AZStd::string) { return nullptr; }
		
		/**
		* Returns a pointer to a the toqrues map of the aggregator,
		* \param outParam - a pointer to a CopalForceMap, will be assigned the toqrues map
		*/
		virtual void GetAllTorques(CopalForceMap*) {}
    };
    using CopalPhysicsRequestsBus = AZ::EBus<CopalPhysicsRequests>;
} // namespace Copal
