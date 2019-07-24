#ifndef CRYPTO3_WORKFACTOR_HPP
#define CRYPTO3_WORKFACTOR_HPP

#include <nil/crypto3/utilities/types.hpp>

namespace nil {
    namespace crypto3 {

        /**
         * Estimate work factor for discrete logarithm
         * @param prime_group_size size of the group in bits
         * @return estimated security level for this group
         */

        size_t dl_work_factor(size_t prime_group_size);

        /**
         * Return the appropriate exponent size to use for a particular prime
         * group. This is twice the size of the estimated cost of breaking the
         * key using an index calculus attack; the assumption is that if an
         * arbitrary discrete log on a group of size bits would take about 2^n
         * effort, and thus using an exponent of size 2^(2*n) implies that all
         * available attacks are about as easy (as e.g Pollard's kangaroo
         * algorithm can compute the DL in sqrt(x) operations) while minimizing
         * the exponent size for performance reasons.
         */

        size_t dl_exponent_size(size_t prime_group_size);

        /**
         * Estimate work factor for integer factorization
         * @param n_bits size of modulus in bits
         * @return estimated security level for this modulus
         */

        size_t if_work_factor(size_t n_bits);

        /**
         * Estimate work factor for EC discrete logarithm
         * @param prime_group_size size of the group in bits
         * @return estimated security level for this group
         */

        size_t ecp_work_factor(size_t prime_group_size);
    }    // namespace crypto3
}    // namespace nil

#endif