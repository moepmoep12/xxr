#pragma once

#include "../XCS/xcs_constants.h"

struct XCSRConstants : XCSConstants
{
    // s_0
    double maxSpread;

    // m
    double mutationMaxChange;

    bool XCSConstants::doActionSetSubsumption = false;
};