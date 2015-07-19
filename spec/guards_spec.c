
//
// specifying shervin
//
// Sat Apr 25 06:41:17 JST 2015
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "guards"
{
  before each
  {
    fshv_env *env = NULL;
  }
  after each
  {
    fshv_env_free(env);
  }

  describe "fshv_match()"
  {
    it "returns 0 when it doesn't match"
    {
      env = fshv_env_prepare(
        "GET /x?a=b#f HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = fshv_match(env, "GET /nowhere");

      ensure(r == 0);
    }

    it "leaves the bag intact when it doesn't match"
    {
      env = fshv_env_prepare(
        "GET /book/moominvalley/chapter HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = fshv_match(env, "GET /book/:title/chapter/:id");

      ensure(r == 0);
      ensure(env->bag->size zu== 0);
    }

    it "matches an absolute route"
    {
      env = fshv_env_prepare(
        "GET /x?a=b#f HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = fshv_match(env, "GET /x");

      ensure(r == 1);
      ensure(env->bag->size zu== 0);
    }

    it "puts the ** in the bag"
    {
      env = fshv_env_prepare(
        "GET /x/y/z HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = fshv_match(env, "GET /x/**");

      ensure(r == 1);
      ensure(env->bag->size zu== 1);
      ensure(flu_list_get(env->bag, "**") === "y/z");
    }

    it "puts :x in the bag"
    {
      env = fshv_env_prepare(
        "GET /books/heart-of-darkness/3 HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "\r\n");

      int r = fshv_match(env, "GET /books/:name/:page");

      ensure(r == 1);
      ensure(env->bag->size zu== 2);
      ensure(flu_list_get(env->bag, "name") === "heart-of-darkness");
      ensure(flu_list_get(env->bag, "page") === "3");
    }
  }

  describe "fshv_smatch()"
  {
    it "sub-matches"
  }
}

