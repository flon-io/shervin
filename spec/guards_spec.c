
//
// specifying shervin
//
// Sat Apr 25 06:41:17 JST 2015
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "guards"
{
  before each
  {
    fshv_env *env = NULL;
  }
  after each
  {
    fshv_env_free(env);
  }

  describe "fshv_match()"
  {
    it "returns 0 when it doesn't match"
    {
      env = fshv_env_prepare(
        "GET /x?a=b#f HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = fshv_match(env, "GET /nowhere");

      ensure(r == 0);
    }

    it "matches an absolute route"
  }
}

