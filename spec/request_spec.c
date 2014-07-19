
//
// specifying shervin
//
// Tue Jul 15 06:03:36 JST 2014
//

#include "shervin.h"


context "request"
{
  before each
  {
    shv_request *req = NULL;
  }
  after each
  {
    if (req != NULL) shv_request_free(req);
  }

  describe "shv_request_parse()"
  {
    it "returns a pointer to a shv_request struct"
    {
      req = shv_parse_request("GET / HTTP/1.0\r\n");

      ensure(req != NULL);
      ensure(req->method == 'g');
    }
  }
}

