#ifndef CRYPTO3_XMSS_SIGNATURE_HPP
#define CRYPTO3_XMSS_SIGNATURE_HPP

#include <cstddef>
#include <nil/crypto3/utilities/exceptions.hpp>
#include <nil/crypto3/utilities/types.hpp>
#include <nil/crypto3/utilities/secmem.hpp>

#include <nil/crypto3/pubkey/xmss/xmss_parameters.hpp>
#include <nil/crypto3/pubkey/xmss/xmss_wots_publickey.hpp>

namespace nil {
    namespace crypto3 {

        class XMSS_Signature final {
        public:
            /**
             * Creates a signature from an XMSS signature method and a uint8_t sequence
             * representing a raw signature.
             *
             * @param oid XMSS signature method
             * @param raw_sig An XMSS signature serialized using
             *                XMSS_Signature::bytes().
             **/
            XMSS_Signature(XMSS_Parameters::xmss_algorithm_t oid, const secure_vector<uint8_t> &raw_sig);

            /**
             * Creates an XMSS Signature from a leaf index used for signature
             * generation, a random value and a tree signature.
             *
             * @param leaf_idx Leaf index used to generate the signature.
             * @param randomness A random value.
             * @param tree_sig A tree signature.
             **/
            XMSS_Signature(size_t leaf_idx, const secure_vector<uint8_t> &randomness,
                           const XMSS_WOTS_PublicKey::TreeSignature &tree_sig) :
                m_leaf_idx(leaf_idx),
                m_randomness(randomness), m_tree_sig(tree_sig) {
            }

            /**
             * Creates an XMSS Signature from a leaf index used for signature
             * generation, a random value and a tree signature.
             *
             * @param leaf_idx Leaf index used to generate the signature.
             * @param randomness A random value.
             * @param tree_sig A tree signature.
             **/
            XMSS_Signature(size_t leaf_idx, secure_vector<uint8_t> &&randomness,
                           XMSS_WOTS_PublicKey::TreeSignature &&tree_sig) :
                m_leaf_idx(leaf_idx),
                m_randomness(std::move(randomness)), m_tree_sig(std::move(tree_sig)) {
            }

            size_t unused_leaf_index() const {
                return m_leaf_idx;
            }

            void set_unused_leaf_idx(size_t idx) {
                m_leaf_idx = idx;
            }

            const secure_vector<uint8_t> randomness() const {
                return m_randomness;
            }

            secure_vector<uint8_t> &randomness() {
                return m_randomness;
            }

            void set_randomness(const secure_vector<uint8_t> &randomness) {
                m_randomness = randomness;
            }

            void set_randomness(secure_vector<uint8_t> &&randomness) {
                m_randomness = std::move(randomness);
            }

            const XMSS_WOTS_PublicKey::TreeSignature &tree() const {
                return m_tree_sig;
            }

            XMSS_WOTS_PublicKey::TreeSignature &tree() {
                return m_tree_sig;
            }

            void set_tree(const XMSS_WOTS_PublicKey::TreeSignature &tree_sig) {
                m_tree_sig = tree_sig;
            }

            void set_tree(XMSS_WOTS_PublicKey::TreeSignature &&tree_sig) {
                m_tree_sig = std::move(tree_sig);
            }

            /**
             * Generates a serialized representation of XMSS Signature by
             * concatenating the following elements in order:
             * 8-byte leaf index, n-bytes randomness, ots_signature,
             * authentication path.
             *
             * n is the element_size(), len equal to len(), h the tree height
             * defined by the chosen XMSS signature method.
             *
             * @return serialized signature, a sequence of
             *         (len + h + 1)n bytes.
             **/
            secure_vector<uint8_t> bytes() const;

        private:
            size_t m_leaf_idx;
            secure_vector<uint8_t> m_randomness;
            XMSS_WOTS_PublicKey::TreeSignature m_tree_sig;
        };
    }    // namespace crypto3
}    // namespace nil

#endif