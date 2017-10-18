#pragma once
#include "StdAfx.h"
#include "AggregatorBaseComponent.h"
#include <Copal/CopalBus.h>

namespace Copal
{
	class EMAggregatorComponent
		:public Copal::AggregatorBaseComponent
	{
	public:
		AZ_COMPONENT(EMAggregatorComponent, "{0505C919-F878-41D5-873B-78C4D23D231E}");
		// AZ::Component interface implementation.

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);
		void Activate() override;
		void SetDensity(float density);
		void SetCharge(float charge);

		float GetDensity() { return ChargeDensity; }
		float GetCharge() { return Charge; }
		
	protected:
		CopalForceMap ForcesMap;
		CopalForceMap TorquesMap;

		float Charge = 0;
		float ChargeDensity = 0;

		bool UseDensity = false;
		bool UseCharge = true;

		//AZStd::vector < AZStd::string > HandledTags = AZStd::vector<AZStd::string>(1, "Electromagnetic");
	};
}