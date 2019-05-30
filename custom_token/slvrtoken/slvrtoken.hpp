/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>

using namespace eosio;

namespace ampersand {

    using std::string;

    CONTRACT slvrtoken : public eosio::contract {

    public:
	using contract::contract;

        const name SLVRTOKEN_CONTRACT_ACCNAME = name("ampervstoken");
        const name DRTOKEN_CONTRACT_ACCNAME = name("amperdrtoken");
        const string DR_TOKEN_NAME = "ANDS";
        const uint8_t DR_TOKEN_PRECISION = 4;

        slvrtoken(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): 
              eosio::contract(receiver, code, ds),  _issues(receiver, code.value), _customers(receiver, code.value)
        {}

        ACTION issueopen( asset issue, name issuer, uint64_t round );

        ACTION issueclose( asset issue, uint64_t round );

        ACTION create( name issuer, asset new_supply, uint16_t slvr_per_token_mg, 
         uint64_t issue_round, bool transfer_locked = true, bool redeem_locked = true, bool contract_locked = false );

        ACTION issue( name to, asset quantity, string memo, uint64_t issue_round );

        ACTION tokenlock( asset lock );

        ACTION tokenunlock( asset unlock );

        ACTION lock( asset lock, uint64_t issue_round );

        ACTION unlock( asset unlock, uint64_t issue_round );

        ACTION redeemlock( asset lock, uint64_t issue_round );

        ACTION redeemunlock( asset unlock, uint64_t issue_round );

        ACTION transfer( name from, name to,
                         asset quantity, string memo );

        ACTION redeem( name owner, asset quantity );

        ACTION burn( name owner, asset quantity );

        inline asset get_supply( symbol sym )const;

        inline asset get_balance( name owner, symbol sym )const;

    private:

        TABLE account {
            asset balance;

            uint64_t primary_key()const { return balance.symbol.raw(); }

            EOSLIB_SERIALIZE( account, (balance) )
        };

        TABLE currency_stats {
            asset supply;
            asset total_supply;
            name issuer;
            uint16_t slvr_per_token_mg; // # of milligrams of silver per token
            bool contract_locked;

            uint64_t primary_key()const { return supply.symbol.raw(); }

            EOSLIB_SERIALIZE( currency_stats, (supply)(total_supply)(issuer)
                               (slvr_per_token_mg)(contract_locked) )
        };

        TABLE issuestats {
            uint64_t round;
            asset supply;
            asset total_supply;
            name issuer;
            uint16_t slvr_per_token_mg;
            bool transfer_locked;
            bool redeem_locked;
            bool open_status;

            uint64_t primary_key()const { return round; }

            EOSLIB_SERIALIZE( issuestats, (round)(supply)(total_supply)(issuer)(slvr_per_token_mg)
                                     (transfer_locked)(redeem_locked)(open_status) )
        };

        TABLE custinfo {
            uint64_t key;
            name account_name;
            uint64_t issue_round;
            asset issue_balance;
            uint64_t primary_key() const { return key; }
            EOSLIB_SERIALIZE( custinfo, (key)(account_name)(issue_round)(issue_balance) )
       };

        typedef eosio::multi_index<"accounts"_n, account> accounts;
        typedef eosio::multi_index<"stats"_n, currency_stats> stats;
        typedef eosio::multi_index<"issues"_n, issuestats> issues;
        typedef eosio::multi_index<"customers"_n, custinfo> customers;

        issues _issues;
        customers _customers;

        void sub_balance( name owner, asset value, int64_t locked_balance );
        void add_balance( name owner, asset value, name ram_payer );
        int64_t get_transfer_locked_issues_balance( name owner );
        int64_t get_redeem_locked_issues_balance( name owner );
        void transfer_update_issue_customer_tables( name from, name to, asset value );
        void redeem_update_issue_customer_tables( name from, asset value );
        void purge_data( name owner );
            
    public:

        struct transfer_args {
            name from;
            name to;
            asset quantity;
            string memo;
        };

    };

    asset slvrtoken::get_supply( symbol sym )const
    {
        stats statstable( _code, sym.raw() );
        const auto& st = statstable.get( sym.raw() );
         return st.supply;
    }

    asset slvrtoken::get_balance( name owner, symbol sym )const
    {
        accounts accountstable( _code, owner.value );
        const auto& ac = accountstable.get( sym.raw() );
        return ac.balance;
    }

} /// namespace ampersand
