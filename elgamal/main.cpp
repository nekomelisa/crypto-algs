#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <iostream>
#include <QCoreApplication>
#include <unordered_map>
#include <vector>

struct key_pair
{
    EC_POINT* public_key;
    BIGNUM* secret_key;
};

struct ciphertext
{
    EC_POINT* C;
    EC_POINT* D;
};

// Converts a string to a BIGNUM (up to 4 characters)
BIGNUM* string_to_bignum(const std::string& text)
{
    if (text.size() > 4)
    {
        std::cerr << "[Error] Message is too long! Must be <= 4 characters (32 bits)." << std::endl;
        return nullptr;
    }

    BIGNUM* bn = BN_new();
    BN_bin2bn(reinterpret_cast<const unsigned char*>(text.c_str()), text.size(), bn);
    return bn;
}

// Key generation function
key_pair keygen(EC_GROUP* curve)
{
    key_pair keys;
    BN_CTX* ctx = BN_CTX_new();
    keys.secret_key = BN_new();
    BIGNUM* order = BN_new();
    EC_GROUP_get_order(curve, order, ctx);
    BN_rand_range(keys.secret_key, order);

    BIGNUM* s_inv = BN_new();
    BN_mod_inverse(s_inv, keys.secret_key, order, ctx);

    keys.public_key = EC_POINT_new(curve);
    EC_POINT_mul(curve, keys.public_key, s_inv, NULL, NULL, ctx);

    BN_free(s_inv);
    BN_free(order);
    BN_CTX_free(ctx);
    return keys;
}

// Encryption function
ciphertext encrypt(EC_GROUP* curve, EC_POINT* public_key, BIGNUM* message)
{
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* order = BN_new();
    EC_GROUP_get_order(curve, order, ctx);
    BIGNUM* r = BN_new();
    BN_rand_range(r, order);

    EC_POINT* C = EC_POINT_new(curve);
    const EC_POINT* G = EC_GROUP_get0_generator(curve);
    EC_POINT* mG = EC_POINT_new(curve);
    EC_POINT_mul(curve, mG, message, NULL, NULL, ctx);
    EC_POINT_mul(curve, C, r, G, NULL, ctx);
    EC_POINT_add(curve, C, C, mG, ctx);

    EC_POINT* D = EC_POINT_new(curve);
    EC_POINT_mul(curve, D, NULL, public_key, r, ctx);

    BN_free(order);
    BN_free(r);
    EC_POINT_free(mG);
    BN_CTX_free(ctx);
    return {C, D};
}

// Computes discrete logarithm using Baby-step Giant-step
BIGNUM* compute_discrete_log(EC_GROUP* curve, const EC_POINT* G, const EC_POINT* C)
{
    const int lookup_size = 1 << 16; // 2^16 = 65536

    BN_CTX* ctx = BN_CTX_new();
    std::unordered_map<std::string, int> lookup_table;
    BIGNUM* two_pow_16 = BN_new();
    BN_set_word(two_pow_16, lookup_size);

    EC_POINT* G_16 = EC_POINT_new(curve);
    EC_POINT_mul(curve, G_16, NULL, G, two_pow_16, ctx);

    // Step 1: Precompute table for xhi
    EC_POINT* xhiG_16 = EC_POINT_new(curve);
    for (int xhi = 0; xhi < lookup_size; xhi++)
    {
        BIGNUM* xhi_bn = BN_new();
        BN_set_word(xhi_bn, xhi);
        EC_POINT_mul(curve, xhiG_16, NULL, G_16, xhi_bn, ctx);
        char* key = EC_POINT_point2hex(curve, xhiG_16, POINT_CONVERSION_UNCOMPRESSED, ctx);
        lookup_table[key] = xhi;
        OPENSSL_free(key);
        BN_free(xhi_bn);
    }

    // Step 2: Iterate over xlo values
    EC_POINT* xloG = EC_POINT_new(curve);
    EC_POINT* target = EC_POINT_new(curve);
    for (int xlo = 0; xlo < lookup_size; xlo++)
    {
        BIGNUM* xlo_bn = BN_new();
        BN_set_word(xlo_bn, xlo);
        EC_POINT_mul(curve, xloG, NULL, G, xlo_bn, ctx);
        EC_POINT_invert(curve, xloG, ctx);
        EC_POINT_add(curve, target, C, xloG, ctx);

        char* key = EC_POINT_point2hex(curve, target, POINT_CONVERSION_UNCOMPRESSED, ctx);
        if (lookup_table.find(key) != lookup_table.end())
        {
            int xhi = lookup_table[key];

            // Compute final x = xlo + (2^16 * xhi)
            BIGNUM* x = BN_new();
            BIGNUM* xhi_bn = BN_new();
            BIGNUM* scaled_xhi = BN_new();

            BN_set_word(xhi_bn, xhi);
            BN_mul(scaled_xhi, xhi_bn, two_pow_16, ctx);
            BN_add(x, scaled_xhi, xlo_bn);

            // Cleanup
            OPENSSL_free(key);
            BN_free(xhi_bn);
            BN_free(scaled_xhi);
            BN_free(xlo_bn);
            EC_POINT_free(xloG);
            EC_POINT_free(xhiG_16);
            EC_POINT_free(G_16);
            EC_POINT_free(target);
            BN_free(two_pow_16);
            BN_CTX_free(ctx);

            return x;
        }

        OPENSSL_free(key);
        BN_free(xlo_bn);
    }

    std::cerr << "[Error] Discrete log not found!" << std::endl;
    EC_POINT_free(xloG);
    EC_POINT_free(xhiG_16);
    EC_POINT_free(G_16);
    EC_POINT_free(target);
    BN_free(two_pow_16);
    BN_CTX_free(ctx);
    return NULL;
}

// Decryption function
BIGNUM* decrypt(EC_GROUP* curve, BIGNUM* secret_key, ciphertext ct)
{
    BN_CTX* ctx = BN_CTX_new();
    EC_POINT* sD = EC_POINT_new(curve);
    EC_POINT* C_minus_sD = EC_POINT_new(curve);
    const EC_POINT* G = EC_GROUP_get0_generator(curve);

    EC_POINT_mul(curve, sD, NULL, ct.D, secret_key, ctx);
    EC_POINT_invert(curve, sD, ctx);
    EC_POINT_add(curve, C_minus_sD, ct.C, sD, ctx);

    BIGNUM* x = compute_discrete_log(curve, G, C_minus_sD);

    EC_POINT_free(sD);
    EC_POINT_free(C_minus_sD);
    BN_CTX_free(ctx);

    return x;
}

void process_message(EC_GROUP* curve, key_pair keys, const std::string& original_message)
{
    std::cout << "\nProcessing message: \"" << original_message << "\"\n" << std::flush;

    BIGNUM* message = string_to_bignum(original_message);
    if (!message)
    {
        std::cerr << "Skipping invalid message: " << original_message << "\n" << std::flush;
        return;
    }

    ciphertext ct = encrypt(curve, keys.public_key, message);
    BIGNUM* decrypted_message = decrypt(curve, keys.secret_key, ct);

    int size = BN_num_bytes(decrypted_message);
    std::vector<unsigned char> buffer(size);
    BN_bn2bin(decrypted_message, buffer.data());

    std::string decrypted_text(buffer.begin(), buffer.end());
    std::cout << "Decrypted Message: " << decrypted_text << "\n" << std::flush;

    BN_free(message);
    BN_free(decrypted_message);
    EC_POINT_free(ct.C);
    EC_POINT_free(ct.D);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EC_GROUP* curve = EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
    key_pair keys = keygen(curve);

    std::vector<std::string> messages = {"1", "123", "#", "h", "he", "hel", "hell", "hello"};

    for (const auto& message : messages)
    {
        process_message(curve, keys, message);
    }

    EC_POINT_free(keys.public_key);
    BN_free(keys.secret_key);
    EC_GROUP_free(curve);

    return 0;
}
