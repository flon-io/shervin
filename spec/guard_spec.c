
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
    it "never returns NULL"
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

      flu_dict *d = shv_path_guard(req, "/x");
      ensure(d != NULL);
      ensure(d->size == 0);
    }

    it "returns a path dict for /book/:name"
    {
      shv_request *req = shv_parse_request(""
        "GET /book/anna_karenine HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_dict *d = shv_path_guard(req, "/book/:name");
      ensure(d != NULL);
      ensure(d->size == 1);
      ensure(flu_list_get(d, "name") === "anna_karenine");
    }
    // TODO: unescape URIs? anna%20karenine?
    // TODO: query string

    it "fails if the path is too short"
    {
      shv_request *req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_dict *d = shv_path_guard(req, "/x/y");
      ensure(d == NULL);
    }

    it "fails if the path is too long"
    {
      shv_request *req = shv_parse_request(""
        "GET /x/y HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_dict *d = shv_path_guard(req, "/x");
      ensure(d == NULL);
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

