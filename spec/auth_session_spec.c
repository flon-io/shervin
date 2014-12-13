
//
// specifying shervin
//
// Fri Dec 12 22:32:08 JST 2014
//

#include "flutim.h"
#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "session auth:"
{
  before each
  {
    shv_request *req = NULL;
    flu_dict *params = NULL;
    shv_response *res = shv_response_malloc(200);

    params = flu_d(
      "a", sa_specauth,
      "n", "shervin.test",
      NULL);

    shv_session_add("toto", "abcdef123", flu_gets('u'));
  }
  after each
  {
    shv_request_free(req);
    flu_list_free(params);
    shv_response_free(res);

    shv_session_store_reset();
  }

  int sa_specauth(const char *user, const char *pass, flu_dict *params)
  {
    return (strcmp(user, pass) == 0);
  }

  describe "shv_session_auth_filter()"
  {
    it "authentifies (hit)"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123; o.ther=1234abc\r\n"
        "\r\n");

      int r = shv_session_auth_filter(req, res, params);

      expect(r i== 0); // continue routing
      expect(flu_list_get(req->routing_d, "_user") === "toto");

      //expect(shv_session_store()->size == 1);
      ////
      //shv_session *s = shv_session_store()->first->item;
      //expect(s->sid !== "abcdef123");
      //  // session entry simply got with new sid and new mtimeus

      expect(flu_list_get(res->headers, "set-cookie") === "xxx");
    }

    it "authentifies (hit, cookie last)"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
        "\r\n");

      int r = shv_session_auth_filter(req, res, params);

      expect(r i== 0); // continue routing
      expect(flu_list_get(req->routing_d, "_user") === "toto");

      expect(shv_session_store()->size == 1);

      expect(flu_list_get(res->headers, "set-cookie") === "xxx");
    }

    it "authentifies (miss, no cookie)"
    {
      req = shv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      int r = shv_session_auth_filter(req, res, params);

      expect(r i== 1); // stop routing

      expect(flu_list_get(res->headers, "set-cookie") == NULL);
    }
//    it "authentifies (miss)"
//    {
//      req = shv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "Authorization: Basic nadanadanada\r\n"
//        "\r\n");
//
//      params = flu_d("a", specauth, NULL);
//
//      int r = shv_basic_auth_filter(req, res, params);
//
//      expect(r i== 1); // stop routing
//
//      expect(res->status_code i== 401);
//      expect(flu_list_get(req->routing_d, "_user") == NULL);
//
//      expect(flu_list_get(res->headers, "WWW-Authenticate") === ""
//        "Basic realm=\"shervin\"");
//    }
  }
}

