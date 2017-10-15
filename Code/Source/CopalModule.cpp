
#include "StdAfx.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "CopalSystemComponent.h"
#include "AggregatorBaseComponent.h"
#include "ForceBaseComponent.h"
#include "ConstantForceCopalComponent.h"
#include "SimpleSpringForceComponent.h"
#include "LocalGravityComponent.h"
#include "LocalConstantEMFieldComponent.h"
#include "HarmonicOscillatorComponent.h"
#include "EMAggregatorComponent.h"

#include <IGem.h>

namespace Copal
{
    class CopalModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(CopalModule, "{E43C31D5-1484-44A7-9D24-B2801D14C60F}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(CopalModule, AZ::SystemAllocator, 0);

        CopalModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
				CopalSystemComponent::CreateDescriptor(),
				ConstantForceComponent::CreateDescriptor(),
				AggregatorBaseComponent::CreateDescriptor(),
				SimpleSpringForceComponent::CreateDescriptor(),
				LocalGravityComponent::CreateDescriptor(),
				LocalConstantEMFieldComponent::CreateDescriptor(),
				EMAggregatorComponent::CreateDescriptor(),
				HarmonicOscillatorComponent::CreateDescriptor()
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
AZ_DECLARE_MODULE_CLASS(Copal_53423b5a4c29446cb2832e46d6846443, Copal::CopalModule)
