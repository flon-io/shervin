
//
// specifying shervin
//
// Sun Sep 21 15:35:29 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods

#include "servman.h"


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

  it "flips burgers"
  {
    ensure(1 == 1);
  }
}

