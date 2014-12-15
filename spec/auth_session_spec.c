
//
// specifying shervin
//
// Fri Dec 12 22:32:08 JST 2014
//

#include "flutim.h"
#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "session auth:"
{
  before each
  {
    fshv_request *req = NULL;
    flu_dict *params = NULL;
    fshv_response *res = fshv_response_malloc(200);

    params = flu_d(
      "a", sa_specauth,
      "n", "shervin.test",
      NULL);

    fshv_session_add("toto", "toto:1234:4567", "abcdef123", flu_gets('u'));
  }
  after each
  {
    fshv_request_free(req);
    flu_list_free(params);
    fshv_response_free(res);

    fshv_session_store_reset();
  }

  int sa_specauth(const char *user, const char *pass, flu_dict *params)
  {
    return (strcmp(user, pass) == 0);
  }

  describe "fshv_session_auth_filter()"
  {
    it "authentifies (hit)"
    {
      req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123; o.ther=1234abc\r\n"
        "\r\n");
      req->startus = flu_gets('u');

      int r = fshv_session_auth_filter(req, res, params);

      expect(r i== 0); // continue routing
      expect(flu_list_get(req->routing_d, "_user") === "toto");

      expect(fshv_session_store()->size == 1);

      fshv_session *ses = fshv_session_store()->first->item;
      expect(ses->id === "toto:1234:4567");

      char *s = flu_list_get(res->headers, "set-cookie");
      expect(s ^== ses->sid);
      expect(s >== ";Expires=");
      expect(s >== ";HttpOnly");
      expect(strstr(s, ";Secure") == NULL);
    }

    it "authentifies (hit, cookie last)"
    {
      req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
        "\r\n");
      req->startus = flu_gets('u');

      int r = fshv_session_auth_filter(req, res, params);

      expect(r i== 0); // continue routing
      expect(flu_list_get(req->routing_d, "_user") === "toto");

      expect(fshv_session_store()->size == 1);

      fshv_session *ses = fshv_session_store()->first->item;
      expect(ses->id === "toto:1234:4567");

      char *s = flu_list_get(res->headers, "set-cookie");
      expect(s ^== ses->sid);
      expect(s >== ";Expires=");
      expect(s >== ";HttpOnly");
      expect(strstr(s, ";Secure") == NULL);
    }

    it "authentifies (hit, https)"
    {
      req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
        "X-Forwarded-Proto: https\r\n"
        "\r\n");
      req->startus = flu_gets('u');

      int r = fshv_session_auth_filter(req, res, params);

      expect(r i== 0); // continue routing
      expect(flu_list_get(req->routing_d, "_user") === "toto");

      expect(fshv_session_store()->size == 1);

      fshv_session *ses = fshv_session_store()->first->item;
      expect(ses->id === "toto:1234:4567");

      char *s = flu_list_get(res->headers, "set-cookie");
      expect(s $== ";Secure");
    }

    it "authentifies (miss, no cookie)"
    {
      req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");
      req->startus = flu_gets('u');

      int r = fshv_session_auth_filter(req, res, params);

      expect(r i== 1); // stop routing

      expect(flu_list_get(res->headers, "set-cookie") == NULL);
    }
  }
}

