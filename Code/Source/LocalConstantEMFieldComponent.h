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
		
		void Print();
		void PrintV(AZ::Vector3);

		void OnTriggerAreaEntered(AZ::EntityId) override;
		void OnTriggerAreaExited(AZ::EntityId) override;

		bool GetElectricForceEnabled() { return EFEnabled; }
		void SetElectricForceEnabled(bool enabled) { EFEnabled = enabled; }

		bool GetMagneticForceEnabled() { return MFEnabled; }
		void SetMagneticForceEnabled(bool enabled) { MFEnabled = enabled; }

		AZ::Vector3 GetElectricFieldVector() { return EFVector; }
		void SetElectricFieldVector(AZ::Vector3 fieldVector) { EFVector = fieldVector; }

		AZ::Vector3 GetMagneticFieldVector() { return MFVector; }
		void SetMagneticFieldVector(AZ::Vector3 fieldVector) { MFVector = fieldVector; }

	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag = "Electromagnetic";

		AZStd::vector<AZ::EntityId> AffectedEntities;

		bool EFEnabled = false;
		bool MFEnabled = false;

		AZ::Vector3 EFVector = AZ::Vector3(10,10,-10);
		AZ::Vector3 MFVector = AZ::Vector3(3, 7, -15);

	};
}
