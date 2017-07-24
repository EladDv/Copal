
#pragma once

#include <AzCore/EBus/EBus.h>

namespace Copal
{
    class CopalRequests
        : public AZ::EBusTraits
    {

    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions
    };
    using CopalRequestBus = AZ::EBus<CopalRequests>;
} // namespace Copal
