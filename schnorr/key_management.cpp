#include "key_management.h"

EC_GROUP *init_group()
{
    return EC_GROUP_new_by_curve_name(NID_secp256k1);
}

SchnorrKeyPair key_gen(EC_GROUP *group, BN_CTX *ctx)
{
    SchnorrKeyPair keys;
    keys.priv_key = BN_new();
    keys.pub_key = EC_POINT_new(group);

    BN_rand_range(keys.priv_key, EC_GROUP_get0_order(group));

    EC_POINT_mul(group, keys.pub_key, keys.priv_key, nullptr, nullptr, ctx);

    return keys;
}

EC_POINT *aggregate_keys(const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx)
{
    EC_POINT *P_agg = EC_POINT_new(group);
    EC_POINT_set_to_infinity(group, P_agg);

    for (const auto &key : keys)
    {
        EC_POINT_add(group, P_agg, P_agg, key.pub_key, ctx);
    }

    return P_agg;
}

EC_POINT *key_sum(const std::vector<SchnorrKeyPair> &keys, EC_GROUP *group, BN_CTX *ctx)
{
    EC_POINT *sum = EC_POINT_new(group);
    EC_POINT_set_to_infinity(group, sum);

    for (const auto &key : keys)
    {
        EC_POINT_add(group, sum, sum, key.pub_key, ctx);
    }

    return sum;
}

EC_POINT *key_substitution_attack(const std::vector<SchnorrKeyPair> &keys, const SchnorrKeyPair &attacker, EC_GROUP *group, BN_CTX *ctx)
{
    EC_POINT *sum = key_sum(keys, group, ctx);
    EC_POINT *double_attacker = EC_POINT_new(group);
    EC_POINT *P_fake = EC_POINT_new(group);

    EC_POINT_dbl(group, double_attacker, attacker.pub_key, ctx);

    EC_POINT_invert(group, double_attacker, ctx);
    EC_POINT_add(group, P_fake, sum, double_attacker, ctx);

    EC_POINT_invert(group, P_fake, ctx);

    EC_POINT_free(sum);
    EC_POINT_free(double_attacker);

    return P_fake;
}


