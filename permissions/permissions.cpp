#include "permissions.hpp"
#include "eosiolib/action.hpp"

namespace ampr {

    void permissions::hasauth(account_name account) {
        eosio::print(has_auth(account));
    }

    void permissions::reqauth(account_name account) {
        require_auth(account);
    }

    void permissions::reqauth2(account_name account, permission_name permission) {
        require_auth2(_self, N(subactive));
        eosio::print(name{account});
        eosio::print("@");
        eosio::print(name{permission});
    }

    void permissions::reqauth3(account_name account, permission_name permission) {
        require_auth2(N(acc2), N(subactive));
        require_auth2(N(acc4), N(subactive));
        require_auth2(N(acc3), N(active));
        //require_auth2(account, permission);
        eosio::print(name{account});
        eosio::print("@");
        eosio::print(name{permission});
    }

    void permissions::send(account_name sent, permission_name p_sent, account_name req) {
        action(permission_level{sent, p_sent},
               N(test), N(reqauth),
               std::make_tuple(req)
        ).send();
    }

    void permissions::send2(account_name sent, permission_name p_sent, account_name req, permission_name p_req) {
        action(permission_level{sent, p_sent},
               N(test), N(reqauth2),
               std::make_tuple(req, p_req)
        ).send();
    }
}

EOSIO_ABI(ampr::permissions, (hasauth)(reqauth)(reqauth2)(send)(send2))

