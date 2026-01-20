#ifndef PQC_KE_HANDLER_H_
#define PQC_KE_HANDLER_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct pqc_ke_handler_t pqc_ke_handler_t;

struct pqc_ke_handler_t {
    /**
     * Generate Kyber keypair
     */
    int (*generate_keypair)(pqc_ke_handler_t *this, 
                           uint8_t **public_key, size_t *pk_len,
                           uint8_t **secret_key, size_t *sk_len);
    
    /**
     * Encapsulate shared secret
     */
    int (*encapsulate)(pqc_ke_handler_t *this,
                      uint8_t *public_key, size_t pk_len,
                      uint8_t **ciphertext, size_t *ct_len,
                      uint8_t **shared_secret, size_t *ss_len);
    
    /**
     * Decapsulate shared secret
     */
    int (*decapsulate)(pqc_ke_handler_t *this,
                      uint8_t *ciphertext, size_t ct_len,
                      uint8_t *secret_key, size_t sk_len,
                      uint8_t **shared_secret, size_t *ss_len);
    
    /**
     * Destroy handler
     */
    void (*destroy)(pqc_ke_handler_t *this);
};

pqc_ke_handler_t *pqc_ke_handler_create();

#endif /* PQC_KE_HANDLER_H_ */
