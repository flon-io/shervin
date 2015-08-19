
//
// specifying shervin
//
// Sat Dec  6 17:31:49 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "basic auth:"
{
  before each
  {
    fshv_env *env = NULL;
  }
  after each
  {
    fshv_env_free(env);
  }

  static int specauth(const char *user, const char *pass)
  {
    return (strcmp(user, pass) == 0);
  }

  describe "fshv_basic_auth()"
  {
    it "rejects non basic auth"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Complicated dG90bzp0b3Rv\r\n"
        "\r\n",
        NULL);

      int r = fshv_basic_auth(env, specauth);

      expect(r i== 0);
    }

    it "authentifies (hit)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic dG90bzp0b3Rv\r\n"
        "\r\n",
        NULL);

      int r = fshv_basic_auth(env, specauth);

      expect(r i== 1);
      expect(flu_list_get(env->bag, "_basic_user") === "toto");
    }

//    it "authentifies (miss)"
//    {
//      req = fshv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "Authorization: Basic nadanadanada\r\n"
//        "\r\n");
//
//      //params = flu_d("func", specauth, NULL);
//      params = flu_d("a", specauth, NULL);
//
//      int r = fshv_basic_auth_filter(req, res, 0, params);
//
//      expect(r i== 0); // handled -> 0
//
//      expect(res->status_code i== 401);
//      expect(flu_list_get(req->routing_d, "_basic_user") == NULL);
//
//      expect(flu_list_get(res->headers, "WWW-Authenticate") === ""
//        "Basic realm=\"shervin\"");
//    }
//
//    it "accepts a 'realm' parameter"
//    {
//      req = fshv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "Authorization: Basic nadanadanada\r\n"
//        "\r\n");
//
//      //params = flu_d("func", specauth, "realm", "wonderland", NULL);
//      params = flu_d("a", specauth, "realm", "wonderland", NULL);
//
//      int r = fshv_basic_auth_filter(req, res, 0, params);
//
//      expect(r i== 0); // handled -> 0
//
//      expect(res->status_code i== 401);
//      expect(flu_list_get(req->routing_d, "_basic_user") == NULL);
//
//      expect(flu_list_get(res->headers, "WWW-Authenticate") === ""
//        "Basic realm=\"wonderland\"");
//    }
  }
}

