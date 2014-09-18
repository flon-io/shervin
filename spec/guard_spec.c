
//
// specifying shervin
//
// Sun Aug 24 15:53:36 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "guards"
{
  before each
  {
    shv_request *req = NULL;
    flu_dict *d = NULL;
  }
  after each
  {
    if (req != NULL) shv_request_free(req);
    if (d != NULL) flu_list_free_all(d);
  }

  describe "shv_any_guard()"
  {
    it "provides the query string and fragment"
    {
      req = shv_parse_request(""
        "GET /x?a=b#f HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_any_guard(req, NULL);
      ensure(d != NULL);
      ensure(d->size == 0);
    }
  }

  describe "shv_path_guard()"
  {
    it "returns a(n empty) dict if the path matches"
    {
      req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/x");
      ensure(d != NULL);
      ensure(d->size == 0);
    }

    it "returns a dict for /book/:name"
    {
      req = shv_parse_request(""
        "GET /book/anna_karenine HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/book/:name");
      ensure(d != NULL);
      ensure(d->size == 1);
      ensure(flu_list_get(d, "name") === "anna_karenine");
    }

    it "includes the query string in the dict"
    {
      req = shv_parse_request(""
        "GET /book/anna_karenine?v=2.0 HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/book/:name");
      ensure(d != NULL);
      ensure(d->size == 1);
      ensure(flu_list_get(d, "name") === "anna_karenine");
    }

    it "fails if the path is too short"
    {
      req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/x/y");
      ensure(d == NULL);
    }

    it "fails if the path is too long"
    {
      req = shv_parse_request(""
        "GET /x/y HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/x");
      ensure(d == NULL);
    }

    it "returns NULL else"
    {
      req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      ensure(shv_path_guard(req, "/y") == NULL);
    }
  }
}

