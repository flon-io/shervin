
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
  }
  after each
  {
    if (req != NULL) shv_request_free(req);
    if (params != NULL) flu_list_free(params);
  }

  describe "shv_any_guard()"
  {
    it "provides the query string and fragment"
    {
      req = shv_parse_request_head(
        "GET /x?a=b#f HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = shv_any_guard(req, NULL, NULL);
      ensure(r == 1);
      ensure(req->routing_d->size == 0);
    }
  }

  describe "shv_path_guard()"
  {
    it "returns 1 if the path matches"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/x", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
      ensure(req->routing_d->size == 0);
    }

    it "returns a dict for /book/:name"
    {
      req = shv_parse_request_head(
        "GET /book/anna_karenine HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/book/:name", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
      ensure(req->routing_d->size == 1);
      ensure(flu_list_get(req->routing_d, "name") === "anna_karenine");
    }

    it "includes the query string in the dict"
    {
      req = shv_parse_request_head(
        "GET /book/anna_karenine?v=2.0 HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/book/:name", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
      ensure(req->routing_d->size == 1);
      ensure(flu_list_get(req->routing_d, "name") === "anna_karenine");
    }

    it "fails if the path is too short"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/x/y", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 0);
    }

    it "fails if the path is too long"
    {
      req = shv_parse_request_head(
        "GET /x/y HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/x", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 0);
    }

    it "fails if the path is too long (/ vs /favicon.ico)"
    {
      req = shv_parse_request_head(
        "GET /favicon.ico HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 0);
    }

    it "accepts the HTTP method as prefix (miss)"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "POST /x", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 0);
    }

    it "accepts the HTTP method as prefix (hit)"
    {
      req = shv_parse_request_head(
        "POST /x HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "POST /x", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
    }

    it "accepts a /** ending (hit 0)"
    {
      req = shv_parse_request_head(
        "GET /x/y/z/bravo HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "GET /x/y/**", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
      ensure(req->routing_d->size == 1);
      ensure(flu_list_get(req->routing_d, "**") === "z/bravo");
    }

    it "accepts a /** ending (hit 1)"
    {
      req = shv_parse_request_head(
        "GET /x/y HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "GET /x/y/**", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
      ensure(req->routing_d->size == 1);
      ensure(flu_list_get(req->routing_d, "**") === "");
    }

    it "accepts a /** ending (miss)"
    {
      req = shv_parse_request_head(
        "GET /x/z/y/ HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "GET /x/y/**", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 0);
      ensure(req->routing_d->size == 0);
    }

    it "discards a final /"
    {
      req = shv_parse_request_head(
        "GET /x/ HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/x", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 1);
      ensure(req->routing_d->size == 0);
    }

    it "returns 0 else"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      params = flu_d("path", "/y", NULL);
      int r = shv_path_guard(req, NULL, params);

      ensure(r == 0);
    }
  }
}
