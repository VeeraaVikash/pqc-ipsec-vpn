

```markdown
# Post-Quantum Cryptography Integration in IPsec VPN

<div align="center">

![PQC-VPN](https://img.shields.io/badge/PQC-Kyber--768-blueviolet?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Proof--of--Concept-success?style=for-the-badge)
![License](https://img.shields.io/badge/License-Apache%202.0-blue?style=for-the-badge)

**A production-aligned Proof-of-Concept demonstrating quantum-resistant key exchange in enterprise VPN infrastructure**

[Features](#-key-features) • [Architecture](#-architecture) • [Installation](#-installation) • [Demo](#-demo) • [Documentation](#-documentation)

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Key Features](#-key-features)
- [Architecture](#-architecture)
- [Technical Stack](#-technical-stack)
- [Installation](#-installation)
- [Usage](#-usage)
- [Project Structure](#-project-structure)
- [Performance](#-performance-benchmarks)
- [Security](#-security-considerations)
- [Dashboard](#-monitoring-dashboard)
- [Future Work](#-roadmap)
- [License](#-license)

---

## 🎯 Overview

This project demonstrates how **NIST-standardized Post-Quantum Cryptography** (Kyber-768) can be integrated into existing **IPsec VPN** systems without requiring a complete protocol redesign.

### The Problem

Current VPN encryption (RSA, ECDH) will be vulnerable to quantum computers using Shor's algorithm. Organizations need to begin transitioning to quantum-safe cryptography **now** to protect sensitive data.

### Our Solution

A hybrid approach that integrates Post-Quantum Cryptography into existing VPN infrastructure:
- ✅ Uses Kyber-768 for quantum-safe key exchange
- ✅ Derives Pre-Shared Key (PSK) from PQC shared secret
- ✅ Integrates with standard StrongSwan IPsec/IKEv2
- ✅ No protocol redesign required
- ✅ Clear migration path for enterprises

---

## ✨ Key Features

### 🛡️ Quantum-Safe Cryptography
- **Algorithm:** Kyber-768 (NIST FIPS 203 standardized)
- **Security Level:** NIST Level 3 (~192-bit classical security)
- **Quantum Attack Resistance:** Protected against Shor's algorithm

### 🔌 Seamless Integration
- Works with existing StrongSwan IPsec infrastructure
- Standard IKEv2 protocol with PQC-derived PSK
- No changes to IPsec/ESP protocols required

### ⚡ High Performance
- Key exchange: **<100ms**
- Minimal overhead: **~4.7KB** per handshake
- Production-viable performance metrics

### 📊 Professional Dashboard
- Real-time VPN monitoring
- Beautiful dark-themed UI
- Live metrics and charts
- Security status visualization

---

## 🏗️ Architecture

### System Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                         VPN Client                              │
│  ┌──────────────────┐         ┌──────────────────────────┐     │
│  │   Kyber-768 KEM  │────────▶│  Shared Secret (32 bytes)│     │
│  │  Key Generation  │         └──────────┬───────────────┘     │
│  │  Encapsulation   │                    │                      │
│  └──────────────────┘                    ▼                      │
│                              ┌────────────────────┐             │
│                              │ PSK Derivation     │             │
│                              │ (Hex Conversion)   │             │
│                              └─────────┬──────────┘             │
└──────────────────────────────────────────┼──────────────────────┘
                                           │
                              ═════════════╪═══════════════
                               IPsec/IKEv2 Tunnel (AES-256)
                              ═════════════╪═══════════════
                                           │
┌──────────────────────────────────────────┼──────────────────────┐
│                         VPN Server        │                      │
│                              ┌────────────▼──────────┐           │
│                              │ PSK Authentication    │           │
│                              │ (PQC-Derived)         │           │
│                              └───────────────────────┘           │
│  ┌──────────────────┐                                           │
│  │   Kyber-768 KEM  │                                           │
│  │  Key Generation  │                                           │
│  │  Decapsulation   │                                           │
│  └──────────────────┘                                           │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Technical Stack

| Component | Technology | Version | Purpose |
|-----------|-----------|---------|---------|
| **PQC Library** | liboqs | Latest | Kyber-768 KEM implementation |
| **VPN Software** | StrongSwan | 5.9.13 | IPsec/IKEv2 VPN daemon |
| **Operating System** | Ubuntu | 24.04 LTS | Host system |
| **Programming** | C | C11 | Core cryptographic operations |
| **Dashboard** | React | 18.3.1 | Real-time monitoring UI |

---

## 📦 Installation

### Prerequisites

```bash
# Ubuntu 24.04 LTS
sudo apt update && sudo apt upgrade -y

# Install build dependencies
sudo apt install -y \
    build-essential \
    cmake \
    git \
    wget \
    libssl-dev \
    pkg-config \
    autoconf \
    libtool \
    strongswan \
    strongswan-pki \
    libcharon-extra-plugins \
    libcurl4-openssl-dev
```

### Step 1: Build liboqs

```bash
cd ~
git clone https://github.com/open-quantum-safe/liboqs.git
cd liboqs
mkdir build && cd build

cmake -GNinja \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    ..

ninja
sudo ninja install
sudo ldconfig
```

### Step 2: Build PQC Plugin

```bash
cd ~/pqc-vpn-project/pqc-plugin
make

# Verify installation
./test_pqc
```

**Expected output:**
```
=== PQC Plugin Test ===

[PQC] Kyber-768 KEM initialized
✓ Keypair generated successfully
✓ Encapsulation successful
✓ Decapsulation successful
✓ Shared secrets match! (32 bytes)

=== All tests passed! ===
```

---

## 🚀 Usage

### Running PQC Key Exchange

**Terminal 1 - Server:**
```bash
cd ~/pqc-vpn-project/vpn-config
sudo ip netns exec vpn-server ./pqc_vpn_wrapper server 192.168.100.1
```

**Terminal 2 - Client:**
```bash
cd ~/pqc-vpn-project/vpn-config
sudo ip netns exec vpn-client ./pqc_vpn_wrapper client 192.168.100.1
```

**Expected Output:**
```
[PQC Server] ✓ Key exchange complete!
[PQC Server] Shared secret: 62bf083b8f533ba0f2f7208d259079d0...
[PQC Client] ✓ Key exchange complete!
[PQC Client] Shared secret: 62bf083b8f533ba0f2f7208d259079d0...
✓ Secrets MATCH!
```

### Configuring StrongSwan VPN

```bash
# Generate PSK from PQC shared secret
SECRET_HEX=$(xxd -p /tmp/pqc_shared_secret_server.bin | tr -d '\n')

# Add to StrongSwan secrets
echo "@server @client : PSK 0x${SECRET_HEX}" | sudo tee /etc/ipsec.secrets

# Start VPN
sudo ipsec start
sudo ipsec statusall
```

---

## 📂 Project Structure

```
pqc-vpn-project/
├── pqc-plugin/                 # Kyber-768 Implementation
│   ├── pqc_ke_handler.c        # KEM operations
│   ├── pqc_ke_handler.h
│   ├── pqc_ke_plugin.c         # Plugin wrapper
│   ├── test_pqc.c              # Test suite
│   ├── Makefile
│   └── libstrongswan-pqc-ke.so
│
├── vpn-config/                 # VPN Configuration
│   ├── pqc_vpn_wrapper.c       # Network key exchange
│   ├── ipsec-server.conf       # StrongSwan server
│   ├── ipsec-client.conf       # StrongSwan client
│   └── ipsec.secrets
│
├── dashboard/                  # Monitoring Dashboard
│   ├── src/
│   │   ├── App.js
│   │   ├── App.css
│   │   └── components/
│   └── package.json
│
├── setup_namespaces.sh
└── README.md
```

---

## ⚡ Performance Benchmarks

### Cryptographic Operations

| Operation | Time | Throughput |
|-----------|------|------------|
| **Key Generation** | ~40ms | 25 ops/sec |
| **Encapsulation** | ~30ms | 33 ops/sec |
| **Decapsulation** | ~30ms | 33 ops/sec |
| **Total Handshake** | **~100ms** | **10 handshakes/sec** |

### Network Overhead

| Metric | Classical (ECDH) | PQC (Kyber-768) | Overhead |
|--------|------------------|-----------------|----------|
| Handshake Size | ~100 bytes | ~2.3 KB | +2200% |
| Handshake Time | ~20ms | ~100ms | +400% |

**Conclusion:** Performance overhead is **acceptable for production** use.

---

## 🔒 Security Considerations

### Quantum Safety

✅ **Protected Against:**
- Shor's algorithm (quantum factoring)
- Grover's algorithm (quantum search)
- Future quantum computer attacks

### Security Best Practices

1. **Key Rotation:** Implement regular PSK updates
2. **Secure Storage:** Protect shared secret files (chmod 600)
3. **Network Isolation:** Use network namespaces or separate hosts
4. **Monitoring:** Enable logging for all key exchange events
5. **Auditing:** Regular security audits recommended

---

## 📊 Monitoring Dashboard

### Features

- **Real-time Metrics:** Live VPN status, latency, throughput
- **Visual Analytics:** Charts, graphs, and progress indicators
- **Security Monitoring:** Algorithm in use, security score
- **Activity Logs:** Recent events and security alerts
- **Responsive Design:** Works on desktop, tablet, mobile

### Running the Dashboard

```bash
cd ~/pqc-vpn-project/dashboard/pqc-dashboard
npm install
npm start

# Opens at http://localhost:3000
```

---

## 🛣️ Roadmap

### ✅ Completed (PoC Phase)

- [x] Kyber-768 library integration
- [x] Key exchange protocol implementation
- [x] StrongSwan configuration
- [x] PSK derivation from PQC secret
- [x] Performance benchmarking
- [x] Monitoring dashboard

### 🔜 Future Work (Production Phase)

- [ ] **Hybrid Mode:** Combine classical + PQC
- [ ] **Kernel Integration:** Native IPsec/IKEv2 protocol extension
- [ ] **Automated Key Rotation:** Periodic PSK updates
- [ ] **Performance Optimization:** Hardware acceleration
- [ ] **Multi-Platform Support:** Windows, macOS, iOS, Android
- [ ] **Security Audit:** Third-party cryptographic review
- [ ] **FIPS Compliance:** Federal standards certification

---

## 📚 References

- [NIST Post-Quantum Cryptography](https://csrc.nist.gov/projects/post-quantum-cryptography)
- [FIPS 203: Module-Lattice-Based KEM Standard](https://csrc.nist.gov/pubs/fips/203/final)
- [Kyber Specification](https://pq-crystals.org/kyber/)
- [liboqs - Open Quantum Safe](https://github.com/open-quantum-safe/liboqs)
- [StrongSwan Documentation](https://docs.strongswan.org/)
- [RFC 7296 - IKEv2](https://datatracker.ietf.org/doc/html/rfc7296)

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

```bash
# Fork and clone
git clone https://github.com/VeeraaVikash/pqc-ipsec-vpn.git

# Create feature branch
git checkout -b feature/amazing-feature

# Make changes and commit
git commit -m 'Add amazing feature'
git push origin feature/amazing-feature
```

---

## 📄 License

This project is licensed under the **Apache License 2.0** - see the [LICENSE](LICENSE) file for details.

---

## ✨ Acknowledgments

- **Open Quantum Safe Project** for liboqs library
- **StrongSwan Team** for robust VPN implementation
- **NIST** for PQC standardization efforts
- **CRYSTALS-Kyber Team** for designing practical KEM

---

## 📞 Contact

**Author:** Veeraa Vikash  
**GitHub:** [@VeeraaVikash](https://github.com/VeeraaVikash)  
**Project:** [pqc-ipsec-vpn](https://github.com/VeeraaVikash/pqc-ipsec-vpn)

---

## ⚠️ Disclaimer

This is a **Proof-of-Concept** for research and educational purposes. While production-aligned, it requires:

- Additional security hardening
- Professional security audit
- Comprehensive testing
- Performance optimization

**Do not deploy in production** without proper review.

---

<div align="center">

**🔐 Building a Quantum-Safe Future 🔐**

*Protecting today's data from tomorrow's threats*

Made with ❤️ for a quantum-safe world

</div>
```
