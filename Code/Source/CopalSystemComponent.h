
#pragma once

#include <AzCore/Component/Component.h>

#include <Copal/CopalBus.h>

namespace Copal
{
    class CopalSystemComponent
        : public AZ::Component
        , protected CopalRequestBus::Handler
    {
    public:
        AZ_COMPONENT(CopalSystemComponent, "{CB1CB713-F4D1-4FED-9710-46F8D7DA1D28}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // CopalRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
