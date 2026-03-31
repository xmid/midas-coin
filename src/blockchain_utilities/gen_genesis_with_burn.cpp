// Utility to generate new GENESIS_TX with burn address
// Usage: gen_genesis_with_burn [miner_address] [burn_amount]
// If miner_address not provided, all coins go to burn address
// If burn_amount not provided, all coins go to burn address

#include <iostream>
#include <string>
#include <vector>
#include "include_base_utils.h"
#include "string_tools.h"
using namespace epee;

#include "cryptonote_core/cryptonote_tx_utils.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_basic/cryptonote_basic_impl.h"
#include "cryptonote_config.h"
#include "common/util.h"

int main(int argc, char* argv[])
{
  cryptonote::network_type nettype = cryptonote::MAINNET;
  
  // Get burn address
  cryptonote::account_public_address burn_addr = cryptonote::get_burn_address();
  std::string burn_addr_str = cryptonote::get_burn_address_str(nettype);
  
  std::cout << "=== Genesis Transaction Generator with Burn Address ===" << std::endl;
  std::cout << "Burn Address: " << burn_addr_str << std::endl;
  std::cout << std::endl;
  
  // Calculate genesis block reward
  uint64_t block_reward;
  if (!cryptonote::get_block_reward(0, 0, 0, block_reward, 1))
  {
    std::cerr << "Failed to get block reward" << std::endl;
    return 1;
  }
  
  std::cout << "Genesis Block Reward: " << block_reward << " atomic units" << std::endl;
  std::cout << std::endl;
  
  cryptonote::account_public_address miner_address = burn_addr; // Default: all to burn
  uint64_t burn_amount = block_reward; // Default: all to burn
  uint64_t miner_amount = 0;
  
  if (argc > 1)
  {
    std::string miner_addr_str = argv[1];
    
    // Parse miner address
    cryptonote::address_parse_info info;
    if (!cryptonote::get_account_address_from_str(info, nettype, miner_addr_str))
    {
      std::cerr << "Failed to parse miner address: " << miner_addr_str << std::endl;
      return 1;
    }
    
    if (info.is_subaddress)
    {
      std::cerr << "Warning: Address is a subaddress. Genesis should use main address." << std::endl;
    }
    
    miner_address = info.address;
    std::cout << "Miner Address: " << miner_addr_str << std::endl;
    
    // If burn amount specified
    if (argc > 2)
    {
      try
      {
        burn_amount = std::stoull(argv[2]);
        if (burn_amount > block_reward)
        {
          std::cerr << "Error: Burn amount (" << burn_amount << ") exceeds block reward (" << block_reward << ")" << std::endl;
          return 1;
        }
        miner_amount = block_reward - burn_amount;
      }
      catch (const std::exception& e)
      {
        std::cerr << "Failed to parse burn amount: " << argv[2] << std::endl;
        return 1;
      }
    }
    else
    {
      // Default: all to burn
      burn_amount = block_reward;
      miner_amount = 0;
    }
  }
  else
  {
    std::cout << "No miner address specified - all coins will go to burn address" << std::endl;
  }
  
  std::cout << "Burn Amount: " << burn_amount << " atomic units" << std::endl;
  if (miner_amount > 0)
  {
    std::cout << "Miner Amount: " << miner_amount << " atomic units" << std::endl;
  }
  std::cout << std::endl;
  
  // Create genesis transaction manually
  cryptonote::transaction genesis_tx;
  genesis_tx.vin.clear();
  genesis_tx.vout.clear();
  genesis_tx.extra.clear();
  
  // Create tx key
  cryptonote::keypair txkey = cryptonote::keypair::generate(hw::get_device("default"));
  cryptonote::add_tx_pub_key_to_extra(genesis_tx, txkey.pub);
  if (!cryptonote::sort_tx_extra(genesis_tx.extra, genesis_tx.extra))
  {
    std::cerr << "Failed to sort tx extra" << std::endl;
    return 1;
  }
  
  // Create genesis input
  cryptonote::txin_gen in;
  in.height = 0;
  
  // Create outputs
  uint8_t hard_fork_version = 1; // Genesis uses v1
  // Genesis uses v1, so no view tags (HF_VERSION_VIEW_TAGS = 15)
  bool use_view_tags = false;
  
  size_t output_index = 0;
  
  // Output to burn address
  if (burn_amount > 0)
  {
    crypto::key_derivation derivation = AUTO_VAL_INIT(derivation);
    crypto::public_key out_eph_public_key = AUTO_VAL_INIT(out_eph_public_key);
    
    bool r = crypto::generate_key_derivation(burn_addr.m_view_public_key, txkey.sec, derivation);
    if (!r)
    {
      std::cerr << "Failed to generate_key_derivation for burn address" << std::endl;
      return 1;
    }
    
    r = crypto::derive_public_key(derivation, output_index, burn_addr.m_spend_public_key, out_eph_public_key);
    if (!r)
    {
      std::cerr << "Failed to derive_public_key for burn address" << std::endl;
      return 1;
    }
    
    crypto::view_tag view_tag;
    if (use_view_tags)
      crypto::derive_view_tag(derivation, output_index, view_tag);
    
    cryptonote::tx_out out;
    cryptonote::set_tx_out(burn_amount, out_eph_public_key, use_view_tags, view_tag, out);
    genesis_tx.vout.push_back(out);
    output_index++;
  }
  
  // Output to miner address (if any)
  if (miner_amount > 0)
  {
    crypto::key_derivation derivation = AUTO_VAL_INIT(derivation);
    crypto::public_key out_eph_public_key = AUTO_VAL_INIT(out_eph_public_key);
    
    bool r = crypto::generate_key_derivation(miner_address.m_view_public_key, txkey.sec, derivation);
    if (!r)
    {
      std::cerr << "Failed to generate_key_derivation for miner address" << std::endl;
      return 1;
    }
    
    r = crypto::derive_public_key(derivation, output_index, miner_address.m_spend_public_key, out_eph_public_key);
    if (!r)
    {
      std::cerr << "Failed to derive_public_key for miner address" << std::endl;
      return 1;
    }
    
    crypto::view_tag view_tag;
    if (use_view_tags)
      crypto::derive_view_tag(derivation, output_index, view_tag);
    
    cryptonote::tx_out out;
    cryptonote::set_tx_out(miner_amount, out_eph_public_key, use_view_tags, view_tag, out);
    genesis_tx.vout.push_back(out);
  }
  
  // Set transaction properties
  genesis_tx.version = 1; // Genesis uses v1
  genesis_tx.unlock_time = 0 + CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW;
  genesis_tx.vin.push_back(in);
  genesis_tx.invalidate_hashes();
  
  // Convert to hex
  cryptonote::blobdata tx_blob = cryptonote::tx_to_blob(genesis_tx);
  std::string hex_tx = epee::string_tools::buff_to_hex_nodelimer(tx_blob);
  
  std::cout << "\n=== New GENESIS_TX (hex) ===" << std::endl;
  std::cout << hex_tx << std::endl;
  std::cout << "\n=== Summary ===" << std::endl;
  std::cout << "Burn Address: " << burn_addr_str << std::endl;
  std::cout << "Burn Amount: " << burn_amount << " atomic units" << std::endl;
  if (miner_amount > 0)
  {
    std::cout << "Miner Amount: " << miner_amount << " atomic units" << std::endl;
  }
  std::cout << "\n=== Instructions ===" << std::endl;
  std::cout << "1. Copy the hex string above" << std::endl;
  std::cout << "2. Replace GENESIS_TX in src/cryptonote_config.h (line ~243 for mainnet)" << std::endl;
  std::cout << "3. Delete blockchain database and restart daemon" << std::endl;
  std::cout << "4. Verify burn address in genesis block matches: " << burn_addr_str << std::endl;
  
  return 0;
}
