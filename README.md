# crypto-lags
This repository contains the source code, documentation, and supporting files for a comprehensive production practice project focused on modern cryptographic methods.
Key Components:
	•	RSA Encryption:
Implements RSA encryption/decryption with an optimized Karatsuba multiplication algorithm for efficient handling of large numbers. Detailed theoretical analysis and proofs are provided.
	•	SHA-1 Hash Function:
Provides an implementation of the SHA-1 hash function along with a discussion on its vulnerabilities, reasons for deprecation, and modern alternatives such as SHA-256 and SHA-3.
	•	Elliptic Curve Diffie-Hellman (ECDH):
Implements the ECDH protocol using the secp256k1 curve for secure key exchange, integrated with AES encryption to secure message transmission.
	•	Twisted ElGamal & Baby-Step Giant-Step (BSGS):
Implements the Twisted ElGamal encryption scheme tailored for elliptic curves along with the BSGS algorithm for solving the discrete logarithm problem, including detailed explanations of variables and their roles.
	•	Schnorr Digital Signature:
Provides an implementation of the Schnorr digital signature scheme, including both individual signatures and a naive group signature scheme.
	•	Key Substitution Attack & MuSig Protocol:
Demonstrates a key substitution attack on naive group Schnorr signatures and implements the MuSig protocol as a robust countermeasure to enhance security.

Project Goals:
The primary aim of this work is to identify weaknesses in various cryptographic algorithms and to understand the methods for mitigating these vulnerabilities. Through an in-depth exploration of foundational cryptographic schemes—which are still employed by numerous high-profile services such as SSL/TLS, VPNs, and cryptocurrency networks (e.g., Bitcoin, Ethereum)—this project bridges theoretical cryptography with practical implementation. It also requires a solid foundation in number theory, abstract algebra, cryptographic theory, and optimization methods.

Future Directions:
The insights and results from this project can be applied to further improve cryptographic systems, enhance information security, and support academic research. Potential future work includes the development of more secure protocols, adaptive security systems, and continued exploration into advanced cryptographic techniques.
