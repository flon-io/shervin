
//
// specifying shervin
//
// Sat Dec  6 17:31:49 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "auth"
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

  describe "shv_basic_auth_filter()"
  {
    it "authentifies"
    {
      expect(0 == 1);
    }
//    it "serves files"
//    {
//      req = shv_parse_request_head(
//        "GET /x/y/a/b/hello.txt HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/hello.txt"));
//
//      params = flu_d("root", "../spec/www", NULL);
//      int r = shv_dir_handler(req, res, params);
//
//      expect(r i== 1);
//
//      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/hello.txt");
//      expect(flu_list_get(res->headers, "shv_content_length") === ""
//        "12");
//      expect(flu_list_get(res->headers, "content-type") === ""
//        "text/plain");
//    }
  }
}

