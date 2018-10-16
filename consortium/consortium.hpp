#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>

// eos namespaces
using namespace eosio::asset;
using namespace eosio::multi_index;
using namespace eosio::require_auth;

namespace ampr {
    
    using std::string;

    class consortium : public contract {
    public:
        consortium( account_name self ):contract(self){}

        [[eosio::action]]
        void deposit_silver( account_name producer, 
							 account_name storage,
							 account_name coupler,
                           uint64_t quantity );
                       
        [[eosio::action]]
		void validate_silver( 

        [[eosio::action]]
        void tokenize_silver( account_name coupler,
                              asset rights,
                              uint64_t quantity ); 

        [[eosio::action]]
        void create_token( account_name issuer,
                      asset maximum_supply );

        [[eosio::action]]
        void issue_token( account_name to, 
                     asset quantity, 
                     string memo );

        [[eosio::action]]
        void transfer_token( account_name from,
                        account_name to,
                        asset        quantity,
                        string       memo );

        [[eosio::action]]
        void burn( account_name from,
                   asset quantity );

        [[eosio::action]]
        void redeem_silver( account_name owner );

        inline asset get_supply( symbol_name sym )const;
         
        inline asset get_balance( account_name owner, 
                                   symbol_name sym )const;

    private:
        struct [[eosio::table]] account {
            asset balance;

            uint64_t primary_key()const { return balance.symbol.name(); }
        };

        struct [[eosio::table]] currency_stats {
            asset supply;
            asset max_supply;
            account_name issuer;

            uint64_t primary_key()const { return supply.symbol.name(); }
        };

        typedef eosio::multi_index<N(accounts), account> accounts;
        typedef eosio::multi_index<N(stat), currency_stats> stats;

        void sub_balance( account_name owner, asset value );
        void add_balance( account_name owner, asset value, account_name ram_payer );

    public:
        struct transfer_args {
            account_name from;
            account_name to;
            asset quantity;
            string memo;
        };
    };

    asset consortium::get_supply( symbol_name sym )const
    {
        stats statstable( _self, sym );
        const auto& st = statstable.get( sym );
        return st.supply;
    }

    asset consortium::get_balance( account_name owner, symbol_name sym )const
    {
        accounts accountstable( _self, owner );
        const auto& ac = accountstable.get( sym );
        return ac.balance;
    }

} // namespace ampr
