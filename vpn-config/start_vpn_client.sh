#!/bin/bash
cd /home/veeraavikash/pqc-vpn-project/vpn-config

# Copy configs to /etc for client
sudo cp ipsec-client.conf /etc/ipsec.conf
sudo cp ipsec.secrets /etc/ipsec.secrets
sudo chmod 600 /etc/ipsec.secrets

# Add tunnel IP in client namespace
sudo ip netns exec vpn-client ip addr add 10.10.2.1/24 dev lo 2>/dev/null || true

# Start StrongSwan in client namespace
echo "Starting VPN Client..."
sudo ip netns exec vpn-client ipsec start --nofork
