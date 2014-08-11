
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
    it "parses GET requests"
    {
      req = shv_parse_request(""
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
      req = shv_parse_request(""
        "POST /msgbin HTTP/1.1\r\n"
        "content-type: application/x-www-form-urlencoded;charset=utf-8\r\n"
        "host: https://importexport.amazonaws.com\r\n"
        "content-length: 207\r\n"
        "\r\n"
      );

      ensure(req != NULL);

      ensure(req->method == 'p');

      ensure(req->uri === "/msgbin");

      ensure(req->headers[0] === ""
        "content-type");
      ensure(req->headers[1] === ""
        " application/x-www-form-urlencoded;charset=utf-8");
      ensure(req->headers[4] === ""
        "content-length");
      ensure(req->headers[5] === ""
        " 207");

      ensure(req->status_code == 200);
    }

    it "returns a req with ->status_code == 400 when it cannot parse"
    {
      req = shv_parse_request(""
        "GET /\r\n"
        "\r\n"
      );

      ensure(req != NULL);
      ensure(req->status_code == 400);
    }
  }
}

