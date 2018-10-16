#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>

// std namespaces
using std::string;

//boost namespaces

// eos namespaces
//using namespace eosio::asset;
//using namespace eosio::multi_index;
//using namespace eosio::require_auth;

using namespace eosio;

namespace ampr {
	class token : public contract {
		public:
			token( account_name self ):contract( self ){}
	
			// @abi action
			[[eosio::action]]
			void create( account_name issuer,
					     asset maximum_supply );

			// @abi action
			[[eosio::action]]
			void issue( account_name to,
					    asset quantity,
						string memo );

			// @abi action
			[[eosio::action]]
			void transfer ( account_name from,
							account_name to,
							asset quantity,
							string memo );

			// @abi action
			inline asset get_supply( symbol_name sym ) const;

			// @abi action
			inline asset get_balance( account_name owner,
									  symbol_name sym ) const;	
				
		private: 
			struct [[eosio::table]] account {
				asset balance;

				uint64_t primary_key() const { return balance.symbol.name(); }
			};
	
			struct  [[eosio::table]] currency_stats {
				asset supply;
				asset max_supply;
				account_name issuer;
				
				uint64_t primary_key() const { return supply.symbol.name(); }		
			};

			typedef multi_index<N(accounts), account> accounts;
			typedef multi_index<N(stats), currency_stats> stats;

			void sub_balance(account_name owner, asset value );

        	void add_balance(account_name owner, asset value,
                         account_name ram_payer);


		public:
			struct transfer_args {
				account_name from;
				account_name to;
				asset quantity;
				string memo;
			};
	};

	asset token::get_supply( symbol_name sym ) const{
		stats statstable( _self, sym );
		const auto& st = statstable.get( sym );
		return st.supply;
	}

	asset token::get_balance( account_name owner, symbol_name sym ) const{
		accounts accounts_table( owner, sym );
		const auto& at = accounts_table.get( sym );
		return at.balance;
	}
} // namespace ampr
