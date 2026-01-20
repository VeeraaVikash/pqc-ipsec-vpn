#include "pqc_ke_plugin.h"
#include "pqc_ke_handler.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("=== PQC Plugin Test ===\n\n");
    
    // Create handler
    pqc_ke_handler_t *handler = pqc_ke_handler_create();
    if (!handler) {
        fprintf(stderr, "Failed to create handler\n");
        return 1;
    }
    
    // Test 1: Generate keypair
    printf("\n[Test 1] Generating keypair...\n");
    uint8_t *pk = NULL, *sk = NULL;
    size_t pk_len = 0, sk_len = 0;
    
    if (handler->generate_keypair(handler, &pk, &pk_len, &sk, &sk_len) != 0) {
        fprintf(stderr, "Keypair generation failed\n");
        return 1;
    }
    printf("✓ Keypair generated successfully\n");
    
    // Test 2: Encapsulate
    printf("\n[Test 2] Encapsulating shared secret...\n");
    uint8_t *ct = NULL, *ss_enc = NULL;
    size_t ct_len = 0, ss_enc_len = 0;
    
    if (handler->encapsulate(handler, pk, pk_len, &ct, &ct_len, 
                            &ss_enc, &ss_enc_len) != 0) {
        fprintf(stderr, "Encapsulation failed\n");
        return 1;
    }
    printf("✓ Encapsulation successful\n");
    
    // Test 3: Decapsulate
    printf("\n[Test 3] Decapsulating shared secret...\n");
    uint8_t *ss_dec = NULL;
    size_t ss_dec_len = 0;
    
    if (handler->decapsulate(handler, ct, ct_len, sk, sk_len, 
                            &ss_dec, &ss_dec_len) != 0) {
        fprintf(stderr, "Decapsulation failed\n");
        return 1;
    }
    printf("✓ Decapsulation successful\n");
    
    // Test 4: Verify shared secrets match
    printf("\n[Test 4] Verifying shared secrets match...\n");
    if (ss_enc_len != ss_dec_len) {
        fprintf(stderr, "✗ Shared secret lengths don't match!\n");
        return 1;
    }
    
    if (memcmp(ss_enc, ss_dec, ss_enc_len) != 0) {
        fprintf(stderr, "✗ Shared secrets don't match!\n");
        return 1;
    }
    
    printf("✓ Shared secrets match! (%zu bytes)\n", ss_enc_len);
    printf("  First 16 bytes: ");
    for (int i = 0; i < 16 && i < ss_enc_len; i++) {
        printf("%02x", ss_enc[i]);
    }
    printf("...\n");
    
    // Cleanup
    free(pk);
    free(sk);
    free(ct);
    free(ss_enc);
    free(ss_dec);
    handler->destroy(handler);
    
    printf("\n=== All tests passed! ===\n");
    return 0;
}
