#include <eosiolib/eosio.hpp>
using namespace eosio;

class hello : public eosio::contract {
  public:
      using contract::contract;

      [[eosio::action]]
      void hi( account_name user ) {
		 acctable actable( _self, _self );
		 uint128_t count=1;
		 auto it = actable.find( user );
		 if (it == actable.end()) {
		    actable.emplace( _self, [&]( auto& ac ) {
			   ac.owner = user;
			   ac.count = 1;
			}); 
		 } else {
		    actable.modify( it, _self, [&]( auto& ac ) {
			   ac.count = ac.count + 1;
			   count = ac.count;
			}); 
		 }

         print( "Hello, ", name{user}, " Visit Count:", count );
      }

  private:
	  struct [[eosio::table]]  account {
         account_name owner;
		 uint128_t count;

         uint64_t primary_key()const { return owner; }

		 EOSLIB_SERIALIZE( account, ( owner )( count ) )
      };
	  typedef eosio::multi_index<N(accounts), account> acctable;
};
EOSIO_ABI( hello, (hi) )
