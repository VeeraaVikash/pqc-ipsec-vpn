#!/bin/bash
sudo ip netns exec vpn-server bash -c '
export IPSEC_DIR=/etc/netns/vpn-server
cd /etc/netns/vpn-server
/usr/lib/ipsec/starter \
    --daemon charon \
    --nofork
'
