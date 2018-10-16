#pragma once

#include <eosiolib/eosio.hpp>

using namespace eosio;

namespace ampr {

    class permissions : public contract {
    public:
        using contract::contract;

        [[eosio::action]]
		void hasauth(account_name account);

        [[eosio::action]]
        void reqauth(account_name account);

        [[eosio::action]]
        void reqauth2(account_name account, permission_name permission);

        [[eosio::action]]
        void reqauth3(account_name account, permission_name permission);

        [[eosio::action]]
        void send(account_name sent, permission_name p_sent, account_name req);

        [[eosio::action]]
        void send2(account_name sent, permission_name p_sent, account_name req, permission_name p_req);
    };
} // namespace ampr
