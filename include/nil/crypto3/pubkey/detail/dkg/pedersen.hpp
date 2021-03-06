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

#ifndef CRYPTO3_PUBKEY_PEDERSEN_DKG_HPP
#define CRYPTO3_PUBKEY_PEDERSEN_DKG_HPP

#include <numeric>

#include <nil/crypto3/pubkey/detail/secret_sharing/feldman.hpp>

namespace nil {
    namespace crypto3 {
        namespace pubkey {
            namespace detail {
                //
                // "A threshold cryptosystem without a trusted party" by Torben Pryds Pedersen.
                // https://dl.acm.org/citation.cfm?id=1754929
                //
                template<typename Group>
                struct pedersen_dkg : feldman_sss<Group> {
                    typedef feldman_sss<Group> base_type;

                    typedef typename base_type::private_element_type private_element_type;
                    typedef typename base_type::public_element_type public_element_type;

                    typedef typename base_type::share_type share_type;

                    template<typename Shares,
                             typename base_type::template check_indexed_private_element_type<
                                 typename std::iterator_traits<typename Shares::iterator>::value_type> = true>
                    static inline share_type deal_share(const Shares &shares) {
                        BOOST_RANGE_CONCEPT_ASSERT((boost::SinglePassRangeConcept<const Shares>));
                        return deal_share(shares.begin(), shares.end());
                    }

                    template<typename SharesIterator,
                             typename base_type::template check_indexed_private_element_type<
                                 typename std::iterator_traits<SharesIterator>::value_type> = true>
                    static inline share_type deal_share(SharesIterator first, SharesIterator last) {
                        BOOST_CONCEPT_ASSERT((boost::InputIteratorConcept<SharesIterator>));
                        auto n = std::distance(first, last);
                        assert(base_type::check_minimal_size(n));
                        auto index = first->first;
                        assert(base_type::check_participant_index(index, n));

                        private_element_type share = private_element_type::zero();
                        for (auto it = first; it != last; it++) {
                            assert(index == it->first);
                            share = share + it->second;
                        }
                        return share_type(index, share);
                    }
                };
            }    // namespace detail
        }        // namespace pubkey
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_PUBKEY_PEDERSEN_DKG_HPP
