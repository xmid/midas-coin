// Utility to generate new GENESIS_TX with custom miner address
// Usage: gen_new_genesis <address>

#include <iostream>
#include <string>
#include "include_base_utils.h"
#include "string_tools.h"
using namespace epee;

#include "cryptonote_core/cryptonote_tx_utils.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_basic/cryptonote_basic_impl.h"
#include "common/util.h"

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <miner_address>" << std::endl;
    std::cerr << "Example: " << argv[0] << " 66xy9F3nCWmSxpzzmbiLaW6ssRqJdMxLEEJxs5ZLPjAHRnbumnAy2VyL5TmvDCYDM22Ct9PMBriuiDDETJhRc3fg2HSuzXe" << std::endl;
    return 1;
  }

  std::string address_str = argv[1];

  // Parse address
  cryptonote::address_parse_info info;
  if (!cryptonote::get_account_address_from_str(info, cryptonote::MAINNET, address_str))
  {
    std::cerr << "Failed to parse address: " << address_str << std::endl;
    return 1;
  }

  if (info.is_subaddress)
  {
    std::cerr << "Warning: Address is a subaddress. Genesis should use main address." << std::endl;
  }

  std::cout << "Generating genesis transaction for address: " << address_str << std::endl;

  // Create genesis miner transaction (height 0)
  cryptonote::transaction genesis_tx;
  bool r = cryptonote::construct_miner_tx(
    0,                    // height
    0,                    // median_weight
    0,                    // already_generated_coins
    0,                    // current_block_weight
    0,                    // fee
    info.address,         // miner_address
    genesis_tx,
    cryptonote::blobdata(), // extra_nonce
    999,                  // max_outs
    1                     // hard_fork_version (genesis uses v1)
  );

  if (!r)
  {
    std::cerr << "Failed to construct miner tx" << std::endl;
    return 1;
  }

  // Convert to hex
  cryptonote::blobdata tx_blob = cryptonote::tx_to_blob(genesis_tx);
  std::string hex_tx = epee::string_tools::buff_to_hex_nodelimer(tx_blob);

  std::cout << "\n=== New GENESIS_TX (hex) ===" << std::endl;
  std::cout << hex_tx << std::endl;
  std::cout << "\n=== Instructions ===" << std::endl;
  std::cout << "1. Copy the hex string above" << std::endl;
  std::cout << "2. Replace GENESIS_TX in src/cryptonote_config.h (line 240 for mainnet)" << std::endl;
  std::cout << "3. Delete blockchain database and restart daemon" << std::endl;
  std::cout << "4. The genesis block reward will be sent to: " << address_str << std::endl;

  return 0;
}
