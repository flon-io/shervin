
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
        "GET /x/y/a/b HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      flu_list_set(req->routing_d, "**", rdz_strdup("a/b"));

      params = flu_d("root", "../spec/www", NULL);
      int r = shv_dir_handler(req, res, params);

      expect(r i== 1);

      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
        "../spec/www/a/b");
    }

    it "returns 404 if the file is not found"
    it "returns 404 when the request goes ../"
  }
}

