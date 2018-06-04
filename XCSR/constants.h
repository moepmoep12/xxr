#pragma once

#include "../XCS/constants.h"

namespace XCSR
{

    struct Constants : XCS::Constants
    {
        double minValue = 0.0;

        double maxValue = 1.0;

        // s_0
        double maxSpread;

        // m
        double mutationMaxChange;

        bool XCS::Constants::doActionSetSubsumption = false;

        // Destructor
        virtual ~Constants() = default;
    };

}