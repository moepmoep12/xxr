#pragma once

#include "../XCS/constants.h"

namespace XCSR
{

    struct Constants : XCS::Constants
    {
        // s_0
        double maxSpread;

        // m
        double mutationMaxChange;

        bool XCS::Constants::doActionSetSubsumption = false;
    };

}