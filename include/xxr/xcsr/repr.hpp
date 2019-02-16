#pragma once

#include "csr/experiment.hpp"
#include "obr/experiment.hpp"
#include "ubr/experiment.hpp"

namespace xxr { namespace xcsr_impl
{

    // XCSR representations
    enum class Repr
    {
        CSR,
        OBR,
        UBR,
    };

    // Type traits for XCSR representations
    template <Repr R, typename T, typename Action>
    struct repr_to_experiment;

    template<typename T, typename Action>
    struct repr_to_experiment<Repr::CSR, T, Action>
    {
        using type = csr::Experiment<T, Action>;
    };

    template<typename T, typename Action>
    struct repr_to_experiment<Repr::OBR, T, Action>
    {
        using type = obr::Experiment<T, Action>;
    };

    template<typename T, typename Action>
    struct repr_to_experiment<Repr::UBR, T, Action>
    {
        using type = ubr::Experiment<T, Action>;
    };

}}
