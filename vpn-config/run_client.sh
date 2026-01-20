#!/bin/bash
sudo ip netns exec vpn-client bash -c '
    # Add tunnel IP
    ip addr add 10.10.2.1/24 dev lo 2>/dev/null || true
    
    # Add connection
    /usr/lib/ipsec/stroke add pqc-vpn \
        @client @server \
        192.168.100.2 192.168.100.1 \
        10.10.2.0/24 10.10.1.0/24
    
    sleep 2
    
    # Initiate connection
    echo "[Client] Initiating connection..."
    /usr/lib/ipsec/stroke up pqc-vpn
    
    tail -f /dev/null
'
