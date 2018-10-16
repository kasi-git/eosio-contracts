#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>

namespace ampr {

  enum Role {
    /**
     * Token holder, can hold tokens and digital rights
     */
    HOLDER = 0,
    COUPLER = 1,
    PRODUCER = 2,
    STORAGE = 3,
  };

  //@abi table
  struct [[eosio::table]] holderdata {

    account_name owner;

    uint128_t rights_balance;
    
    uint128_t token_balance;

    char rolenum;

    uint64_t primary_key() const { return owner; }
    
    EOSLIB_SERIALIZE(holderdata, (owner)(rights_balance)(token_balance)(rolenum))
  };

  //@abi table
  struct storagedata {

    account_name owner;

    uint128_t total_assets;

    uint128_t coupled_assets;

    uint64_t primary_key() const { return owner; }

    EOSLIB_SERIALIZE(storagedata, (owner)(total_assets)(coupled_assets))
  };

  const char * ROLENAME(Role role) {
    const char * return_val = "ERROR";
    switch (role) {

    case COUPLER:
      return_val = "COUPLER";
      break;

    case PRODUCER:
      return_val = "PRODUCER";
      break;

    case STORAGE:
      return_val = "STORAGE";
      break;

    }

    return return_val;
  }

  const char * ROLENAME(char rolenum) {
    return ROLENAME((Role) rolenum);
  }
  
  class ampr_contract : public eosio::contract {
  public:
    ampr_contract(account_name self):eosio::contract(self){}
    
    //@abi action
	[[eosio::action]]
    void createholder(account_name created_by, account_name account);

    //@abi action
    void checkholder(account_name account);

    //@abi action
    void checkstorage(account_name account);
    
    //@abi action
    void createrights(account_name account, uint128_t rights);
    
    //@abi action
    void sendrights(account_name from, account_name to, uint128_t rights);

    //@abi action
    void sendtokens(account_name from, account_name to, uint128_t tokens);

    //@abi action
    void setrole(account_name set_by, account_name account, char rolenum);

    //@abi action
    void deposit(account_name deposit_by, account_name account, uint128_t quantity);

    //@abi action
    void couple(account_name coupler, account_name storage, account_name account, uint128_t quantity);
    
  private:
    auto has_holder(const account_name account);
    
    auto get_holder(const account_name account);

    auto has_storage(const account_name account);

    auto get_storage(const account_name account);
    
    void require_role(account_name account, Role role);
    
    typedef eosio::multi_index<N(holderdata), holderdata> holdertable;

    typedef eosio::multi_index<N(storagedata), storagedata> storagetable;
    
    //    static holdertable _holders;
  };
}

