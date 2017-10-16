#pragma once
#include "StdAfx.h"
#include <LmbrCentral\Scripting\TriggerAreaComponentBus.h>
#include "ForceBaseComponent.h"
namespace Copal
{
	class LocalConstantEMFieldComponent
		: public Copal::ForceBaseComponent
		, public LmbrCentral::TriggerAreaNotificationBus::Handler
	{
	public:
		AZ_COMPONENT(LocalConstantEMFieldComponent, "{CD1C1AD7-971A-45F0-921A-9C7BDC7B2098}");

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType & required)
		{
			required.push_back(AZ_CRC("ProximityTriggerService"));
			required.push_back(AZ_CRC("ShapeService"));
		}

		void Activate() override;
		void Deactivate() override;

		void OnPostPhysicsUpdate() override;

		void OnTriggerAreaEntered(AZ::EntityId) override;
		void OnTriggerAreaExited(AZ::EntityId) override;

		bool GetElectricForceEnabled() { return ElectricForceEnabled; }
		void SetElectricForceEnabled(bool enabled) { ElectricForceEnabled = enabled; }

		bool GetMagneticForceEnabled() { return MagneticForceEnabled; }
		void SetMagneticForceEnabled(bool enabled) { MagneticForceEnabled = enabled; }

		AZ::Vector3 GetElectricFieldVector() { return ElectricFieldVector; }
		void SetElectricFieldVector(AZ::Vector3 fieldVector) { ElectricFieldVector = fieldVector; }

		AZ::Vector3 GetMagneticFieldVector() { return MagneticFieldVector; }
		void SetMagneticFieldVector(AZ::Vector3 fieldVector) { MagneticFieldVector = fieldVector; }

	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag = "Electromagnetic";

		AZStd::vector<Copal::CopalPhysicsRequests*> AffectedEntityChannels;

		bool ElectricForceEnabled = false;
		bool MagneticForceEnabled = false;

		AZ::Vector3 ElectricFieldVector = AZ::Vector3(0, 0, 0);
		AZ::Vector3 MagneticFieldVector = AZ::Vector3(0, 0, 0);

	};
}
