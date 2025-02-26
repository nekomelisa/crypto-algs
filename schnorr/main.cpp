#include <QCoreApplication>
#include <iostream>
#include "key_management.h"
#include "schnorr.h"
#include "group_schnorr.h"
#include "musig.h"
#include <chrono>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


        BN_CTX *ctx = BN_CTX_new();
        EC_GROUP *group = init_group();
        std::string message = "test message 123 #";

        auto start = std::chrono::high_resolution_clock::now();


        SchnorrKeyPair classic_keys = key_gen(group, ctx);
        SchnorrSignature classic_signature = sign(message, classic_keys, group, ctx);
        bool classic_valid = verify(message, classic_signature, classic_keys, group, ctx);
        std::cout << "classic Schnorr signature is " << (classic_valid ? "valid" : "invalid") << std::endl;


        SchnorrKeyPair group_key1 = key_gen(group, ctx);
        SchnorrKeyPair group_key2 = key_gen(group, ctx);
        std::vector<SchnorrKeyPair> group_keys = {group_key1, group_key2};
        EC_POINT *P_group = key_sum(group_keys, group, ctx);
        GroupSchnorrSignature group_signature = naive_group_sign(message, group_keys, group, ctx);
        bool group_valid = verify(message, {group_signature.R_agg, group_signature.s_agg}, {nullptr, P_group}, group, ctx);
        std::cout << "naive group Schnorr signature is " << (group_valid ? "valid" : "invalid") << std::endl;


        SchnorrKeyPair attacker = key_gen(group, ctx);
        std::vector<SchnorrKeyPair> attack_keys = {group_key1, group_key2, attacker};
        EC_POINT *P_fake = key_substitution_attack(attack_keys, attacker, group, ctx);
        std::vector<SchnorrKeyPair> attack_keys_modified = {group_key1, group_key2, {nullptr, P_fake}};
        EC_POINT *P_attack = key_sum(attack_keys_modified, group, ctx);
        SchnorrSignature attack_signature = sign(message, attacker, group, ctx);
        bool attack_valid = verify(message, attack_signature, {nullptr, P_attack}, group, ctx);
        std::cout << "key substitution sttack signature is " << (attack_valid ? "valid" : "invalid") << std::endl;


        SchnorrKeyPair musig_key1 = key_gen(group, ctx);
        SchnorrKeyPair musig_key2 = key_gen(group, ctx);
        std::vector<SchnorrKeyPair> musig_keys = {musig_key1, musig_key2};
        EC_POINT *P_musig = musig_aggregate_keys(musig_keys, group, ctx);
        MuSigSignature musig_signature = musig_sign(message, musig_keys, group, ctx);
        bool musig_valid = musig_verify(message, musig_signature, P_musig, group, ctx);
        std::cout << "musig signature is " << (musig_valid ? "valid" : "invalid") << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "execution time: " << elapsed.count() << " seconds" << std::endl;

        // Cleanup
        EC_POINT_free(classic_signature.R);
        BN_free(classic_signature.s);
        BN_free(classic_keys.priv_key);
        EC_POINT_free(classic_keys.pub_key);

        EC_POINT_free(group_signature.R_agg);
        BN_free(group_signature.s_agg);
        BN_free(group_key1.priv_key);
        EC_POINT_free(group_key1.pub_key);
        BN_free(group_key2.priv_key);
        EC_POINT_free(group_key2.pub_key);
        EC_POINT_free(P_group);

        EC_POINT_free(P_fake);
        EC_POINT_free(P_attack);
        EC_POINT_free(attack_signature.R);
        BN_free(attack_signature.s);
        BN_free(attacker.priv_key);
        EC_POINT_free(attacker.pub_key);

        EC_POINT_free(P_musig);
        EC_POINT_free(musig_signature.R_agg);
        BN_free(musig_signature.s);
        BN_free(musig_key1.priv_key);
        EC_POINT_free(musig_key1.pub_key);
        BN_free(musig_key2.priv_key);
        EC_POINT_free(musig_key2.pub_key);

        EC_GROUP_free(group);
        BN_CTX_free(ctx);


    return 0;
}
