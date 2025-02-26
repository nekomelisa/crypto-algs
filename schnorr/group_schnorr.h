#ifndef GROUP_SCHNORR_H
#define GROUP_SCHNORR_H

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <vector>
#include "key_management.h"


struct GroupSchnorrSignature
{
    EC_POINT *R_agg; // aggregated commitment
    BIGNUM *s_agg;   // aggregated signature
};

GroupSchnorrSignature naive_group_sign(const std::string &message, const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx);

#endif // GROUP_SCHNORR_H
