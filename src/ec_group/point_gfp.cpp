#include <nil/crypto3/pubkey/ec_group/point_gfp.hpp>

namespace nil {
    namespace crypto3 {

// encoding and decoding
        std::vector<uint8_t> point_gfp::encode(point_gfp::compression_type format) const {
            if (is_zero()) {
                return std::vector<uint8_t>(1);
            } // single 0 byte

            const size_t p_bytes = m_curve.get_p().bytes();

            const cpp_int x = get_affine_x();
            const cpp_int y = get_affine_y();

            std::vector<uint8_t> result;

            if (format == point_gfp::UNCOMPRESSED) {
                result.resize(1 + 2 * p_bytes);
                result[0] = 0x04;
                cpp_int::encode_1363(&result[1], p_bytes, x);
                cpp_int::encode_1363(&result[1 + p_bytes], p_bytes, y);
            } else if (format == point_gfp::COMPRESSED) {
                result.resize(1 + p_bytes);
                result[0] = 0x02 | static_cast<uint8_t>(y.get_bit(0));
                cpp_int::encode_1363(&result[1], p_bytes, x);
            } else if (format == point_gfp::HYBRID) {
                result.resize(1 + 2 * p_bytes);
                result[0] = 0x06 | static_cast<uint8_t>(y.get_bit(0));
                cpp_int::encode_1363(&result[1], p_bytes, x);
                cpp_int::encode_1363(&result[1 + p_bytes], p_bytes, y);
            } else {
                throw std::invalid_argument("EC2OSP illegal point encoding");
            }

            return result;
        }

        namespace {

            cpp_int decompress_point(bool yMod2, const cpp_int &x, const cpp_int &curve_p, const cpp_int &curve_a,
                                     const cpp_int &curve_b) {
                cpp_int xpow3 = x * x * x;

                cpp_int g = curve_a * x;
                g += xpow3;
                g += curve_b;
                g = g % curve_p;

                cpp_int z = ressol(g, curve_p);

                if (z < 0) {
                    throw illegal_point("error during EC point decompression");
                }

                if (bit_test(z, 0) != yMod2) {
                    z = curve_p - z;
                }

                return z;
            }

        }

        point_gfp os2ecp(const uint8_t data[], size_t data_len, const curve_gfp &curve) {
            // Should we really be doing this?
            if (data_len <= 1) {
                return point_gfp(curve);
            } // return zero

            std::pair<cpp_int, cpp_int> xy = os2ecp(data, data_len, curve.get_p(), curve.get_a(), curve.get_b());

            point_gfp point(curve, xy.first, xy.second);

            if (!point.on_the_curve()) {
                throw illegal_point("os2ecp: Decoded point was not on the curve");
            }

            return point;
        }

        std::pair<cpp_int, cpp_int> os2ecp(const uint8_t data[], size_t data_len, const cpp_int &curve_p,
                                           const cpp_int &curve_a, const cpp_int &curve_b) {
            if (data_len <= 1) {
                throw decoding_error("os2ecp invalid point");
            }

            const uint8_t pc = data[0];

            cpp_int x, y;

            if (pc == 2 || pc == 3) {
                //compressed form
                x = cpp_int::decode(&data[1], data_len - 1);

                const bool y_mod_2 = ((pc & 0x01) == 1);
                y = decompress_point(y_mod_2, x, curve_p, curve_a, curve_b);
            } else if (pc == 4) {
                const size_t l = (data_len - 1) / 2;

                // uncompressed form
                x = cpp_int::decode(&data[1], l);
                y = cpp_int::decode(&data[l + 1], l);
            } else if (pc == 6 || pc == 7) {
                const size_t l = (data_len - 1) / 2;

                // hybrid form
                x = cpp_int::decode(&data[1], l);
                y = cpp_int::decode(&data[l + 1], l);

                const bool y_mod_2 = ((pc & 0x01) == 1);

                if (decompress_point(y_mod_2, x, curve_p, curve_a, curve_b) != y) {
                    throw illegal_point("os2ecp: Decoding error in hybrid format");
                }
            } else {
                throw std::invalid_argument("os2ecp: Unknown format type " + std::to_string(pc));
            }

            return std::make_pair(x, y);
        }
    }
}