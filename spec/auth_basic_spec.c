
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

      int r = fshv_basic_auth(env, "therealm", specauth);

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

      int r = fshv_basic_auth(env, "therealm", specauth);

      expect(r i== 1);
      expect(flu_list_get(env->bag, "_basic_user") === "toto");

      expect(env->res->status_code i== -1);
    }

    it "authentifies (miss)"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic NADANADANADA\r\n"
        "\r\n",
        NULL);

      int r = fshv_basic_auth(env, "therealm", specauth);

      expect(r i== 0);
      expect(flu_list_get(env->bag, "_basic_user") == NULL);

      expect(env->res->status_code i== 401);
    }

    it "answers with www-authenticate in case of auth miss"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic BLUEORIGNAL\r\n"
        "\r\n",
        NULL);

      int r = fshv_basic_auth(env, "wonderland", specauth);

      expect(r i== 0);

      expect(flu_list_get(env->res->headers, "WWW-Authenticate") === ""
        "Basic realm=\"wonderland\"");
    }

    it "doesn't answer with www-authenticate if the realm is NULL"
    {
      env = fshv_env_prepare(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Authorization: Basic BLACKORIGNAL\r\n"
        "\r\n",
        NULL);

      int r = fshv_basic_auth(env, NULL, specauth);

      expect(r i== 0);

      expect(flu_list_get(env->res->headers, "WWW-Authenticate") == NULL);
    }
  }
}

