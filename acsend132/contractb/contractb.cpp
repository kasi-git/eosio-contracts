#include <eosiolib/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] contractb : public eosio::contract {
  public:
    using contract::contract;

    contractb (name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void callb(name user, std::string type) {
        //eosio::print("contracta::callme called");
 
        action(
            permission_level{get_self(),"active"_n},
            "contracta"_n,
            "callme"_n,
            std::make_tuple(user, type)
    ).send();



    }
};

EOSIO_DISPATCH( contractb, (callb));

