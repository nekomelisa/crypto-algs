#include <QCoreApplication>
#include <QDebug>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <cstring>


void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}


EC_KEY* generate_key_pair(QString name)
{
    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec_key)
        handleErrors();

    if (EC_KEY_generate_key(ec_key) != 1)
        handleErrors();

    return ec_key;
}

QByteArray compute_shared_secret(EC_KEY *own_key, EC_POINT *peer_pub_key)
{
    const EC_GROUP *group = EC_KEY_get0_group(own_key);
    EC_POINT *shared_point = EC_POINT_new(group);
    BN_CTX *ctx = BN_CTX_new();

    if (!EC_POINT_mul(group, shared_point, nullptr, peer_pub_key, EC_KEY_get0_private_key(own_key), ctx))
        handleErrors();

    char *shared_hex = EC_POINT_point2hex(group, shared_point, POINT_CONVERSION_UNCOMPRESSED, ctx);
    QByteArray shared_secret = QByteArray(shared_hex);
    OPENSSL_free(shared_hex);

    EC_POINT_free(shared_point);
    BN_CTX_free(ctx);

    return shared_secret;
}


std::vector<unsigned char> derive_key(const std::vector<unsigned char>& dh_secret)
{
    std::vector<unsigned char> key(32);
    SHA256(dh_secret.data(), dh_secret.size(), key.data());
    return key;
}

std::vector<unsigned char> aes_encrypt(const std::vector<unsigned char>& plaintext,
                                       const std::vector<unsigned char>& key,
                                       std::vector<unsigned char>& iv)
{
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, ciphertext_len = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    ciphertext_len += len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}


std::vector<unsigned char> aes_decrypt(const std::vector<unsigned char>& ciphertext,
                                       const std::vector<unsigned char>& key,
                                       const std::vector<unsigned char>& iv)
{
    std::vector<unsigned char> plaintext(ciphertext.size());
    int len = 0, plaintext_len = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    plaintext_len += len;
    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return plaintext;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EC_KEY *alice_key = generate_key_pair("Alice");
    EC_KEY *bob_key = generate_key_pair("Bob");

    EC_POINT *alice_pub = (EC_POINT*)EC_KEY_get0_public_key(alice_key);
    EC_POINT *bob_pub = (EC_POINT*)EC_KEY_get0_public_key(bob_key);

    QByteArray alice_shared_secret = compute_shared_secret(alice_key, bob_pub);
    QByteArray bob_shared_secret = compute_shared_secret(bob_key, alice_pub);

    qDebug() << "Alice's computed shared secret: " << alice_shared_secret;
    qDebug() << "Bob's computed shared secret: " << bob_shared_secret;

    if (alice_shared_secret == bob_shared_secret)
    {
        qDebug() << "dh key exchage completed successfully!";
    }

    std::vector<unsigned char> dh_secret(alice_shared_secret.begin(), alice_shared_secret.end());

    std::vector<unsigned char> key = derive_key(dh_secret);

    std::vector<unsigned char> iv(16);
    RAND_bytes(iv.data(), iv.size());

    std::string message = "Hello, this is a secret!";
    std::vector<unsigned char> plaintext(message.begin(), message.end());

    std::vector<unsigned char> ciphertext = aes_encrypt(plaintext, key, iv);

    std::vector<unsigned char> decrypted_text = aes_decrypt(ciphertext, key, iv);

    std::string decrypted_message(decrypted_text.begin(), decrypted_text.end());
    std::cout << "Decrypted message: " << decrypted_message << std::endl;



    return 0;
}
