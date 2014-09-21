
//
// specifying shervin
//
// Sun Sep 21 15:35:29 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods

#include "servman.h"
#include "bocla.h"


context "live"
{
  before all
  {
    server_start();
  }
  after all
  {
    server_stop();
  }

  describe "shervin"
  {
    before each
    {
      fcla_response *res = NULL;
    }
    after each
    {
      if (res) fcla_response_free(res);
    }

    it "serves /hello/toto"
    {
      res = fcla_get("http://127.0.0.1:4001/hello/toto");

      ensure(res->body === "hello toto\n");
    }
  }
}

