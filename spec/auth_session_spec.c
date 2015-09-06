
//
// specifying shervin
//
// Fri Dec 12 22:32:08 JST 2014
//

#include "flutim.h"
//#include "shervin.h"
//#include "shv_protected.h" // direct access to fshv_request methods
#include "shv_auth_session_memstore.h"


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
      fshv_session_memstore_push(NULL, NULL, NULL, NULL, -1);
        // reset store
    }

    it "starts a session"
    {
      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary

      fshv_session *s = fshv_session_memstore_push(
        NULL, "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      expect(s->used i== 0);
      expect(fshv_session_memstore()->size == 1);

      fshv_session *s1 = fshv_session_memstore_push(
        NULL, "deadbeef", NULL, NULL, flu_gets('u')); // query

      expect(s1 != NULL);
      expect(s1->sid === "deadbeef");
    }

    it "refreshes a session"
    {
      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary

      fshv_session_memstore_push(
        NULL, "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        NULL, "c0bebeef", "toto", "toto:1234", flu_gets('u') + e); // renew

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
        NULL, "boeufcharolais", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 0);
    }

    it "queries and misses (session used)"
    {
      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary

      fshv_session *s = fshv_session_memstore_push(
        NULL, "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      s->used = 1; // memstore...

      s = fshv_session_memstore_push(
        NULL, "deadbeef", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 1);
    }

    it "queries and misses"
    {
      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary

      fshv_session_memstore_push(
        NULL, "deadbeef", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        NULL, "boeufcharolais", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 1);
    }

    it "queries and hits"
    {
      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary

      fshv_session_memstore_push(
        NULL, "delaplatabeef", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        NULL, "delaplatabeef", NULL, NULL, flu_gets('u')); // query

      expect(s != NULL);
      expect(s->id === "toto:1234");

      expect(fshv_session_memstore()->size == 1);
    }

    it "queries, misses and expires (1 record)"
    {
      fshv_session_memstore_push(
        NULL, "beef0", "alice", "alice:1234", flu_gets('u') - 1); // start

      expect(fshv_session_memstore()->size == 1);

      fshv_session *s = fshv_session_memstore_push(
        NULL, "beef1", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 0);
        // the query purged the store
    }

    it "queries, misses and expires (1+ records)"
    {
      fshv_session_memstore_push(
        NULL, "beef0", "alice", "alice:1234", flu_gets('u') - 3); // start
      fshv_session_memstore_push(
        NULL, "beef1", "alice", "alice:1234", flu_gets('u') - 2); // start

      expect(fshv_session_memstore()->size == 2);

      expect(
        ((fshv_session *)fshv_session_memstore()->last->item)->used i== 1);
      expect(
        ((fshv_session *)fshv_session_memstore()->first->item)->used i== 0);

      fshv_session *s = fshv_session_memstore_push(
        NULL, "beef1", NULL, NULL, flu_gets('u')); // query

      expect(s == NULL);

      expect(fshv_session_memstore()->size == 0);
        // the query purged the store
    }

    it "stops a session"
    {
      long long e = (long long)10 * 60 * 60 * 1000 * 1000;
        // expiry time, arbitrary

      fshv_session_memstore_push(
        NULL, "waterbuffalo", "toto", "toto:1234", flu_gets('u') + e); // start

      fshv_session *s = fshv_session_memstore_push(
        NULL, "waterbuffalo", NULL, NULL, -1); // stop

      expect(s == NULL);
      expect(fshv_session_memstore()->size == 1);

      s = fshv_session_memstore()->first->item;

      expect(s->used i== 1);
    }
  }

  describe "fshv_session_auth()"
  {
    before each
    {
      fshv_env *env = NULL;

      fshv_session_memstore_push(NULL, NULL, NULL, NULL, -1);
        // reset store

      fshv_session_memstore_push(
        NULL,
        "abcdef123",
        "toto",
        "toto:1234:4567",
        flu_gets('u') + (long long)10 * 60 * 60 * 1000 * 1000);
          // start session

        // expiry time, arbitrary
    }
    after each
    {
      fshv_env_free(env);
    }

    it "misses (no cookie)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n",
        NULL);
      //req->startus = flu_gets('u');

      int r = fshv_session_auth(
        env, fshv_session_memstore_push, "shervin.test");

      expect(r i== 0);

      expect(env->res->status_code i== 401);
      expect(flu_list_get(env->res->headers, "set-cookie") == NULL);
    }

    it "misses (cookies but none for us)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; o.ther=1234abc\r\n"
        "\r\n",
        NULL);
      //req->startus = flu_gets('u');

      int r = fshv_session_auth(
        env, fshv_session_memstore_push, "shervin.test");

      expect(r i== 0);

      expect(env->res->status_code i== 401);
    }

    it "authentifies (hit)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123; o.ther=1234abc\r\n"
        "\r\n",
        NULL);
      //env->req->startus = flu_gets('u');

      int r = fshv_session_auth(
        env, fshv_session_memstore_push, "shervin.test");

      expect(r i== 1);

      expect(env->res->status_code i== -1);
      expect(flu_list_get(env->bag, "_session_user") === "toto");

      expect(fshv_session_memstore()->size i== 2);

      fshv_session *ses = fshv_session_memstore()->first->item;
      expect(ses->id === "toto:1234:4567");

      char *s = flu_list_get(env->res->headers, "set-cookie");
      expect(s ^== "shervin.test=");
      expect(s >== ses->sid);
      expect(s >== ";Expires=");
      expect(s >== ";HttpOnly");
      expect(strstr(s, ";Secure") == NULL);
    }

    it "authentifies (hit, cookie last)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
        "\r\n",
        NULL);
      //req->startus = flu_gets('u');

      int r = fshv_session_auth(
        env, fshv_session_memstore_push, "shervin.test");

      expect(r i== 1);

      expect(env->res->status_code i== -1);
      expect(flu_list_get(env->bag, "_session_user") === "toto");

      expect(fshv_session_memstore()->size i== 2);

      fshv_session *ses = fshv_session_memstore()->first->item;
      expect(ses->id === "toto:1234:4567");

      char *s = flu_list_get(env->res->headers, "set-cookie");
      expect(s ^== "shervin.test=");
      expect(s >== ses->sid);
      expect(s >== ";Expires=");
      expect(s >== ";HttpOnly");
      expect(strstr(s, ";Secure") == NULL);
    }

    it "authentifies (hit, https)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Cookie: geo.nada=timbuk; shervin.test=abcdef123\r\n"
        "X-Forwarded-Proto: https\r\n"
        "\r\n",
        NULL);
      //req->startus = flu_gets('u');

      int r = fshv_session_auth(
        env, fshv_session_memstore_push, "shervin.test");

      expect(r i== 1);

      expect(env->res->status_code i== -1);
      expect(flu_list_get(env->bag, "_session_user") === "toto");

      expect(fshv_session_memstore()->size i== 2);

      fshv_session *ses = fshv_session_memstore()->first->item;
      expect(ses->id === "toto:1234:4567");

      char *s = flu_list_get(env->res->headers, "set-cookie");
      expect(s $== ";Secure");
    }
  }
}

