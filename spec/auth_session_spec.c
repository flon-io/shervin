
//
// specifying shervin
//
// Fri Dec 12 22:32:08 JST 2014
//

//#include "flutim.h"
//#include "shervin.h"
//#include "shv_protected.h" // direct access to fshv_request methods
//#include "shv_auth_session_memstore.h"


context "session auth:"
{
  // * pushing with all the parameters set and expiry time:
  //   start or refreshes a session
  //   returns the new session in case of success, NULL else
  // * pushing with only the sid set and now:
  //   queries and expires,
  //   returns a session in case of success, NULL else
  // * pushing with only the sid set and -1:
  //   stops the session and returns NULL
  // * pushing with all NULL and -1:
  //   resets the store and returns NULL

  describe "fshv_session_memstore_push()"
  {
    before each
    {
      fshv_session_memstore_push(NULL, NULL, NULL, -1);
        // reset store

      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary
    }

    it "starts a session"
    {
      fshv_session *s = fshv_session_memstore_push(
        "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      expect(s->used i== 0);
      expect(fshv_session_memstore()->size == 1);

      fshv_session *s1 = fshv_session_memstore_push(
        "deadbeef", NULL, NULL, flu_gets('u')); // query

      expect(s1 != NULL);
      expect(s1->sid === "deadbeef");
    }

    it "refreshes a session"
    {
      fshv_session_memstore_push(
        "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        "c0bebeef", "toto", "toto:1234", flu_gets('u') + e); // renew

      expect(s->used i== 0);
      expect(fshv_session_memstore()->size == 2);

      fshv_session *s1 = fshv_session_memstore()->first->item;
      fshv_session *s0 = fshv_session_memstore()->first->next->item;

      expect(s1->used i== 0);
      expect(s0->used i== 1);
    }

    it "queries and misses (store empty)"
    {
      fshv_session *s = fshv_session_memstore_push(
        "boeufcharolais", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 0);
    }

    it "queries and misses (session used)"
    {
      fshv_session *s = fshv_session_memstore_push(
        "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      s->used = 1; // memstore...

      s = fshv_session_memstore_push(
        "deadbeef", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 1);
    }

    it "queries and misses"
    {
      fshv_session_memstore_push(
        "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        "boeufcharolais", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 1);
    }

    it "queries and hits"
    {
      fshv_session_memstore_push(
        "delaplatabeef", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        "delaplatabeef", NULL, NULL, flu_gets('u')); // query

      expect(s != NULL);
      expect(s->id === "toto:1234");

      expect(fshv_session_memstore()->size == 1);
    }

    it "queries, misses and expires (1 record)"
    {
      fshv_session_memstore_push(
        "beef0", "alice", "alice:1234", flu_gets('u') - 1); // start

      expect(fshv_session_memstore()->size == 1);

      fshv_session *s = fshv_session_memstore_push(
        "beef1", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 0);
        // the query purged the store
    }

    it "queries, misses and expires (1+ records)"
    {
      fshv_session_memstore_push(
        "beef0", "alice", "alice:1234", flu_gets('u') - 3); // start
      fshv_session_memstore_push(
        "beef1", "alice", "alice:1234", flu_gets('u') - 2); // start

      expect(fshv_session_memstore()->size == 2);

      expect(
        ((fshv_session *)fshv_session_memstore()->last->item)->used i== 1);
      expect(
        ((fshv_session *)fshv_session_memstore()->first->item)->used i== 0);

      fshv_session *s = fshv_session_memstore_push(
        "beef1", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 0);
        // the query purged the store
    }

    it "stops a session"
    {
      fshv_session_memstore_push(
        "waterbuffalo", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        "waterbuffalo", NULL, NULL, -1); // stop

      expect(s == NULL);
      expect(fshv_session_memstore()->size == 1);

      s = fshv_session_memstore()->first->item;

      expect(s->used i== 1);
    }
  }

//  describe "fshv_session_auth_filter()"
//  {
//    before each
//    {
//      fshv_request *req = NULL;
//      flu_dict *params = NULL;
//      fshv_response *res = fshv_response_malloc(200);
//
//      params = flu_d(
//        "a", sa_specauth,
//        "n", "shervin.test",
//        NULL);
//
//      fshv_session_memstore_push(NULL, NULL, NULL, -1);
//        // reset store
//
//      fshv_session_memstore_push(
//        "abcdef123",
//        "toto",
//        "toto:1234:4567",
//        flu_gets('u') + (long long)10 * 60 * 60 * 1000 * 1000);
//          // start session
//
//        // expiry time, arbitrary
//    }
//    after each
//    {
//      fshv_request_free(req);
//      flu_list_free(params);
//      fshv_response_free(res);
//    }
//
//    int sa_specauth(const char *user, const char *pass, flu_dict *params)
//    {
//      return (strcmp(user, pass) == 0);
//    }
//
//    it "authentifies (hit)"
//    {
//      req = fshv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "Cookie: geo.nada=timbuk; shervin.test=abcdef123; o.ther=1234abc\r\n"
//        "\r\n");
//      req->startus = flu_gets('u');
//
//      int r = fshv_session_auth_filter(req, res, 0, params);
//
//      expect(r i== 0); // continue routing
//      expect(flu_list_get(req->routing_d, "_session_user") === "toto");
//
//      expect(fshv_session_memstore()->size i== 2);
//
//      fshv_session *ses = fshv_session_memstore()->first->item;
//      expect(ses->id === "toto:1234:4567");
//
//      char *s = flu_list_get(res->headers, "set-cookie");
//      expect(s ^== "shervin.test=");
//      expect(s >== ses->sid);
//      expect(s >== ";Expires=");
//      expect(s >== ";HttpOnly");
//      expect(strstr(s, ";Secure") == NULL);
//    }
//
//    it "authentifies (hit, cookie last)"
//    {
//      req = fshv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
//        "\r\n");
//      req->startus = flu_gets('u');
//
//      int r = fshv_session_auth_filter(req, res, 0, params);
//
//      expect(r i== 0); // continue routing
//      expect(flu_list_get(req->routing_d, "_session_user") === "toto");
//
//      expect(fshv_session_memstore()->size i== 2);
//
//      fshv_session *ses = fshv_session_memstore()->first->item;
//      expect(ses->id === "toto:1234:4567");
//
//      char *s = flu_list_get(res->headers, "set-cookie");
//      expect(s ^== "shervin.test=");
//      expect(s >== ses->sid);
//      expect(s >== ";Expires=");
//      expect(s >== ";HttpOnly");
//      expect(strstr(s, ";Secure") == NULL);
//    }
//
//    it "authentifies (hit, https)"
//    {
//      req = fshv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
//        "X-Forwarded-Proto: https\r\n"
//        "\r\n");
//      req->startus = flu_gets('u');
//
//      int r = fshv_session_auth_filter(req, res, 0, params);
//
//      expect(r i== 0); // continue routing
//      expect(flu_list_get(req->routing_d, "_session_user") === "toto");
//
//      expect(fshv_session_memstore()->size i== 2);
//
//      fshv_session *ses = fshv_session_memstore()->first->item;
//      expect(ses->id === "toto:1234:4567");
//
//      char *s = flu_list_get(res->headers, "set-cookie");
//      expect(s $== ";Secure");
//    }
//
//    it "authentifies (miss, no cookie)"
//    {
//      req = fshv_parse_request_head(
//        "GET /x HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//      req->startus = flu_gets('u');
//
//      int r = fshv_session_auth_filter(req, res, 0, params);
//
//      expect(r i== 0); // handled -> 0
//
//      expect(flu_list_get(res->headers, "set-cookie") == NULL);
//    }
//  }
}

