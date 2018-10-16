#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/dispatcher.hpp>
#include <eosiolib/multi_index.hpp>
#include "ampr.hpp"

using namespace eosio;
using namespace ampr;

/**
 * internal method to check to see if the given account has holder data
 *
 * @param account the account to check
 * @return true if the account has a holder
 */
auto ampr_contract :: has_holder(const account_name account) {
  eosio_assert(is_account(account), "account does not exist");

  holdertable holder_data(_self, _self);
  
  return (holder_data.find(account) != holder_data.end());
}

/**
 * internal method to check to fetch the holder data for the given account,
 * creating it if necessary.  The CPU/RAM cost for this operation is staked
 * by the contract owner.
 *
 * @param account the account for which to fetch the holder data
 * @return the holder data
 */
auto ampr_contract :: get_holder(const account_name account) {
  eosio_assert(is_account(account), "account does not exist");

  holdertable holder_data(_self, _self);
  
  auto iterator = holder_data.find(account);
  if (iterator == holder_data.end()) {
    holder_data.emplace(_self, [&](auto& holder) {
	holder.owner = account;
	holder.rights_balance = 0;
	holder.token_balance = 0;
    });
  }
  return holder_data.get(account);
}

/**
 * internal method to check to see if the given account has storage data
 *
 * @param account the account to check
 * @return true if the account has a storage
 */
auto ampr_contract :: has_storage(const account_name account) {
  eosio_assert(is_account(account), "account does not exist");

  storagetable storage_data(_self, _self);
  
  return (storage_data.find(account) != storage_data.end());
}

/**
 * internal method to check to fetch the storage data for the given account,
 * creating it if necessary.  The CPU/RAM cost for this operation is staked
 * by the contract owner.
 *
 * @param account the account for which to fetch the storage data
 * @return the storage data
 */
auto ampr_contract :: get_storage(const account_name account) {
  eosio_assert(is_account(account), "account does not exist");

  storagetable storage_data(_self, _self);
  
  auto iterator = storage_data.find(account);
  if (iterator == storage_data.end()) {
    storage_data.emplace(_self, [&](auto& storage) {
	storage.owner = account;
	storage.total_assets = 0;
	storage.coupled_assets = 0;
    });
  }
  return storage_data.get(account);
}

/**
 * internal method to require that the given account have the given role
 *
 * @param account the account to check
 * @param role the role required
 * @assert the given account must be the given role type
 */
void ampr_contract :: require_role(account_name account, Role role) {
  eosio_assert(is_account(account), "account does not exist");

  auto holder = get_holder(account);
  eosio_assert((holder.rolenum == (char) role), "account is not the proper role");
}

/**
 * public method to create a holder, if necessary, using resources staked by the
 * contract owner
 *
 * @param created_by the account creating the holder which must be a transaction signer
 * @param account the account for which a holder should be created
 * @assert created_by must be a coupler or contract owner, and must be a signer
 */
void ampr_contract :: createholder(account_name created_by, account_name account) {
  require_auth(created_by);
  if (created_by != _self) {
    require_role(created_by, Role::COUPLER);
  }
  auto holder = get_holder(account);
  print("Holder has ", holder.rights_balance, " rights and ",
	holder.token_balance, " tokens");
}

/**
 * public method to check to see if the given account has a holder, and to print
 * the contents of that holder if found
 *
 * @param account the account to check
 * @print the role and rights and token balance of the holder, or a not found message
 */
void ampr_contract :: checkholder(account_name account) {
  holdertable holder_data(_self, _self);

  auto iterator = holder_data.find(account);

  if (iterator == holder_data.end()) {
    print("Holder ", account, " not found.");
    return;
  }
  
  auto holder = holder_data.get(account);
  print("Holder ", account, " is role ", ROLENAME(holder.rolenum), " and has ",
	holder.rights_balance, " rights and ", holder.token_balance, " tokens");
}

/**
 * public method to check to see if the given account has storage data, and to print
 * the contents of that storage data if found
 *
 * @param account the account to check
 * @print the total and coupled assets in the storage, or a not found message
 */
void ampr_contract :: checkstorage(account_name account) {
  storagetable storage_data(_self, _self);

  auto iterator = storage_data.find(account);

  if (iterator == storage_data.end()) {
    print("Storage ", account, " not found.");
    return;
  }
  
  auto storage = storage_data.get(account);
  print("Storage ", account, " has ", storage.total_assets, " total and ",
	storage.coupled_assets, " coupled assets.");
}

/**
 * public method to create a number of digital rights and place them in the given
 * account.  costs for this transaction are staked by the contract owner
 *
 * @param account the coupler which is creating these rights and will receive them
 * @param rights the number of rights to create
 * @assert account must be a coupler
 */
void ampr_contract :: createrights(account_name account, uint128_t rights) {
  require_role(account, Role::COUPLER);

  holdertable holder_data(_self, _self);
  
  auto iterator = holder_data.find(account);
  holder_data.modify(iterator, _self, [&](auto& account) {
      account.rights_balance += rights;
  });

  print("Rights created");
}

/**
 * transfers rights from one account to another.  costs for this transaction are
 * paid by the account transfering the rights.  contract owner's stake will be
 * used to create the target holder, if necessary.
 *
 * @param from the account from which rights are transfered
 * @param to the account to which the rights are transfered
 * @param rights the number of rights to be transfered
 * @assert from and to are different accounts
 * @assert from account must be a signer on the transaction
 * @assert from account must have a holder with sufficient balance
 * @assert to account must be a valid account
 */
void ampr_contract :: sendrights(account_name from, account_name to, uint128_t rights) {
  eosio_assert(from != to, "cannot send to self");
  require_auth(from);
  eosio_assert(has_holder(from), "from account does not have holder");
  eosio_assert(is_account(to), "to account does not exist");

  holdertable holder_data(_self, _self);
  
  auto from_holder = get_holder(from);
  auto to_holder = get_holder(to);

  eosio_assert(from_holder.rights_balance >= rights, "insufficient balance");

  auto iterator = holder_data.find(from);
  holder_data.modify(iterator, from, [&](auto& holder) {
      holder.rights_balance -= rights;
  });
  
  iterator = holder_data.find(to);
  holder_data.modify(iterator, from, [&](auto& holder) {
      holder.rights_balance += rights;
  });
}

/**
 * transfers tokens from one account to another.  costs for this transaction are
 * paid by the account transfering the rights.  contract owner's stake will be
 * used to create the target holder, if necessary.
 *
 * @param from the account from which tokens are transfered
 * @param to the account to which the tokens are transfered
 * @param tokens the number of tokens to be transfered
 * @assert from and to are different accounts
 * @assert from account must be a signer on the transaction
 * @assert from account must have a holder with sufficient balance
 * @assert to account must be a valid account
 */
void ampr_contract :: sendtokens(account_name from, account_name to, uint128_t tokens) {
  eosio_assert(from != to, "cannot send to self");
  require_auth(from);
  eosio_assert(is_account(to), "to account does not exist");

  holdertable holder_data(_self, _self);
  
  auto from_holder = get_holder(from);
  auto to_holder = get_holder(to);

  eosio_assert(from_holder.token_balance >= tokens, "insufficient balance");

  auto iterator = holder_data.find(from);
  holder_data.modify(iterator, from, [&](auto& holder) {
      holder.token_balance -= tokens;
  });
  
  iterator = holder_data.find(to);
  holder_data.modify(iterator, from, [&](auto& holder) {
      holder.token_balance += tokens;
  });
}

/**
 * sets the role of the given account.  contract owner's stake will be used to
 * create this holder, if necessary, and to set the role number.
 *
 * @param set_by the account setting the role
 * @param account the account whose role should be set
 * @param rolenum the role to be set
 * @assert account cannot set its own role
 * @assert set_by account must be a signer
 * @assert account must exist
 */
void ampr_contract :: setrole(account_name set_by, account_name account, char rolenum) {
  eosio_assert(set_by != account, "cannot set own role");
  require_auth(set_by);
  eosio_assert(is_account(account), "account does not exist");
  
  if (set_by != _self) {
    require_role(set_by, Role::COUPLER);
  }

  get_holder(account);

  if (rolenum == (char) Role::STORAGE) {
    print("Creating storage... ");
    get_storage(account);
  }
  
  holdertable holder_data(_self, _self);

  auto iterator = holder_data.find(account);
  if (iterator == holder_data.end()) {
    eosio_assert(false, "account does not have holder data");
  }

  holder_data.modify(iterator, _self, [&](auto& holder) {
      holder.rolenum = rolenum;
  });

  print("Role set to ", ROLENAME(rolenum));
}

/**
 * deposits a given amount of assets into the given account, which must be a storage
 * location.
 *
 * @param deposit_by the account depositing the silver
 * @param account the account for the storage location getting the deposit
 * @param quantity the amount of assets being deposited
 * @assert account cannot deposit into own account
 * @assert deposit_by must be a signer
 * @assert account must be a signer
 * @assert quantity must be greater than zero
 * @assert deposit_by must be a coupler
 * @assert account must be a storage location
 */
void ampr_contract :: deposit(account_name deposit_by, account_name account, uint128_t quantity) {
  eosio_assert(deposit_by != account, "cannot deposit into own account");
  require_auth(deposit_by);
  require_auth(account);
  eosio_assert(quantity > 0, "quantity must be greater than zero");
  if (deposit_by != _self) {
    require_role(deposit_by, Role::COUPLER);
  }
  require_role(account, Role::STORAGE);

  storagetable storage_data(_self, _self);

  auto iterator = storage_data.find(account);
  if (iterator == storage_data.end()) {
    eosio_assert(false, "account does not have storage data");
  }

  storage_data.modify(iterator, _self, [&](auto& storage) {
      storage.total_assets += quantity;
  });
}

/**
 * couples a given amount of deposited silver, consuming digital rights and creating silver tokens.
 */
void ampr_contract :: couple(account_name coupler, account_name storage, account_name account, uint128_t quantity) {
  require_auth(coupler);
  require_auth(account);
  require_role(coupler, Role::COUPLER);
  require_role(storage, Role::STORAGE);
  eosio_assert(quantity > 0, "quantity must be greater than zero");
  eosio_assert(has_holder(account), "account does not have holder data");

  auto storage_record = get_storage(storage);
  auto holder_record = get_holder(account);

  eosio_assert(storage_record.total_assets - storage_record.coupled_assets >= quantity, "storage does not have enough uncoupled quantity");
  eosio_assert(holder_record.rights_balance >= quantity, "account does not have enough rights to couple the quantity");
  
  holdertable holder_data(_self, _self);
  storagetable storage_data(_self, _self);

  auto holder_iterator = holder_data.find(account);
  if (holder_iterator == holder_data.end()) {
    eosio_assert(false, "could not find holder data for account");
  }
  holder_data.modify(holder_iterator, _self, [&](auto& holder) {
      holder.rights_balance -= quantity;
      holder.token_balance += quantity;
  });

  auto storage_iterator = storage_data.find(storage);
  if (storage_iterator == storage_data.end()) {
    eosio_assert(false, "could not find storage data for storage");
  }
  storage_data.modify(storage_iterator, _self, [&](auto& storage) {
      storage.coupled_assets += quantity;
  });
}

EOSIO_ABI(ampr_contract, (createholder) (checkholder) (checkstorage) (createrights) (sendrights) (sendtokens) (setrole) (deposit) (couple))

