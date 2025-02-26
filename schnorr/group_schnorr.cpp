#include "group_schnorr.h"

GroupSchnorrSignature naive_group_sign(const std::string &message, const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx)
{
    GroupSchnorrSignature signature;
    signature.R_agg = EC_POINT_new(group);
    signature.s_agg = BN_new();

    BIGNUM *order = BN_new();
    EC_GROUP_get_order(group, order, ctx);

    std::vector<BIGNUM *> nonces(keys.size());
    std::vector<EC_POINT *> commitments(keys.size());

    for (size_t i = 0; i < keys.size(); ++i)
    {
        nonces[i] = BN_new();
        commitments[i] = EC_POINT_new(group);

        BN_rand_range(nonces[i], order);
        EC_POINT_mul(group, commitments[i], nonces[i], nullptr, nullptr, ctx);
    }

    EC_POINT_set_to_infinity(group, signature.R_agg);
    for (const auto &R : commitments)
    {
        EC_POINT_add(group, signature.R_agg, signature.R_agg, R, ctx);
    }

    char *R_hex = EC_POINT_point2hex(group, signature.R_agg, POINT_CONVERSION_COMPRESSED, ctx);

    std::string data = std::string(R_hex) + message;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);

    BIGNUM *e = BN_new();
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, e);

    BN_zero(signature.s_agg);
    for (size_t i = 0; i < keys.size(); ++i)
    {
        BIGNUM *s_i = BN_new();
        BIGNUM *ex_i = BN_new();

        BN_mod_mul(ex_i, e, keys[i].priv_key, order, ctx); // e * x_i
        BN_mod_sub(s_i, nonces[i], ex_i, order, ctx); // s_i = k_i - e * x_i mod n

        BN_mod_add(signature.s_agg, signature.s_agg, s_i, order, ctx); // s_agg += s_i

        BN_free(s_i);
        BN_free(ex_i);
    }

    OPENSSL_free(R_hex);
    BN_free(order);
    BN_free(e);
    for (auto nonce : nonces) BN_free(nonce);
    for (auto R : commitments) EC_POINT_free(R);

    return signature;
}


