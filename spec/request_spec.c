
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
    it "returns a pointer to a shv_request struct"
    {
      req = shv_parse_request(""
        "GET / HTTP/1.0\r\n"
        "\r\n"
      );

      ensure(req != NULL);
      ensure(req->method == 'g');
    }

    it "parses message headers"
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

      // TODO: access the headers and their values
    }
  }
}

