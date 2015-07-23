
//
// specifying shervin
//
// Fri Jul 24 06:37:20 JST 2015
//

#include "shervin.h"
//#include "shv_protected.h" // direct access to fshv_request methods


context "shv_protected.c"
{
  describe "fshv_serve_file()"
  {
    before each
    {
      fshv_env *env = fshv_env_prepare(
        "GET /whatever HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n",
        NULL);
    }
    after each
    {
      fshv_env_free(env);
    }

    it "returns -1 when it doesn't find a file"
    {
      ssize_t r = fshv_serve_file(env, "../spec/www/a/b/nada.html");

      expect(r li== -1);
    }

    it "returns 0 when the file is a directory"
    {
      ssize_t r = fshv_serve_file(env, "../spec/www/a/b");
      expect(r li== 0);

      r = fshv_serve_file(env, "../spec/www/a/b/");
      expect(r li== 0);
    }

    it "returns the size of the file and sets headers when it's a regular file"
    {
      ssize_t r = fshv_serve_file(env, "../spec/www/a/b/index.html");

      expect(r li== 13);

      expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
        "13");
      expect(flu_list_get(env->res->headers, "fshv_file") === ""
        "../spec/www/a/b/index.html");
      expect(flu_list_get(env->res->headers, "content-type") === ""
        "text/html");
    }
  }
}

