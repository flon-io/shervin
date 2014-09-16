
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
    if (d != NULL) flu_list_and_items_free(d, free);
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
      ensure(d->size == 3);
      ensure(flu_list_get(d, "_path") === "/x");
      ensure(flu_list_get(d, "a") === "b");
      ensure(flu_list_get(d, "_fragment") === "f");
    }
  }

  describe "shv_path_guard()"
  {
    it "returns a path dict if the path matches"
    {
      req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/x");
      ensure(d != NULL);
      ensure(d->size == 1);
      ensure(flu_list_get(d, "_path") === "/x");
    }

    it "returns a path dict for /book/:name"
    {
      req = shv_parse_request(""
        "GET /book/anna_karenine HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/book/:name");
      ensure(d != NULL);
      ensure(d->size == 2);
      ensure(flu_list_get(d, "_path") === "/book/anna_karenine");
      ensure(flu_list_get(d, "name") === "anna_karenine");
    }
    // TODO: unescape URIs? anna%20karenine?

    it "includes the query string in the dict"
    {
      req = shv_parse_request(""
        "GET /book/anna_karenine?v=2.0 HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      d = shv_path_guard(req, "/book/:name");
      ensure(d != NULL);
      ensure(d->size == 3);
      ensure(flu_list_get(d, "_path") === "/book/anna_karenine");
      ensure(flu_list_get(d, "name") === "anna_karenine");
      ensure(flu_list_get(d, "v") === "2.0");
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

