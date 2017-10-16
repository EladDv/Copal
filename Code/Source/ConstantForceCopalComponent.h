#pragma once
#include "StdAfx.h"
#include "ForceBaseComponent.h"
namespace Copal
{
	class ConstantForceComponent
		: public Copal::ForceBaseComponent
	{
		public:
			AZ_COMPONENT(ConstantForceComponent, "{4CB90E17-DE88-4166-90BC-70D428D8F077}");

			// Required Reflect function.
			static void Reflect(AZ::ReflectContext* context);

			void OnPostPhysicsUpdate() override;

			bool GetForceEnabled() { return ForceEnabled; }
			void SetForceEnabled(bool e) { ForceEnabled = e; ForceUpdated = false; }

			AZ::Vector3 GetForceVector() { return ForceVector; }
			void SetForceVector(AZ::Vector3 v) { ForceVector = v; ForceUpdated = false; }

			AZ::EntityId GetAttachedEntity() { return AttachedEntity; }
			void SetAttachedEntity(AZ::EntityId e)
			{
				AttachedEntity = e;
				AttachedHandler = Copal::CopalPhysicsRequestsBus::FindFirstHandler(e);
				ForceUpdated = false;
			}


		protected:
			AZStd::string ForceName;
			AZStd::string ForceTag;

			AZ::EntityId AttachedEntity;
			Copal::CopalPhysicsRequests* AttachedHandler;

			bool ForceEnabled = false;
			bool ForceUpdated = false; // Save CPU cycles on already updated forces
			AZ::Vector3 ForceVector = AZ::Vector3(0, 0, 0);

	};
}

