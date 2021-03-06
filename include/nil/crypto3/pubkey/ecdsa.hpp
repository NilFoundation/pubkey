//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
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

#ifndef CRYPTO3_PUBKEY_ECDSA_KEY_HPP
#define CRYPTO3_PUBKEY_ECDSA_KEY_HPP

#include <nil/crypto3/multiprecision/number.hpp>

#include <nil/crypto3/pubkey/private_key.hpp>

#include <nil/crypto3/pubkey/detail/consistency.hpp>
#include <nil/crypto3/pubkey/detail/modes/rfc6979.hpp>

namespace nil {
    namespace crypto3 {
        namespace pubkey {
            namespace padding {
                template<typename Scheme, typename Hash>
                struct emsa1;
            }

            template<typename CurveType>
            struct ecdsa {
                typedef CurveType curve_type;

                typedef public_key<ecdsa<CurveType>> public_key_type;
                typedef private_key<ecdsa<CurveType>> private_key_type;

                template<typename Hash>
                using padding_types = std::tuple<padding::emsa1<ecdsa<CurveType>, Hash>>;
            };

            template<typename CurveType>
            struct public_key<ecdsa<CurveType>> {
                typedef CurveType curve_type;

                typedef typename curve_type::value_type value_type;
                typedef typename curve_type::number_type number_type;

                constexpr static const std::size_t key_bits = curve_type::field_type::modulus_bits;
                typedef typename curve_type::value_type key_type;

                constexpr static const std::size_t key_schedule_bits = curve_type::field_type::modulus_bits;
                typedef typename curve_type::value_type key_schedule_type;

                constexpr static const std::size_t signature_bits = curve_type::field_type::modulus_bits * 2;
                typedef std::tuple<value_type, value_type> signature_type;

                template<typename MsgRange>
                static inline bool verify(const public_key_type &pubkey, const MsgRange &message,
                                          const signature_type &signature) {
                    if (sig_len != m_group.get_order_bytes() * 2)
                        return false;

                    const BigInt e(msg, msg_len, m_group.get_order_bits());

                    const BigInt r(sig, sig_len / 2);
                    const BigInt s(sig + sig_len / 2, sig_len / 2);

                    if (r <= 0 || r >= m_group.get_order() || s <= 0 || s >= m_group.get_order())
                        return false;

                    const BigInt w = inverse_mod(s, m_group.get_order());

                    const BigInt u1 = m_group.multiply_mod_order(e, w);
                    const BigInt u2 = m_group.multiply_mod_order(r, w);
                    const PointGFp R = m_gy_mul.multi_exp(u1, u2);

                    if (R.is_zero())
                        return false;

                    const BigInt v = m_group.mod_order(R.get_affine_x());
                    return (v == r);
                }
            };

            template<typename CurveType>
            struct private_key<ecdsa<CurveType>> : public public_key<ecdsa<CurveType>> {
                typedef public_key<ecdsa<CurveType>> public_key_type;

                typedef typename public_key_type::curve_type curve_type;

                typedef typename curve_type::value_type value_type;
                typedef typename curve_type::number_type number_type;

                constexpr static const std::size_t key_bits = curve_type::field_type::modulus_bits;
                typedef typename CurveType::value_type key_type;

                constexpr static const std::size_t key_schedule_bits = curve_type::field_type::modulus_bits;
                typedef typename CurveType::value_type key_schedule_type;

                constexpr static const std::size_t signature_bits = curve_type::field_type::modulus_bits * 2;
                typedef std::tuple<value_type, value_type> signature_type;

                template<typename NonceGenerator>
                inline static bool sign(signature_type &res, const number_type &val, const key_schedule_type &key) {
                    BigInt m(msg, msg_len, m_group.get_order_bits());

                    const BigInt k = NonceGenerator()(m_x, m_group.get_order(), m);

                    const BigInt k_inv = inverse_mod(k, m_group.get_order());
                    const BigInt r = m_group.mod_order(m_group.blinded_base_point_multiply_x(k, rng, m_ws));

                    const BigInt xrm = m_group.mod_order(m_group.multiply_mod_order(m_x, r) + m);
                    const BigInt s = m_group.multiply_mod_order(k_inv, xrm);

                    // With overwhelming probability, a bug rather than actual zero r/s
                    if (r.is_zero() || s.is_zero())
                        throw Internal_Error("During ECDSA signature generated zero r/s");

                    //                    return BigInt::encode_fixed_length_int_pair(r, s, m_group.get_order_bytes());
                    res = std::make_tuple(r, s);
                }
            };
        }    // namespace pubkey
    }        // namespace crypto3
}    // namespace nil

#endif
