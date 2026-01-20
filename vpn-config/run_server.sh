#!/bin/bash
sudo ip netns exec vpn-server bash -c '
    # Add tunnel IP
    ip addr add 10.10.1.1/24 dev lo 2>/dev/null || true
    
    # Use stroke to add connection directly
    /usr/lib/ipsec/stroke add pqc-vpn \
        @server @client \
        192.168.100.1 192.168.100.2 \
        10.10.1.0/24 10.10.2.0/24
    
    echo "[Server] Connection added. Waiting for client..."
    tail -f /dev/null
'
