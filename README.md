# Modern Cryptographic Methods Project

This repository contains the source code, documentation, and supporting files for a comprehensive production practice project focused on modern cryptographic methods. The work was carried out as part of our production practice.

## Overview

This project explores a wide range of cryptographic algorithms and their implementations in C++ using QCA, OpenSSL, and Qt Creator. The main objectives are to identify potential weaknesses in cryptographic algorithms and to develop robust countermeasures.

## Key Components

- **RSA Encryption:**  
  - Implements RSA encryption/decryption with an optimized Karatsuba multiplication algorithm for efficient handling of large numbers.
  - Provides detailed theoretical analysis and proofs.
  
- **SHA-1 Hash Function:**  
  - Contains an implementation of the SHA-1 hash function.
  - Includes discussions on its vulnerabilities, reasons for deprecation, and modern alternatives such as SHA-256 and SHA-3.
  
- **Elliptic Curve Diffie-Hellman (ECDH):**  
  - Implements the ECDH protocol using the secp256k1 curve for secure key exchange.
  - Integrates AES encryption for secure message transmission.
  
- **Twisted ElGamal & Baby-Step Giant-Step (BSGS):**  
  - Implements the Twisted ElGamal encryption scheme tailored for elliptic curves.
  - Incorporates the BSGS algorithm for solving the discrete logarithm problem with detailed explanations of variables and their roles.
  
- **Schnorr Digital Signature:**  
  - Provides an implementation of the Schnorr digital signature scheme for individual signatures.
  
- **Naive Group Schnorr & Key Substitution Attack:**  
  - Implements a naive group Schnorr signature scheme.
  - Demonstrates a key substitution attack on the naive approach.
  
- **MuSig Protocol:**  
  - Implements the MuSig protocol as an enhanced, secure alternative to the naive group signature method.
  - Includes mathematical justifications for its improved security.

## Project Goals

The primary aim of this work is to identify weaknesses in various cryptographic algorithms and understand the methods for mitigating these vulnerabilities. By exploring foundational cryptographic schemes—many of which are still in use by high-profile services such as SSL/TLS, VPNs, and cryptocurrency networks (e.g., Bitcoin, Ethereum)—this project bridges the gap between theoretical cryptography and practical implementation. This project required a strong foundation in:
- Number Theory
- Abstract Algebra
- Cryptographic Theory
- Optimization Methods

## Future Directions

The insights and results from this project can be applied to:
- Further improvement of cryptographic systems.
- Enhancements in information security.
- Academic research and advanced studies in cryptography.

Potential future work includes the development of more secure protocols, adaptive security systems, and the exploration of advanced cryptographic techniques.

## Repository Structure

- `rsa/` – RSA encryption source code and documentation.
- `sha1/` – Implementation of the SHA-1 hash function and related discussions.
- `ecdh/` – Code for Elliptic Curve Diffie-Hellman (ECDH) and AES integration.
- `elgamal/` – Implementation of the Twisted ElGamal scheme and BSGS algorithm.
- `schnorr/` – Individual and group Schnorr digital signature schemes, implementation of the MuSig protocol.


## Acknowledgements

This project was completed as part of our production practice and has benefited from insights provided by the course instructors and various academic resources.
