#include "musig.h"
#include <openssl/sha.h>
#include <iostream>


BIGNUM *compute_lambda(const std::vector<SchnorrKeyPair> &keys, const SchnorrKeyPair &key, EC_GROUP *group, BN_CTX *ctx)
{
    std::string data;
    for (const auto &k : keys)
    {
        char *hex = EC_POINT_point2hex(group, k.pub_key, POINT_CONVERSION_COMPRESSED, ctx);
        data += std::string(hex);
        OPENSSL_free(hex);
    }
    char *key_hex = EC_POINT_point2hex(group, key.pub_key, POINT_CONVERSION_COMPRESSED, ctx);
    data += std::string(key_hex);
    OPENSSL_free(key_hex);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);

    BIGNUM *lambda = BN_new();
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, lambda);
    return lambda;
}

EC_POINT *musig_aggregate_keys(const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx)
{
    EC_POINT *P_agg = EC_POINT_new(group);
    EC_POINT_set_to_infinity(group, P_agg);

    for (const auto &key : keys)
    {
        BIGNUM *lambda = compute_lambda(keys, key, group, ctx);
        EC_POINT *weighted_P = EC_POINT_new(group);
        EC_POINT_mul(group, weighted_P, nullptr, key.pub_key, lambda, ctx);
        EC_POINT_add(group, P_agg, P_agg, weighted_P, ctx);
        EC_POINT_free(weighted_P);
        BN_free(lambda);
    }
    return P_agg;
}

MuSigSignature musig_sign(const std::string &message, const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx)
{
    MuSigSignature signature = {EC_POINT_new(group), BN_new()};
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

    std::vector<SchnorrKeyPair> commitments_as_keys;
    for (size_t i = 0; i < commitments.size(); ++i)
        commitments_as_keys.push_back({nullptr, commitments[i]});

    signature.R_agg = key_sum(commitments_as_keys, group, ctx);

    char *R_hex = EC_POINT_point2hex(group, signature.R_agg, POINT_CONVERSION_COMPRESSED, ctx);
    char *P_hex = EC_POINT_point2hex(group, musig_aggregate_keys(keys, group, ctx), POINT_CONVERSION_COMPRESSED, ctx);
    std::string data = std::string(R_hex) + std::string(P_hex) + message;
    OPENSSL_free(R_hex);
    OPENSSL_free(P_hex);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);
    BIGNUM *e = BN_new();
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, e);

    BN_zero(signature.s);
    for (size_t i = 0; i < keys.size(); ++i)
    {
        BIGNUM *lambda = compute_lambda(keys, keys[i], group, ctx);
        BIGNUM *s_i = BN_new();
        BIGNUM *ex_i = BN_new();

        BN_mod_mul(ex_i, e, lambda, order, ctx);
        BN_mod_mul(ex_i, ex_i, keys[i].priv_key, order, ctx);
        BN_mod_add(s_i, nonces[i], ex_i, order, ctx);
        BN_mod_add(signature.s, signature.s, s_i, order, ctx);

        BN_free(lambda);
        BN_free(s_i);
        BN_free(ex_i);
    }

    BN_free(order);
    BN_free(e);
    for (auto nonce : nonces) BN_free(nonce);
    for (auto R : commitments) EC_POINT_free(R);

    return signature;
}

bool musig_verify(const std::string &message, const MuSigSignature &signature, EC_POINT *P_agg, EC_GROUP *group, BN_CTX *ctx)
{
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(group, order, ctx);

    char *R_hex = EC_POINT_point2hex(group, signature.R_agg, POINT_CONVERSION_COMPRESSED, ctx);
    char *P_hex = EC_POINT_point2hex(group, P_agg, POINT_CONVERSION_COMPRESSED, ctx);
    std::string data = std::string(R_hex) + std::string(P_hex) + message;
    OPENSSL_free(R_hex);
    OPENSSL_free(P_hex);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);
    BIGNUM *e = BN_new();
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, e);

    EC_POINT *expected_R = EC_POINT_new(group);
    EC_POINT_mul(group, expected_R, signature.s, nullptr, nullptr, ctx);

    EC_POINT *eP = EC_POINT_new(group);
    EC_POINT_mul(group, eP, nullptr, P_agg, e, ctx);
    EC_POINT_invert(group, eP, ctx);
    EC_POINT_add(group, expected_R, expected_R, eP, ctx);

    bool valid = EC_POINT_cmp(group, expected_R, signature.R_agg, ctx) == 0;

    BN_free(order);
    BN_free(e);
    EC_POINT_free(expected_R);
    EC_POINT_free(eP);

    return valid;
}
