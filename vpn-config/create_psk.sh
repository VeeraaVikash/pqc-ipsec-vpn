#!/bin/bash

# Read the binary shared secret and convert to hex
SECRET_HEX=$(xxd -p /tmp/pqc_shared_secret_server.bin | tr -d '\n')

# Create ipsec.secrets file
cat > ipsec-secrets.conf << SECRETS
# PSK derived from PQC (Kyber-768) key exchange
@server @client : PSK 0x${SECRET_HEX}
SECRETS

echo "✓ Created PSK configuration from PQC shared secret"
echo "  Secret (first 32 hex chars): ${SECRET_HEX:0:32}..."
