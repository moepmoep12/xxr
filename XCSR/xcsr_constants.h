#pragma once

#include "../XCS/xcs_constants.h"

namespace xcsr
{

    struct XCSRConstants : xcs::XCSConstants
    {
        // s_0
        double maxSpread;

        // m
        double mutationMaxChange;

        bool xcs::XCSConstants::doActionSetSubsumption = false;
    };

}