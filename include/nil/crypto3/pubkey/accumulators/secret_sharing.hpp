//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
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

#ifndef CRYPTO3_ACCUMULATORS_PUBKEY_SSS_HPP
#define CRYPTO3_ACCUMULATORS_PUBKEY_SSS_HPP

#include <set>
#include <utility>
#include <algorithm>
#include <iterator>

#include <boost/concept_check.hpp>

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>

#include <nil/crypto3/pubkey/accumulators/parameters/threshold_value.hpp>

namespace nil {
    namespace crypto3 {
        namespace accumulators {
            namespace impl {
                template<typename Scheme>
                struct deal_shares_impl : boost::accumulators::accumulator_base {
                protected:
                    typedef Scheme scheme_type;

                    typedef typename scheme_type::private_elements_type private_elements_type;
                    typedef typename scheme_type::private_element_type private_element_type;

                public:
                    typedef private_elements_type result_type;

                    template<typename Args>
                    deal_shares_impl(const Args &args) : seen_coeffs(0) {
                        assert(scheme_type::check_minimal_size(args[boost::accumulators::sample]));
                        n = args[boost::accumulators::sample];
                        std::fill_n(std::back_inserter(shares), n, private_element_type::zero());
                    }

                    inline result_type result(boost::accumulators::dont_care) const {
                        assert(scheme_type::check_t(seen_coeffs, n));
                        return shares;
                    }

                    //
                    // input coefficients should be supplied in increasing term degrees order
                    //
                    template<typename Args>
                    inline void operator()(const Args &args) {
                        resolve_type(args[boost::accumulators::sample]);
                    }

                protected:
                    inline void resolve_type(const private_element_type &coeff) {
                        auto shares_it = shares.begin();
                        std::size_t i = 1;
                        private_element_type e_i = private_element_type::one();
                        while (shares_it != shares.end()) {
                            *shares_it = scheme_type::eval_partial_private_element(coeff, i++, seen_coeffs, *shares_it);
                            ++shares_it;
                        }
                        seen_coeffs++;
                    }

                    std::size_t n;
                    std::size_t seen_coeffs;
                    result_type shares;
                };

                template<typename Scheme>
                struct deal_indexed_shares_impl : boost::accumulators::accumulator_base {
                protected:
                    typedef Scheme scheme_type;

                    typedef typename scheme_type::private_element_type private_element_type;
                    typedef typename scheme_type::indexed_private_element_type indexed_private_element_type;
                    typedef typename scheme_type::indexed_private_elements_type indexed_private_elements_type;

                public:
                    typedef indexed_private_elements_type result_type;

                    template<typename Args>
                    deal_indexed_shares_impl(const Args &args) : seen_coeffs(0) {
                        assert(scheme_type::check_minimal_size(args[boost::accumulators::sample]));
                        n = args[boost::accumulators::sample];
                        std::size_t i = 1;
                        std::generate_n(std::inserter(indexed_shares, indexed_shares.end()), n, [&i]() {
                            return indexed_private_element_type(i++, private_element_type::zero());
                        });
                    }

                    inline result_type result(boost::accumulators::dont_care) const {
                        assert(scheme_type::check_t(seen_coeffs, n));
                        return indexed_shares;
                    }

                    //
                    // input coefficients should be supplied in increasing term degrees order
                    //
                    template<typename Args>
                    inline void operator()(const Args &args) {
                        resolve_type(args[boost::accumulators::sample]);
                    }

                protected:
                    inline void resolve_type(const private_element_type &coeff) {
                        auto indexed_shares_it = indexed_shares.begin();
                        private_element_type e_i = private_element_type::one();
                        while (indexed_shares_it != indexed_shares.end()) {
                            indexed_shares_it->second = scheme_type::eval_partial_private_element(
                                coeff, indexed_shares_it->first, seen_coeffs, indexed_shares_it->second);
                            indexed_shares_it++;
                        }
                        seen_coeffs++;
                    }

                    std::size_t n;
                    std::size_t seen_coeffs;
                    result_type indexed_shares;
                };

                template<typename Scheme>
                struct deal_indexed_weighted_shares_impl : boost::accumulators::accumulator_base {
                protected:
                    typedef Scheme scheme_type;

                    typedef typename scheme_type::private_element_type private_element_type;
                    typedef typename scheme_type::private_elements_type private_elements_type;
                    typedef typename scheme_type::weights_type weights_type;
                    typedef typename scheme_type::indexed_private_element_type indexed_private_element_type;
                    typedef typename scheme_type::indexed_private_elements_type indexed_private_elements_type;
                    typedef typename scheme_type::indexed_weighted_private_elements_type
                        indexed_weighted_private_elements_type;

                public:
                    typedef std::pair<private_elements_type, indexed_weighted_private_elements_type> result_type;

                    template<typename Args>
                    deal_indexed_weighted_shares_impl(const Args &args) {
                        assert(scheme_type::check_t(args[nil::crypto3::accumulators::threshold_value],
                                                    args[boost::accumulators::sample]));
                        t = args[nil::crypto3::accumulators::threshold_value];
                        n = args[boost::accumulators::sample];
                        //
                        // default generation of polynomial coefficients
                        //
                        coeffs = scheme_type::get_poly(t, n);
                        std::size_t i = 1;
                        std::fill_n(std::back_inserter(shares_weights), n, 1);
                    }

                    inline result_type result(boost::accumulators::dont_care) const {
                        return std::make_pair(coeffs,
                                              scheme_type::deal_indexed_weighted_shares(coeffs, shares_weights));
                    }

                    template<typename Args>
                    inline void operator()(const Args &args) {
                        resolve_type(args[boost::accumulators::sample]);
                    }

                protected:
                    //
                    // custom generated polynomial coefficients
                    //
                    template<typename IndexedCoeff,
                             typename scheme_type::template check_indexed_private_element_type<IndexedCoeff> = true>
                    inline void resolve_type(const IndexedCoeff &indexed_coeff) {
                        assert(0 <= indexed_coeff.first && indexed_coeff.first < t);
                        *(coeffs.begin() + indexed_coeff.first) = indexed_coeff.second;
                    }

                    template<typename IndexedWeight,
                             typename scheme_type::template check_indexed_weight_type<IndexedWeight> = true>
                    inline void resolve_type(const IndexedWeight &w_i) {
                        assert(0 < w_i.first && w_i.first <= n);
                        assert(0 < w_i.second);
                        *(shares_weights.begin() + w_i.first - 1) = w_i.second;
                    }

                    std::size_t t;
                    std::size_t n;
                    private_elements_type coeffs;
                    weights_type shares_weights;
                };

                template<typename Scheme>
                struct recover_private_element_impl : boost::accumulators::accumulator_base {
                protected:
                    typedef Scheme scheme_type;

                    typedef typename scheme_type::private_element_type private_element_type;
                    typedef typename scheme_type::indexed_private_element_type indexed_private_element_type;
                    typedef typename scheme_type::indexed_private_elements_type indexed_private_elements_type;

                public:
                    typedef private_element_type result_type;

                    template<typename Args>
                    recover_private_element_impl(const Args &args) : seen_shares(0) {
                    }

                    inline result_type result(boost::accumulators::dont_care) const {
                        assert(scheme_type::check_minimal_size(seen_shares));
                        return scheme_type::recover_private_element(indexed_shares);
                    }

                    template<typename Args>
                    inline void operator()(const Args &args) {
                        resolve_type(args[boost::accumulators::sample]);
                    }

                protected:
                    inline void resolve_type(const indexed_private_element_type &indexed_share) {
                        assert(indexed_shares.emplace(indexed_share).second);
                        seen_shares++;
                    }

                    std::size_t seen_shares;
                    indexed_private_elements_type indexed_shares;
                };

                template<typename Scheme>
                struct verify_share_impl : boost::accumulators::accumulator_base {
                protected:
                    typedef Scheme scheme_type;

                    typedef typename scheme_type::public_element_type public_element_type;
                    typedef typename scheme_type::indexed_public_element_type indexed_public_element_type;

                public:
                    typedef bool result_type;

                    template<typename Args>
                    verify_share_impl(const Args &args) :
                        gs_i(args[boost::accumulators::sample]), seen_coeffs(0),
                        share_verification_value(public_element_type::zero()) {
                    }

                    //
                    // input coefficients should be supplied in increasing term degrees order
                    //
                    template<typename Args>
                    inline void operator()(const Args &args) {
                        resolve_type(args[boost::accumulators::sample]);
                    }

                    inline result_type result(boost::accumulators::dont_care) const {
                        return gs_i.second == share_verification_value;
                    }

                protected:
                    inline void resolve_type(const public_element_type &public_coeff) {
                        share_verification_value = scheme_type::eval_partial_verification_value(
                            public_coeff, gs_i.first, seen_coeffs, share_verification_value);
                        seen_coeffs++;
                    }

                    std::size_t seen_coeffs;
                    indexed_public_element_type gs_i;
                    public_element_type share_verification_value;
                };
            }    // namespace impl

            namespace tag {
                template<typename Scheme>
                struct deal_shares : boost::accumulators::depends_on<> {
                    typedef Scheme scheme_type;

                    /// INTERNAL ONLY
                    ///

                    typedef boost::mpl::always<accumulators::impl::deal_shares_impl<scheme_type>> impl;
                };

                template<typename Scheme>
                struct deal_indexed_shares : boost::accumulators::depends_on<> {
                    typedef Scheme scheme_type;

                    /// INTERNAL ONLY
                    ///

                    typedef boost::mpl::always<accumulators::impl::deal_indexed_shares_impl<scheme_type>> impl;
                };

                template<typename Scheme>
                struct deal_indexed_weighted_shares : boost::accumulators::depends_on<> {
                    typedef Scheme scheme_type;

                    /// INTERNAL ONLY
                    ///

                    typedef boost::mpl::always<accumulators::impl::deal_indexed_weighted_shares_impl<scheme_type>> impl;
                };

                template<typename Scheme>
                struct verify_share : boost::accumulators::depends_on<> {
                    typedef Scheme scheme_type;

                    /// INTERNAL ONLY
                    ///

                    typedef boost::mpl::always<accumulators::impl::verify_share_impl<scheme_type>> impl;
                };

                template<typename Scheme>
                struct recover_private_element : boost::accumulators::depends_on<> {
                    typedef Scheme scheme_type;

                    /// INTERNAL ONLY
                    ///

                    typedef boost::mpl::always<accumulators::impl::recover_private_element_impl<scheme_type>> impl;
                };
            }    // namespace tag

            namespace extract {
                template<typename Scheme, typename AccumulatorSet>
                typename boost::mpl::apply<AccumulatorSet, tag::deal_shares<Scheme>>::type::result_type
                    scheme(const AccumulatorSet &acc) {
                    return boost::accumulators::extract_result<tag::deal_shares<Scheme>>(acc);
                }

                template<typename Scheme, typename AccumulatorSet>
                typename boost::mpl::apply<AccumulatorSet, tag::deal_indexed_shares<Scheme>>::type::result_type
                    scheme(const AccumulatorSet &acc) {
                    return boost::accumulators::extract_result<tag::deal_indexed_shares<Scheme>>(acc);
                }

                template<typename Scheme, typename AccumulatorSet>
                typename boost::mpl::apply<AccumulatorSet, tag::deal_indexed_weighted_shares<Scheme>>::type::result_type
                    scheme(const AccumulatorSet &acc) {
                    return boost::accumulators::extract_result<tag::deal_indexed_weighted_shares<Scheme>>(acc);
                }

                template<typename Scheme, typename AccumulatorSet>
                typename boost::mpl::apply<AccumulatorSet, tag::verify_share<Scheme>>::type::result_type
                    scheme(const AccumulatorSet &acc) {
                    return boost::accumulators::extract_result<tag::verify_share<Scheme>>(acc);
                }

                template<typename Scheme, typename AccumulatorSet>
                typename boost::mpl::apply<AccumulatorSet, tag::recover_private_element<Scheme>>::type::result_type
                    scheme(const AccumulatorSet &acc) {
                    return boost::accumulators::extract_result<tag::recover_private_element<Scheme>>(acc);
                }
            }    // namespace extract
        }        // namespace accumulators
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ACCUMULATORS_PUBKEY_SSS_HPP