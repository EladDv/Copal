#pragma once

#include "CopalForceSourceComponent.h"
#include "..\..\..\Code\Engine\LmbrCentral\include\LmbrCentral\Scripting\TriggerAreaComponentBus.h"

namespace Copal
{
	class ConstantEMFieldCopalComponent
		: public Copal::CopalForceSourceComponent
		, public LmbrCentral::TriggerAreaNotificationBus::Handler

	{
	public:
		~ConstantEMFieldCopalComponent() override = default;
		AZ_COMPONENT(ConstantEMFieldCopalComponent, "{EC2B436F-7901-4EB5-8B5E-CF9E493A9D3E}");

		static void Reflect(AZ::ReflectContext* reflection);

		void Activate() override;
		void Deactivate() override;

		void OnPostPhysicsUpdate() override;

		void OnTriggerAreaEntered(AZ::EntityId) override;
		void OnTriggerAreaExited(AZ::EntityId) override;

		bool GetMagneticFieldEnabled() { return EnableMagneticField; }
		void SetMagneticFieldEnabled(bool b) { EnableMagneticField = b; }

		bool GetElectricFieldEnabled() { return EnableElectricField; }
		void SetElectricFieldEnabled(bool b) { EnableElectricField = b; }

		AZ::Vector3 GetMagneticField() { return MagneticField; }
		void SetMagneticField(AZ::Vector3 v) { MagneticField = v; }

		AZ::Vector3 GetElectricField() { return ElectricField; }
		void SetElectricField(AZ::Vector3 v) { ElectricField = v; }

		AZStd::vector<AZ::EntityId> GetAffectedEntities() { return AffectedEntities; }

	protected:
		AZStd::string ForceName;
		AZStd::string ForceTag = "Electromagnetic";

		AZStd::vector<AZ::EntityId> AffectedEntities;

		bool EnableElectricField;
		AZ::Vector3 ElectricField = AZ::Vector3(0,0,0);

		bool EnableMagneticField;
		AZ::Vector3 MagneticField = AZ::Vector3(0, 0, 0);

	};
}
