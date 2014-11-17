
//
// specifying shervin
//
// Thu Nov  6 15:37:01 JST 2014
//

#include "shervin.h"
//#include "shv_protected.h" // direct access to shv_request methods


context "handlers"
{
  before each
  {
    shv_request *req = NULL;
    flu_dict *params = NULL;
    shv_response *res = shv_response_malloc(200);
  }
  after each
  {
    if (req != NULL) shv_request_free(req);
    if (params != NULL) flu_list_free(params);
    if (res != NULL) shv_response_free(res);
  }

  describe "shv_dir_handler()"
  {
    it "serves files"
    {
      req = shv_parse_request_head(""
        "GET /x/y/a/b/hello.txt HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/hello.txt"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 1);

      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
        "../spec/www/a/b/hello.txt");
      expect(flu_list_get(res->headers, "shv_content_length") === ""
        "12");
      expect(flu_list_get(res->headers, "content-type") === ""
        "text/plain");
    }

    it "returns 0 if the file is not found"
    {
      req = shv_parse_request_head(""
        "GET /x/y/a/b/nada.txt HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/nada.txt"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 0);
    }

    it "returns 0 when the file is a dir"
    {
      req = shv_parse_request_head(""
        "GET /x/y/a HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 0);
    }

    it "returns 0 when the request goes ../"
    {
      req = shv_parse_request_head(""
        "GET /x/y/../www/a/b/hello.txt HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("../www/a/b/hello.txt"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 0);
    }

    it "accepts an alternative sendfile \"h\" header via the params"
    {
      req = shv_parse_request_head(""
        "GET /x/y/a/b/hello.txt HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/hello.txt"));

      params = flu_d("r", "../spec/www", "h", "X-Sendfile", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 1);

      expect(flu_list_get(res->headers, "X-Accel-Redirect") == NULL);

      expect(flu_list_get(res->headers, "X-Sendfile") === ""
        "../spec/www/a/b/hello.txt");
      expect(flu_list_get(res->headers, "shv_content_length") === ""
        "12");
      expect(flu_list_get(res->headers, "content-type") === ""
        "text/plain");
    }

    it "serves a/b/index.html when asked for a/b"
    {
      req = shv_parse_request_head(""
        "GET /web/a/b HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/b"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 1);

      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
        "../spec/www/a/b/index.html");
      expect(flu_list_get(res->headers, "shv_content_length") === ""
        "13");
      expect(flu_list_get(res->headers, "content-type") === ""
        "text/html");
    }

    it "accepts alternative indexes"
    {
      req = shv_parse_request_head(""
        "GET /web/a/ HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/"));

      params = flu_d("root", "../spec/www", "index", "index.txt", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 1);

      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
        "../spec/www/a/index.txt");
      expect(flu_list_get(res->headers, "shv_content_length") === ""
        "21");
      expect(flu_list_get(res->headers, "content-type") === ""
        "text/plain");
    }

    it "defaults to text/plain for unknown filetypes"
    {
      req = shv_parse_request_head(""
        "GET /x/y/a/b/nada.nad HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/nada.nad"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 1);

      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
        "../spec/www/a/b/nada.nad");
      expect(flu_list_get(res->headers, "shv_content_length") === ""
        "10");
      expect(flu_list_get(res->headers, "content-type") === ""
        "text/plain");
    }
  }
}

