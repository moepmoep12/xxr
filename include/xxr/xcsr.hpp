#pragma once

#include "xcsr/repr.hpp"
#include "environment/real_multiplexer_environment.hpp"
#include "environment/checkerboard_environment.hpp"
#include "environment/csv_environment.hpp"

namespace xxr
{

    // XCSR Classifier System
    template <xcsr_impl::Repr R, typename T = int, typename Action = int>
    using XCSR = typename xcsr_impl::repr_to_experiment<R, T, Action>::type;

}
