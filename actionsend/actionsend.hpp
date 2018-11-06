#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

using namespace eosio;

class actionsend: public contract {
public:
    actionsend(account_name _self): contract(_self) {}

	[[eosio::action]]
    void func();
};

