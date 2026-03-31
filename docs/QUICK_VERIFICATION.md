# Quick Verification Guide

## How to Verify Genesis Burn Address

### Method 1: Using Tools (Easiest)

```bash
# Generate and see the burn address
./midas-gen-burn-address

# Verify the generation process
./midas-verify-burn-address
```

### Method 2: Check Source Code

1. **Seed string** (public):
   - File: `src/cryptonote_config.h`
   - Line: ~275
   - Value: `"MIDAS_BURN_ADDRESS_V1_GENESIS"`

2. **Generation function**:
   - File: `src/cryptonote_basic/cryptonote_basic_impl.cpp`
   - Function: `get_burn_address()`

### Method 3: Compare with Genesis Block

1. Extract address from genesis block (height 0)
2. Run `./midas-gen-burn-address`
3. Compare - they must match exactly

## What to Verify

✅ **Seed string** is public in source code  
✅ **Address** matches genesis block  
✅ **Public keys** are valid Curve25519 points  
✅ **Generation process** is documented and verifiable  

## Expected Output

When you run `./midas-gen-burn-address`, you'll see:

```
Burn Address (Base58): MIDAS1burn...
Spend Public Key (hex): a1b2c3d4...
View Public Key (hex): f6e5d4c3...
```

These values should match what's in the genesis block.

## Why This Proves No Private Key Exists

1. Address is generated from **hash**, not from private key
2. To find private key, you'd need to solve **discrete logarithm problem**
3. This is **computationally impossible**
4. Therefore, **private key never existed**

## Full Documentation

- Detailed guide: `docs/BURN_ADDRESS_VERIFICATION.md`
- Presentation: `docs/BURN_ADDRESS_PRESENTATION.md`
