#ifndef KEY_MANAGEMENT_H
#define KEY_MANAGEMENT_H

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <iostream>

struct SchnorrKeyPair
{
    BIGNUM *priv_key; // private key x
    EC_POINT *pub_key; // public key P = xG
};

EC_GROUP *init_group();

SchnorrKeyPair key_gen(EC_GROUP *group, BN_CTX *ctx);

EC_POINT *aggregate_keys(const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx);

EC_POINT *key_sum(const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx);

EC_POINT *key_substitution_attack(const std::vector<SchnorrKeyPair> &keys, const SchnorrKeyPair &attacker, EC_GROUP *group, BN_CTX *ctx);


#endif // KEY_MANAGEMENT_H
