#include "pqc_ke_handler.h"
#include <oqs/oqs.h>
#include <string.h>
#include <stdio.h>

typedef struct private_pqc_ke_handler_t private_pqc_ke_handler_t;

struct private_pqc_ke_handler_t {
    pqc_ke_handler_t public;
    OQS_KEM *kem;
};

static int generate_keypair_impl(pqc_ke_handler_t *this, 
                                 uint8_t **public_key, size_t *pk_len,
                                 uint8_t **secret_key, size_t *sk_len)
{
    private_pqc_ke_handler_t *priv = (private_pqc_ke_handler_t*)this;
    
    *public_key = malloc(priv->kem->length_public_key);
    *secret_key = malloc(priv->kem->length_secret_key);
    
    if (!*public_key || !*secret_key) {
        free(*public_key);
        free(*secret_key);
        return -1;
    }
    
    if (OQS_KEM_keypair(priv->kem, *public_key, *secret_key) != OQS_SUCCESS) {
        free(*public_key);
        free(*secret_key);
        return -1;
    }
    
    *pk_len = priv->kem->length_public_key;
    *sk_len = priv->kem->length_secret_key;
    
    printf("[PQC] Generated Kyber-768 keypair (pk=%zu, sk=%zu)\n", 
           *pk_len, *sk_len);
    
    return 0;
}

static int encapsulate_impl(pqc_ke_handler_t *this,
                           uint8_t *public_key, size_t pk_len,
                           uint8_t **ciphertext, size_t *ct_len,
                           uint8_t **shared_secret, size_t *ss_len)
{
    private_pqc_ke_handler_t *priv = (private_pqc_ke_handler_t*)this;
    
    if (pk_len != priv->kem->length_public_key) {
        return -1;
    }
    
    *ciphertext = malloc(priv->kem->length_ciphertext);
    *shared_secret = malloc(priv->kem->length_shared_secret);
    
    if (!*ciphertext || !*shared_secret) {
        free(*ciphertext);
        free(*shared_secret);
        return -1;
    }
    
    if (OQS_KEM_encaps(priv->kem, *ciphertext, *shared_secret, 
                       public_key) != OQS_SUCCESS) {
        free(*ciphertext);
        free(*shared_secret);
        return -1;
    }
    
    *ct_len = priv->kem->length_ciphertext;
    *ss_len = priv->kem->length_shared_secret;
    
    printf("[PQC] Encapsulated shared secret (ct=%zu, ss=%zu)\n", 
           *ct_len, *ss_len);
    
    return 0;
}

static int decapsulate_impl(pqc_ke_handler_t *this,
                           uint8_t *ciphertext, size_t ct_len,
                           uint8_t *secret_key, size_t sk_len,
                           uint8_t **shared_secret, size_t *ss_len)
{
    private_pqc_ke_handler_t *priv = (private_pqc_ke_handler_t*)this;
    
    if (ct_len != priv->kem->length_ciphertext ||
        sk_len != priv->kem->length_secret_key) {
        return -1;
    }
    
    *shared_secret = malloc(priv->kem->length_shared_secret);
    
    if (!*shared_secret) {
        return -1;
    }
    
    if (OQS_KEM_decaps(priv->kem, *shared_secret, ciphertext, 
                       secret_key) != OQS_SUCCESS) {
        free(*shared_secret);
        return -1;
    }
    
    *ss_len = priv->kem->length_shared_secret;
    
    printf("[PQC] Decapsulated shared secret (ss=%zu)\n", *ss_len);
    
    return 0;
}

static void destroy_handler(pqc_ke_handler_t *this)
{
    private_pqc_ke_handler_t *priv = (private_pqc_ke_handler_t*)this;
    if (priv->kem) {
        OQS_KEM_free(priv->kem);
    }
    free(priv);
}

pqc_ke_handler_t *pqc_ke_handler_create()
{
    private_pqc_ke_handler_t *this;
    
    this = malloc(sizeof(private_pqc_ke_handler_t));
    if (!this) {
        return NULL;
    }
    
    memset(this, 0, sizeof(*this));
    
    this->public.generate_keypair = generate_keypair_impl;
    this->public.encapsulate = encapsulate_impl;
    this->public.decapsulate = decapsulate_impl;
    this->public.destroy = destroy_handler;
    this->kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    
    if (!this->kem) {
        fprintf(stderr, "[PQC] Failed to initialize Kyber-768\n");
        free(this);
        return NULL;
    }
    
    printf("[PQC] Kyber-768 KEM initialized\n");
    printf("[PQC]   Public key size: %zu bytes\n", this->kem->length_public_key);
    printf("[PQC]   Secret key size: %zu bytes\n", this->kem->length_secret_key);
    printf("[PQC]   Ciphertext size: %zu bytes\n", this->kem->length_ciphertext);
    printf("[PQC]   Shared secret size: %zu bytes\n", this->kem->length_shared_secret);
    
    return &this->public;
}
