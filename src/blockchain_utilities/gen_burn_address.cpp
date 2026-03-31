// Utility to generate and display Genesis Burn Address information
// Usage: gen_burn_address [mainnet|testnet|stagenet]

#include <iostream>
#include <string>
#include "include_base_utils.h"
#include "string_tools.h"
using namespace epee;

#include "cryptonote_basic/cryptonote_basic_impl.h"
#include "cryptonote_config.h"
#include "common/util.h"

int main(int argc, char* argv[])
{
  cryptonote::network_type nettype = cryptonote::MAINNET;
  
  if (argc > 1)
  {
    std::string net_str = argv[1];
    if (net_str == "testnet")
      nettype = cryptonote::TESTNET;
    else if (net_str == "stagenet")
      nettype = cryptonote::STAGENET;
    else if (net_str != "mainnet")
    {
      std::cerr << "Usage: " << argv[0] << " [mainnet|testnet|stagenet]" << std::endl;
      std::cerr << "Default: mainnet" << std::endl;
      return 1;
    }
  }

  std::cout << "=== Genesis Burn Address Information ===" << std::endl;
  std::cout << "Network: ";
  switch (nettype)
  {
    case cryptonote::MAINNET: std::cout << "MAINNET"; break;
    case cryptonote::TESTNET: std::cout << "TESTNET"; break;
    case cryptonote::STAGENET: std::cout << "STAGENET"; break;
    default: std::cout << "UNKNOWN"; break;
  }
  std::cout << std::endl << std::endl;

  // Get burn address
  cryptonote::account_public_address burn_addr = cryptonote::get_burn_address();
  std::string burn_addr_str = cryptonote::get_burn_address_str(nettype);
  crypto::secret_key view_priv_key = cryptonote::get_burn_address_view_private_key();

  std::cout << "Seed String: " << config::BURN_ADDRESS_SEED << std::endl;
  std::cout << std::endl;

  std::cout << "Burn Address (Base58):" << std::endl;
  std::cout << "  " << burn_addr_str << std::endl;
  std::cout << std::endl;

  std::cout << "Spend Public Key (hex):" << std::endl;
  std::cout << "  " << string_tools::pod_to_hex(burn_addr.m_spend_public_key) << std::endl;
  std::cout << std::endl;

  std::cout << "View Public Key (hex):" << std::endl;
  std::cout << "  " << string_tools::pod_to_hex(burn_addr.m_view_public_key) << std::endl;
  std::cout << std::endl;

  std::cout << "View Private Key (hex) - PUBLIC, for scanning transactions:" << std::endl;
  // secret_key is mlocked<scrubbed<ec_scalar>>, use unwrap to get base type
  std::cout << "  " << string_tools::pod_to_hex(unwrap(unwrap(view_priv_key))) << std::endl;
  std::cout << std::endl;

  std::cout << "=== Verification Info ===" << std::endl;
  std::cout << "Generation method:" << std::endl;
  std::cout << "  1. Hash seed string with Keccak256 → seed_hash" << std::endl;
  std::cout << "  2. Convert seed_hash to point on Curve25519 using ge_fromfe_frombytes_vartime" << std::endl;
  std::cout << "  3. This point becomes spend_public_key" << std::endl;
  std::cout << "  4. Use hash_to_scalar(seed_hash) as view_private_key (deterministic, PUBLIC, valid scalar)" << std::endl;
  std::cout << "  5. Generate view_public_key = view_private_key * G (standard Ed25519)" << std::endl;
  std::cout << std::endl;
  std::cout << "Important: The spend private key for this address NEVER existed." << std::endl;
  std::cout << "It cannot be computed from the public key (discrete logarithm problem)." << std::endl;
  std::cout << std::endl;
  std::cout << "View private key is PUBLIC - anyone can use it to scan transactions." << std::endl;
  std::cout << "But it cannot be used to spend funds (spend_private_key doesn't exist)." << std::endl;
  std::cout << std::endl;
  std::cout << "=== How to Verify ===" << std::endl;
  std::cout << "1. Run: ./midas-verify-burn-address" << std::endl;
  std::cout << "2. Check genesis block - address must match" << std::endl;
  std::cout << "3. Verify public keys are valid Curve25519 points" << std::endl;
  std::cout << "4. See docs/BURN_ADDRESS_VERIFICATION.md for details" << std::endl;
  std::cout << std::endl;
  std::cout << "=== How to Monitor Burn Address ===" << std::endl;
  std::cout << "You can create a watch-only wallet using:" << std::endl;
  std::cout << "  - Burn address (Base58)" << std::endl;
  std::cout << "  - View private key (hex, shown above)" << std::endl;
  std::cout << std::endl;
  std::cout << "This allows anyone to scan and verify all incoming transactions" << std::endl;
  std::cout << "to the burn address, proving that coins are truly burned." << std::endl;

  return 0;
}
