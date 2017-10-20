
#pragma once

#include <AzCore/Component/Component.h>
#include <Copal/CopalBus.h>
#include <AzFramework\TargetManagement\TargetManagementComponent.h>

namespace Copal
{
    class CopalSystemComponent
        : public AZ::Component

        //, protected CopalPhysicsRequestsBus::Handler
    {
    public:
        AZ_COMPONENT(CopalSystemComponent, "{3579E77B-8A22-41A4-A417-9DBC05CE35D6}");

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
