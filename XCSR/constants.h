#pragma once

#include "../XCS/constants.h"

namespace xcsr
{

    struct Constants : xcs::Constants
    {
        // s_0
        double maxSpread;

        // m
        double mutationMaxChange;

        bool xcs::Constants::doActionSetSubsumption = false;
    };

}