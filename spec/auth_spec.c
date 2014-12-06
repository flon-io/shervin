
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

  int specauth(const char *user, const char *pass, flu_dict *params)
  {
    return (strcmp(user, pass) == 0);
  }

  describe "shv_basic_auth_filter()"
  {
    it "authentifies (hit)"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic dG90bzp0b3Rv\r\n"
        "\r\n");

      params = flu_d("func", specauth, NULL);

      int r = shv_basic_auth_filter(req, res, params);

      expect(r i== 0); // continue routing
      expect(flu_list_get(req->routing_d, "_user") === "toto");
    }

    it "authentifies (miss)"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic nadanadanada\r\n"
        "\r\n");

      params = flu_d("func", specauth, NULL);

      int r = shv_basic_auth_filter(req, res, params);

      expect(r i== 1); // stop routing

      expect(res->status_code i== 401);
      expect(flu_list_get(req->routing_d, "_user") == NULL);

      expect(flu_list_get(res->headers, "WWW-Authenticate") === ""
        "Basic realm=\"shervin\"");
    }

    it "accepts a 'realm' parameter"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic nadanadanada\r\n"
        "\r\n");

      params = flu_d("func", specauth, "realm", "wonderland", NULL);

      int r = shv_basic_auth_filter(req, res, params);

      expect(r i== 1); // stop routing

      expect(res->status_code i== 401);
      expect(flu_list_get(req->routing_d, "_user") == NULL);

      expect(flu_list_get(res->headers, "WWW-Authenticate") === ""
        "Basic realm=\"wonderland\"");
    }
  }
}

