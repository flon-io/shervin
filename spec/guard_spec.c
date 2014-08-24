
//
// specifying shervin
//
// Sun Aug 24 15:53:36 JST 2014
//

#include "shervin.h"
//#include "shv_protected.h" // direct access to shv_request methods


context "guards"
{
  describe "shv_any_guard()"
  {
    it "always returns 1"
    {
      ensure(shv_any_guard(NULL, NULL) == 1);
    }
  }
}

