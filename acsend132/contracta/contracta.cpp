#include <eosiolib/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] contracta : public eosio::contract {
  public:
    using contract::contract;

    contracta (name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void callme(name user, std::string type) {
        eosio::print("contracta::callme called");
    }
};

EOSIO_DISPATCH( contracta, (callme));

