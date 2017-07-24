
#include "StdAfx.h"
#include <platform_impl.h>

#include "CopalSystemComponent.h"
#include "Components\CopalForceAggregatorComponent.h"
#include "Components\CopalForceSourceComponent.h"
#include "Components\SimpleOscillatorCopalComponent.h"
#include "Components\SimpleSpringCopalComponent.h"
#include "Components\LocalGravityCopalComponent.h"
#include "Components\ConstantEMFieldCopalComponent.h"
#include "Components\CopalChargedForceAggregatorComponent.h"

#include <IGem.h>

namespace Copal
{
    class CopalModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(CopalModule, "{DE98217F-37F6-47CA-AE4E-639533FDBEC2}", CryHooksModule);

        CopalModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                CopalSystemComponent::CreateDescriptor(),
				CopalForceAggregatorComponent::CreateDescriptor(),
				CopalForceSourceComponent::CreateDescriptor(),
				SimpleOscillatorCopalComponent::CreateDescriptor(),
				SimpleSpringCopalComponent::CreateDescriptor(),
				LocalGravityCopalComponent::CreateDescriptor(),
				ConstantEMFieldCopalComponent::CreateDescriptor(),
				CopalChargedForceAggregatorComponent::CreateDescriptor(),
			});
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<CopalSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(Copal_eb5eede2084941298c1ae62540c57e2f, Copal::CopalModule)
