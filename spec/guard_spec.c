
//
// specifying shervin
//
// Sun Aug 24 15:53:36 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "guards"
{
  describe "shv_any_guard()"
  {
    it "always returns 1"
    {
      ensure(shv_any_guard(NULL, NULL) == 1);
    }
  }

  describe "shv_path_guard()"
  {
    it "returns 1 if the path matches"
    {
      shv_request *req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      ensure(shv_path_guard(req, "/x") == 1);
    }

    it "returns 0 else"
    {
      shv_request *req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      ensure(shv_path_guard(req, "/y") == 0);
    }
  }
}

