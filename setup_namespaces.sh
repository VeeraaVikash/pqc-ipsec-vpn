#!/bin/bash

# Clean up any existing setup
sudo ip netns del vpn-client 2>/dev/null
sudo ip netns del vpn-server 2>/dev/null
sudo ip link del veth0 2>/dev/null

# Create namespaces
sudo ip netns add vpn-server
sudo ip netns add vpn-client

# Create veth pair
sudo ip link add veth0 type veth peer name veth1

# Assign interfaces to namespaces
sudo ip link set veth0 netns vpn-server
sudo ip link set veth1 netns vpn-client

# Configure server namespace
sudo ip netns exec vpn-server ip addr add 192.168.100.1/24 dev veth0
sudo ip netns exec vpn-server ip link set veth0 up
sudo ip netns exec vpn-server ip link set lo up

# Configure client namespace
sudo ip netns exec vpn-client ip addr add 192.168.100.2/24 dev veth1
sudo ip netns exec vpn-client ip link set veth1 up
sudo ip netns exec vpn-client ip link set lo up

echo "✓ Network namespaces created:"
echo "  Server: 192.168.100.1 (vpn-server)"
echo "  Client: 192.168.100.2 (vpn-client)"

# Test connectivity
echo ""
echo "Testing connectivity..."
sudo ip netns exec vpn-client ping -c 3 192.168.100.1
