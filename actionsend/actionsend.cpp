#include "actionsend.hpp"

void actionsend::func() {
    require_auth(_self);
	eosio::print("func");
    action(
        permission_level {_self,N(active)},
        N(acc1),N(transfer),
        std::make_tuple(_self,N(acc1),asset(10,symbol_type(S(4,TOKN))),std::string(""))
    ).send();
}

EOSIO_ABI( actionsend,(func) )
