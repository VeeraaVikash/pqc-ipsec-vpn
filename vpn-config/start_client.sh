#!/bin/bash
sudo ip netns exec vpn-client bash -c '
export IPSEC_DIR=/etc/netns/vpn-client
cd /etc/netns/vpn-client
/usr/lib/ipsec/starter \
    --daemon charon \
    --nofork
'
