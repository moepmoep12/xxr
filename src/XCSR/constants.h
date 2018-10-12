#pragma once

#include "../XCS/constants.h"

namespace XCSR
{

    struct Constants : XCS::Constants
    {
        double minValue = 0.0;

        double maxValue = 1.0;

        // s_0
        double maxSpread = 1.0;

        // m
        double mutationMaxChange = 0.1;

        // Destructor
        virtual ~Constants() = default;
    };

}