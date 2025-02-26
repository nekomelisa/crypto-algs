#include "schnorr.h"
#include "key_management.h"
#include <iostream>

SchnorrSignature sign(const std::string &message, const SchnorrKeyPair &keys, EC_GROUP *group, BN_CTX *ctx)
{
    SchnorrSignature signature;
    signature.R = EC_POINT_new(group);
    signature.s = BN_new();

    BIGNUM *k = BN_new();
    BIGNUM *e = BN_new();
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(group, order, ctx);

    BN_rand_range(k, order);

    EC_POINT_mul(group, signature.R, k, nullptr, nullptr, ctx);

    char *R_hex = EC_POINT_point2hex(group, signature.R, POINT_CONVERSION_COMPRESSED, ctx);

    std::string data = std::string(R_hex) + message;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);

    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, e);

    BN_mod_mul(e, e, keys.priv_key, order, ctx); // e * x
    BN_mod_sub(signature.s, k, e, order, ctx);   // k - ex mod n

    OPENSSL_free(R_hex);
    BN_free(k);
    BN_free(e);
    BN_free(order);

    return signature;
}

bool verify(const std::string &message, const SchnorrSignature &signature, const SchnorrKeyPair &keys, EC_GROUP *group, BN_CTX *ctx)
{
    BIGNUM *e = BN_new();
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(group, order, ctx);

    char *R_hex = EC_POINT_point2hex(group, signature.R, POINT_CONVERSION_COMPRESSED, ctx);

    std::string data = std::string(R_hex) + message;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);

    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, e);

    EC_POINT *R_prime = EC_POINT_new(group);
    EC_POINT *eP = EC_POINT_new(group);

    EC_POINT_mul(group, eP, nullptr, keys.pub_key, e, ctx); // eP = e * P
    EC_POINT_mul(group, R_prime, signature.s, nullptr, nullptr, ctx); // sG = s * G
    EC_POINT_add(group, R_prime, R_prime, eP, ctx); // R' = sG + eP

    char *R_prime_hex = EC_POINT_point2hex(group, R_prime, POINT_CONVERSION_COMPRESSED, ctx);
    bool is_valid = (std::string(R_prime_hex) == std::string(R_hex));

    OPENSSL_free(R_hex);
    OPENSSL_free(R_prime_hex);
    BN_free(e);
    BN_free(order);
    EC_POINT_free(R_prime);
    EC_POINT_free(eP);

    return is_valid;
}
