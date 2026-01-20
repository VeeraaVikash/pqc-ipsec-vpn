#!/bin/bash
cd /home/veeraavikash/pqc-vpn-project/vpn-config

# Copy configs to /etc for server
sudo cp ipsec-server.conf /etc/ipsec.conf
sudo cp ipsec.secrets /etc/ipsec.secrets
sudo chmod 600 /etc/ipsec.secrets

# Add tunnel IP in server namespace
sudo ip netns exec vpn-server ip addr add 10.10.1.1/24 dev lo 2>/dev/null || true

# Start StrongSwan in server namespace
echo "Starting VPN Server..."
sudo ip netns exec vpn-server ipsec start --nofork
