#ifndef CRYPTO3_CODE_BASED_UTIL_HPP
#define CRYPTO3_CODE_BASED_UTIL_HPP

#include <nil/crypto3/pubkey/mce/gf2m_small_m.hpp>

namespace nil {
    namespace crypto3 {

        /**
         * Expand an input to a bit mask depending on it being being zero or non-zero
         * @param tst the input
         * @return the mask 0xFFFF if tst is non-zero and 0 otherwise
         */
        template<typename T>
        uint16_t expand_mask_16bit(T tst) {
            const uint16_t result = (tst != 0);
            return ~(result - 1);
        }

        inline gf2m gray_to_lex(gf2m gray) {
            gf2m result = gray ^ (gray >> 8);
            result ^= (result >> 4);
            result ^= (result >> 2);
            result ^= (result >> 1);
            return result;
        }

        inline gf2m lex_to_gray(gf2m lex) {
            return (lex >> 1) ^ lex;
        }

        inline uint32_t bit_size_to_byte_size(uint32_t bit_size) {
            return (bit_size - 1) / 8 + 1;
        }

        inline uint32_t bit_size_to_32bit_size(uint32_t bit_size) {
            return (bit_size - 1) / 32 + 1;
        }
    }    // namespace crypto3
}    // namespace nil

#endif