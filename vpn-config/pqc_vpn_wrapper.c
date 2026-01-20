#include "../pqc-plugin/pqc_ke_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PQC_PORT 4500
#define BUFFER_SIZE 8192

typedef struct {
    uint8_t type;      // 1=public_key, 2=ciphertext, 3=ack
    uint32_t length;
    uint8_t data[];
} __attribute__((packed)) pqc_message_t;

void save_shared_secret(uint8_t *secret, size_t len, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("Failed to save shared secret");
        return;
    }
    fwrite(secret, 1, len, f);
    fclose(f);
    printf("[PQC VPN] Saved shared secret to %s (%zu bytes)\n", filename, len);
}

int run_server(const char *listen_addr) {
    pqc_ke_handler_t *handler = pqc_ke_handler_create();
    if (!handler) {
        fprintf(stderr, "Failed to create PQC handler\n");
        return 1;
    }
    
    // Generate server keypair
    uint8_t *pk = NULL, *sk = NULL;
    size_t pk_len = 0, sk_len = 0;
    
    printf("[PQC Server] Generating keypair...\n");
    if (handler->generate_keypair(handler, &pk, &pk_len, &sk, &sk_len) != 0) {
        fprintf(stderr, "Keypair generation failed\n");
        return 1;
    }
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PQC_PORT);
    inet_pton(AF_INET, listen_addr, &addr.sin_addr);
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    listen(sock, 1);
    printf("[PQC Server] Listening on %s:%d\n", listen_addr, PQC_PORT);
    
    int client = accept(sock, NULL, NULL);
    printf("[PQC Server] Client connected\n");
    
    // Send public key
    pqc_message_t *msg = malloc(sizeof(pqc_message_t) + pk_len);
    msg->type = 1;
    msg->length = htonl(pk_len);
    memcpy(msg->data, pk, pk_len);
    
    send(client, msg, sizeof(pqc_message_t) + pk_len, 0);
    printf("[PQC Server] Sent public key (%zu bytes)\n", pk_len);
    free(msg);
    
    // Receive ciphertext
    uint8_t buffer[BUFFER_SIZE];
    int recv_len = recv(client, buffer, BUFFER_SIZE, 0);
    pqc_message_t *ct_msg = (pqc_message_t*)buffer;
    
    if (ct_msg->type != 2) {
        fprintf(stderr, "Invalid message type\n");
        return 1;
    }
    
    size_t ct_len = ntohl(ct_msg->length);
    printf("[PQC Server] Received ciphertext (%zu bytes)\n", ct_len);
    
    // Decapsulate
    uint8_t *ss = NULL;
    size_t ss_len = 0;
    
    if (handler->decapsulate(handler, ct_msg->data, ct_len, sk, sk_len, 
                            &ss, &ss_len) != 0) {
        fprintf(stderr, "Decapsulation failed\n");
        return 1;
    }
    
    printf("[PQC Server] ✓ Key exchange complete!\n");
    printf("[PQC Server] Shared secret: ");
    for (int i = 0; i < 16 && i < ss_len; i++) {
        printf("%02x", ss[i]);
    }
    printf("...\n");
    
    // Save shared secret for StrongSwan to use
    save_shared_secret(ss, ss_len, "/tmp/pqc_shared_secret_server.bin");
    
    // Cleanup
    free(pk);
    free(sk);
    free(ss);
    handler->destroy(handler);
    close(client);
    close(sock);
    
    return 0;
}

int run_client(const char *server_addr) {
    pqc_ke_handler_t *handler = pqc_ke_handler_create();
    if (!handler) {
        fprintf(stderr, "Failed to create PQC handler\n");
        return 1;
    }
    
    // Connect to server
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PQC_PORT);
    inet_pton(AF_INET, server_addr, &addr.sin_addr);
    
    printf("[PQC Client] Connecting to %s:%d...\n", server_addr, PQC_PORT);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        return 1;
    }
    printf("[PQC Client] Connected\n");
    
    // Receive public key
    uint8_t buffer[BUFFER_SIZE];
    int recv_len = recv(sock, buffer, BUFFER_SIZE, 0);
    pqc_message_t *pk_msg = (pqc_message_t*)buffer;
    
    if (pk_msg->type != 1) {
        fprintf(stderr, "Invalid message type\n");
        return 1;
    }
    
    size_t pk_len = ntohl(pk_msg->length);
    printf("[PQC Client] Received public key (%zu bytes)\n", pk_len);
    
    // Encapsulate
    uint8_t *ct = NULL, *ss = NULL;
    size_t ct_len = 0, ss_len = 0;
    
    if (handler->encapsulate(handler, pk_msg->data, pk_len, 
                            &ct, &ct_len, &ss, &ss_len) != 0) {
        fprintf(stderr, "Encapsulation failed\n");
        return 1;
    }
    
    // Send ciphertext
    pqc_message_t *ct_msg = malloc(sizeof(pqc_message_t) + ct_len);
    ct_msg->type = 2;
    ct_msg->length = htonl(ct_len);
    memcpy(ct_msg->data, ct, ct_len);
    
    send(sock, ct_msg, sizeof(pqc_message_t) + ct_len, 0);
    printf("[PQC Client] Sent ciphertext (%zu bytes)\n", ct_len);
    free(ct_msg);
    
    printf("[PQC Client] ✓ Key exchange complete!\n");
    printf("[PQC Client] Shared secret: ");
    for (int i = 0; i < 16 && i < ss_len; i++) {
        printf("%02x", ss[i]);
    }
    printf("...\n");
    
    // Save shared secret for StrongSwan to use
    save_shared_secret(ss, ss_len, "/tmp/pqc_shared_secret_client.bin");
    
    // Cleanup
    free(ct);
    free(ss);
    handler->destroy(handler);
    close(sock);
    
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  Server: %s server <listen_addr>\n", argv[0]);
        printf("  Client: %s client <server_addr>\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "server") == 0) {
        return run_server(argv[2]);
    } else if (strcmp(argv[1], "client") == 0) {
        return run_client(argv[2]);
    } else {
        fprintf(stderr, "Invalid mode: %s\n", argv[1]);
        return 1;
    }
}
