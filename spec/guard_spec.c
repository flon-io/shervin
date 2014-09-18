
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
    flu_dict *params = NULL;
    flu_dict *d = NULL;
  }
  after each
  {
    if (req != NULL) shv_request_free(req);
    if (params != NULL) flu_list_free(params);
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

      d = shv_any_guard(req, NULL, NULL, NULL);
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

      params = flu_d("path", "/x", NULL);
      d = shv_path_guard(req, NULL, NULL, params);
      ensure(d != NULL);
      ensure(d->size == 0);
    }

    it "returns a dict for /book/:name"
    {
      req = shv_parse_request(""
        "GET /book/anna_karenine HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/book/:name", NULL);
      d = shv_path_guard(req, NULL, NULL, params);
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

      params = flu_d("path", "/book/:name", NULL);
      d = shv_path_guard(req, NULL, NULL, params);
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

      params = flu_d("path", "/x/y", NULL);
      d = shv_path_guard(req, NULL, NULL, params);
      ensure(d == NULL);
    }

    it "fails if the path is too long"
    {
      req = shv_parse_request(""
        "GET /x/y HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/x", NULL);
      d = shv_path_guard(req, NULL, NULL, params);
      ensure(d == NULL);
    }

    it "returns NULL else"
    {
      req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/y", NULL);
      d = shv_path_guard(req, NULL, NULL, params);

      ensure(d == NULL);
    }
  }
}

