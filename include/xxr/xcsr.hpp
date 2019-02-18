#pragma once

#include "xcsr/repr.hpp"
#include "environment/real_multiplexer_environment.hpp"
#include "environment/checkerboard_environment.hpp"
#include "environment/csv_environment.hpp"

namespace xxr
{

    constexpr auto CSR = xcsr_impl::Repr::CSR;
    constexpr auto OBR = xcsr_impl::Repr::OBR;
    constexpr auto UBR = xcsr_impl::Repr::UBR;

    // XCSR Classifier System
    template <xcsr_impl::Repr R, typename T = double, typename Action = int>
    using XCSR = typename xcsr_impl::repr_to_experiment<R, T, Action>::type;

    using XCSRRepr = xcsr_impl::Repr;
    using XCSRConstants = xcs_impl::Constants;

}
