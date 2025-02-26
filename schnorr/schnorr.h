#ifndef SCHNORR_H
#define SCHNORR_H

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <string>
#include "key_management.h"


struct SchnorrSignature
{
    EC_POINT *R; // commitment point
    BIGNUM *s;   // signature
};

SchnorrSignature sign(const std::string &message, const SchnorrKeyPair &keys, EC_GROUP *group, BN_CTX *ctx);


bool verify(const std::string &message, const SchnorrSignature &signature, const SchnorrKeyPair &keys, EC_GROUP *group, BN_CTX *ctx);

#endif // SCHNORR_H
