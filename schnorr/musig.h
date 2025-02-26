#ifndef MUSIG_H
#define MUSIG_H

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <vector>
#include "key_management.h"

struct MuSigSignature
{
    EC_POINT *R_agg; // aggregated commitment
    BIGNUM *s;       // aggregated signature
};


EC_POINT *musig_aggregate_keys(const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx);

MuSigSignature musig_sign(const std::string &message, const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx);

bool musig_verify(const std::string &message, const MuSigSignature &signature, EC_POINT *P_agg, EC_GROUP *group, BN_CTX *ctx);

#endif // MUSIG_H
