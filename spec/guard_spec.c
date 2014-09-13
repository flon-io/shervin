
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
      ensure(shv_any_guard(NULL, NULL) != NULL);
    }
  }

  describe "shv_path_guard()"
  {
    it "returns a path dict if the path matches"
    {
      shv_request *req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      ensure(shv_path_guard(req, "/x") != NULL);
      // TODO: continue me
    }

    it "returns NULL else"
    {
      shv_request *req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      ensure(shv_path_guard(req, "/y") == NULL);
    }
  }
}

