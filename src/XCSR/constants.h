#pragma once

#include "../XCS/constants.h"

namespace XCSR
{

    struct Constants : XCS::Constants
    {
        double minValue = 0.0;

        double maxValue = 1.0;

        // s_0
        double coveringMaxSpread = 1.0;

        // m
        double mutationMaxChange = 0.1;

        // doRangeRestriction (ignored in XCSR_CS)
        //   Whether to restrict the range of the condition to the interval
        //   [min-value, max-value)
        bool doRangeRestriction = true;

        // doCoveringRandomRangeTruncation (ignored in XCSR_CS)
        //   Whether to truncate the covering random range before generating random
        //   intervals if the interval [x-s_0, x+s_0) is not contained in
        //   [min-value, max-value).
        //   "false" is common for this option, but the covering operator can
        //   generate too many maximum-range intervals if s_0 is larger than
        //   (max-value - min-value) / 2.
        //   Choose \"true\" to avoid the random bias in this situation.
        bool doCoveringRandomRangeTruncation = false;

        // Destructor
        virtual ~Constants() = default;
    };

}