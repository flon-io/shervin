
//
// specifying shervin
//
// Wed Sep 16 07:09:42 JST 2015
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "fshv_env"
{
  before each
  {
    fshv_env *env = NULL;
  }
  after each
  {
    fshv_env_free(env);
  }

  describe "fshv_env_malloc()"
  {
    it "returns a NULL req on an invalid request"
    {
      env = fshv_env_malloc_f(
        "GET /documents/logbook SMTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n",
        NULL);

      expect(env->req == NULL);
    }

    it "succeeds"
    {
      env = fshv_env_malloc_f(
        "GET /documents/logbook HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "X-more: year2015\r\n"
        "\r\n",
        NULL);

      expect(env->req->u === "/documents/logbook");
      expect(env->res->status_code i== -1);
      expect(env->conf == NULL);
    }
  }

  describe "fshv_env_malloc_f()"
  {
    it "returns a NULL req on an invalid request"
    {
      env = fshv_env_malloc_f(
        "GET /documents/%d SMTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n", NULL,
        1234);

      expect(env->req == NULL);
    }

    it "parses the request and returns a fshv_env 'instance'"
    {
      env = fshv_env_malloc_f(
        "GET /documents/%s HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n", NULL,
        "dictionary");

      expect(env->req->u === "/documents/dictionary");
    }
  }

  describe "fshv_env_malloc_x()"
  {
    it "returns a NULL req on an invalid request"
    {
      env = fshv_env_malloc_x(
        "GET /documents/%d SMTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n",
        1234,
        NULL);

      expect(env->req == NULL);
    }

    it "parses the request and returns a fshv_env 'instance'"
    {
      env = fshv_env_malloc_x(
        "GET /documents/%s HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n",
        "lexicon",
        NULL);

      expect(env->req->u === "/documents/lexicon");
      expect(env->conf == NULL);
    }

    it "parses the request and returns a fshv_env 'instance'"
    {
      flu_dict *conf = flu_d("k0", "v0", "k1", "v1", NULL);

      env = fshv_env_malloc_x(
        "GET /documents/%s HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n",
        "lexicon",
        conf);

      expect(env->req->u === "/documents/lexicon");
      expect(env->conf != NULL);
      expect(flu_list_get(env->conf, "k1") === "v1");

      flu_list_free(conf);
    }
  }
}

