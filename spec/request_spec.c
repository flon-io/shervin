
//
// specifying shervin
//
// Tue Jul 15 06:03:36 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "request"
{
  before each
  {
    fshv_request *req = NULL;
  }
  after each
  {
    fshv_request_free(req);
  }

  describe "fshv_request_parse()"
  {
    it "parses GET requests"
    {
      req = fshv_parse_request_head(
        "GET / HTTP/1.0\r\n"
        "\r\n"
      );

      ensure(req != NULL);
      ensure(req->method == 'g');
      ensure(req->uri === "/");

      ensure(req->status_code == 200);
    }

    it "parses POST requests"
    {
      req = fshv_parse_request_head(
        "POST /msgbin HTTP/1.1\r\n"
        "content-type: application/x-www-form-urlencoded;charset=utf-8\r\n"
        "host: https://www.example.com\r\n"
        "content-length: 207\r\n"
        "\r\n"
      );

      ensure(req != NULL);

      ensure(req->method == 'p');

      ensure(req->uri === "/msgbin");

      ensure(req->headers->size == 3);
      //
      ensure(flu_list_get(req->headers, "content-type") === ""
        "application/x-www-form-urlencoded;charset=utf-8");
      ensure(flu_list_get(req->headers, "host") === ""
        "https://www.example.com");
      ensure(flu_list_get(req->headers, "content-length") === ""
        "207");

      ensure(req->status_code == 200);
    }

    it "lowers the case of header field names"
    {
      req = fshv_parse_request_head(
        "GET / HTTP/1.0\r\n"
        "Host: http://example.com\r\n"
        "X-Whatever: nada\r\n"
        "\r\n"
      );

      ensure(req != NULL);
      ensure(flu_list_get(req->headers, "host") === "http://example.com");
      ensure(flu_list_get(req->headers, "x-whatever") === "nada");
    }

    it "returns a req with ->status_code == 400 when it cannot parse"
    {
      req = fshv_parse_request_head(
        "GET /\r\n"
        "\r\n"
      );

      ensure(req != NULL);
      ensure(req->status_code == 400);
    }
  }

  describe "fshv_request_content_length()"
  {
    it "returns -1 when there is no content-length header"
    {
      req = fshv_parse_request_head(
        "GET /msg HTTP/1.1\r\n"
        "Host: https://example.com\r\n"
        "\r\n"
      );

      ensure(fshv_request_content_length(req) == -1);
    }

    it "returns the number for the content-length header"
    {
      req = fshv_parse_request_head(
        "POST /msgbin HTTP/1.1\r\n"
        "content-type: application/x-www-form-urlencoded;charset=utf-8\r\n"
        "host: https://importexport.amazonaws.com\r\n"
        "content-length: 207\r\n"
        "\r\n"
      );

      ensure(fshv_request_content_length(req) == 207);
    }
  }

  context "spec helpers"
  {
    describe "fshv_request_parse_head_f()"
    {
      it "composes and parses"
      {
        req = fshv_parse_request_head_f(
          "GET /%s HTTP/1.0\r\n"
          "\r\n",
          "nada"
        );

        ensure(req != NULL);
        ensure(req->method == 'g');
        ensure(req->uri === "/nada");

        ensure(req->status_code == 200);
      }
    }

    describe "fshv_do_route()"
    {
      it "fills req->routing_d"
      {
        req = fshv_parse_request_head_f(
          "GET /letters/%i HTTP/1.0\r\n"
          "Host: nada.example.org\r\n"
          "\r\n",
          12345
        );
        fshv_do_route("GET /letters/:id", req);

        expect(flu_list_get(req->routing_d, "id") === "12345");
      }
    }
  }
}

