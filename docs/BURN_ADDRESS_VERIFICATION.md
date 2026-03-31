# Genesis Burn Address Verification Guide

This document explains how to verify that the Genesis Burn Address was generated correctly and that its private key never existed.

## Overview

The Genesis Burn Address is a special address whose private key **never existed**. It was created deterministically from a hash, making it cryptographically impossible to compute the private key (solving the discrete logarithm problem).

## Public Verification

Anyone can verify the burn address generation using the following method:

### Step 1: Seed String

The seed string is publicly available in the source code:
```
"MIDAS_BURN_ADDRESS_V1_GENESIS"
```

Location: `src/cryptonote_config.h` (line ~275)

### Step 2: Generation Formula

The address is generated using this deterministic process:

1. **Hash the seed string** using Keccak256:
   ```
   seed_hash = Keccak256("MIDAS_BURN_ADDRESS_V1_GENESIS")
   ```

2. **Convert hash to point on Curve25519**:
   ```
   spend_public_key = ge_fromfe_frombytes_vartime(seed_hash)
   ```
   This uses the `ge_fromfe_frombytes_vartime` function to convert 32 bytes to a valid point on the Ed25519/Curve25519 curve.

3. **Generate view public key**:
   ```
   view_hash = Keccak256(spend_public_key)
   view_public_key = ge_fromfe_frombytes_vartime(view_hash)
   ```

4. **Create address**:
   ```
   address = Base58Encode(network_prefix, spend_public_key || view_public_key)
   ```

### Step 3: Verification Tools

#### Option A: Using the Verification Utility

Run the verification utility:
```bash
./midas-verify-burn-address
```

This will:
- Show the expected burn address
- Show the expected public keys
- Verify they match the genesis block
- Confirm the generation formula

#### Option B: Manual Verification

1. **Get the burn address from genesis block**:
   - Check the genesis transaction outputs
   - The burn address should be in the genesis block

2. **Regenerate from seed**:
   ```cpp
   account_public_address burn_addr = cryptonote::get_burn_address();
   ```
   This should match the address in genesis.

3. **Verify public keys**:
   - Extract public keys from the address
   - Verify they match the expected values from the seed

### Step 4: Cryptographic Proof

The key property: **The private key never existed**

**Why this is provable:**
- The public key was generated from a hash, not from a private key
- To find the private key, you would need to solve: `public_key = private_key * G`
- This is the **discrete logarithm problem**, which is computationally infeasible
- Since the hash was never derived from a private key, no private key exists

**Mathematical proof:**
- If a private key `k` existed such that `k * G = burn_public_key`, then:
  - `k = discrete_log(burn_public_key)`
  - But `burn_public_key = ge_fromfe_frombytes_vartime(Keccak256(seed))`
  - There is no known `k` that satisfies this equation
  - Therefore, `k` does not exist

## Expected Values

After running `./midas-gen-burn-address`, you will see:

### Mainnet Burn Address
- **Address**: `[Will be shown by utility]`
- **Spend Public Key**: `[64 hex characters]`
- **View Public Key**: `[64 hex characters]`

### Verification Checklist

- [ ] Seed string matches: `"MIDAS_BURN_ADDRESS_V1_GENESIS"`
- [ ] Address in genesis block matches regenerated address
- [ ] Public keys match expected values
- [ ] Address format is valid Base58
- [ ] Public keys are valid points on Curve25519

## Independent Verification

Anyone can independently verify by:

1. **Reading the source code**:
   - Check `src/cryptonote_config.h` for seed string
   - Check `src/cryptonote_basic/cryptonote_basic_impl.cpp` for generation function

2. **Running the generator**:
   ```bash
   ./midas-gen-burn-address
   ```

3. **Comparing with genesis block**:
   - Extract address from genesis transaction
   - Compare with generated address
   - They must match exactly

4. **Verifying the math**:
   - Implement the same hash → curve point conversion
   - Verify the result matches

## Transparency

All components are publicly verifiable:

- ✅ **Seed string**: Public in source code
- ✅ **Hash function**: Keccak256 (standard, verifiable)
- ✅ **Curve conversion**: `ge_fromfe_frombytes_vartime` (public algorithm)
- ✅ **Address encoding**: Base58 (standard)
- ✅ **Genesis block**: Public blockchain data

## Security Guarantees

1. **No private key exists**: Mathematically provable
2. **Deterministic generation**: Same seed always produces same address
3. **Public verification**: Anyone can verify the process
4. **Transparency**: All steps are documented and verifiable

## Questions?

If you have questions about the verification process:
1. Check the source code in `src/cryptonote_basic/cryptonote_basic_impl.cpp`
2. Run the verification utility
3. Review this documentation

---

**Last Updated**: [Date]
**Version**: 1.0
