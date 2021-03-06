//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020 Ilias Khairullin <ilias@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_PUBKEY_SCHEME_STATE_HPP
#define CRYPTO3_PUBKEY_SCHEME_STATE_HPP

#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/framework/features.hpp>

#include <nil/crypto3/pubkey/accumulators/sign.hpp>
#include <nil/crypto3/pubkey/accumulators/verify.hpp>
#include <nil/crypto3/pubkey/accumulators/aggregate.hpp>
#include <nil/crypto3/pubkey/accumulators/aggregate_verify.hpp>
#include <nil/crypto3/pubkey/secret_sharing_scheme_state.hpp>

namespace nil {
    namespace crypto3 {
        namespace pubkey {
            /*!
             * @brief Accumulator set with pre-defined signing accumulator params.
             *
             * Meets the requirements of AccumulatorSet
             *
             * @ingroup pubkey
             *
             * @tparam Mode Scheme processing mode type (e.g. isomorphic_signing_policy<bls, nop_padding>)
             */
            template<typename Mode>
            using signing_accumulator_set =
                boost::accumulators::accumulator_set<typename Mode::result_type,
                                                     boost::accumulators::features<accumulators::tag::sign<Mode>>>;

            /*!
             * @brief Accumulator set with pre-defined verification accumulator params.
             *
             * Meets the requirements of AccumulatorSet
             *
             * @ingroup pubkey
             *
             * @tparam Mode Scheme processing mode type (e.g. isomorphic_verification_policy<bls, nop_padding>)
             */
            template<typename Mode>
            using verification_accumulator_set =
                boost::accumulators::accumulator_set<typename Mode::result_type,
                                                     boost::accumulators::features<accumulators::tag::verify<Mode>>>;

            /*!
             * @brief Accumulator set with pre-defined aggregation accumulator params.
             *
             * Meets the requirements of AccumulatorSet
             *
             * @ingroup pubkey
             *
             * @tparam Mode Scheme processing mode type (e.g. isomorphic_aggregation_policy<bls, nop_padding>)
             */
            template<typename Mode>
            using aggregation_accumulator_set =
                boost::accumulators::accumulator_set<typename Mode::result_type,
                                                     boost::accumulators::features<accumulators::tag::aggregate<Mode>>>;

            /*!
             * @brief Accumulator set with pre-defined aggregation accumulator params.
             *
             * Meets the requirements of AccumulatorSet
             *
             * @ingroup pubkey
             *
             * @tparam Mode Scheme processing mode type (e.g. isomorphic_aggregation_policy<bls, nop_padding>)
             */
            template<typename Mode>
            using aggregated_verification_accumulator_set = boost::accumulators::accumulator_set<
                typename Mode::result_type, boost::accumulators::features<accumulators::tag::aggregate_verify<Mode>>>;
        }    // namespace pubkey
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_PUBKEY_SCHEME_STATE_HPP
