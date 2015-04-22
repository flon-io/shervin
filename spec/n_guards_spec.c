
//
// specifying shervin
//
// Wed Apr 22 07:18:13 JST 2015
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "n guards"
{
  before each
  {
    fshv_env *env = NULL;
  }
  after each
  {
    fshv_env_free(env);
  }

  describe "fshv_match_route()"
  {
    it "flips burgers"
    {
      env = fshv_prepare_env(
        "GET /x HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");
      int r = fshv_match_route(env, "GET /x");
      ensure(r == 1);
      ensure(req->routing_d->size == 0);
    }
  }
}

