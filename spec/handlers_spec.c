
//
// specifying shervin
//
// Thu Nov  6 15:37:01 JST 2014
//

#include "gajeta.h"
#include "shervin.h"
//#include "shv_protected.h" // direct access to fshv_request methods


context "handlers"
{
  before each
  {
    fshv_env *env = NULL;
    flu_list *conf = NULL;
  }
  after each
  {
    fshv_env_free(env);
    flu_list_free(conf);
  }

  describe "fshv_serve_files()"
  {
    context "env->bag '**'"
    {
      it "serves successfully"
      {
        env = fshv_env_prepare(
          "GET /x/y/a/b/hello.txt HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        flu_list_set(env->bag, "**", rdz_strdup("a/b/hello.txt"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 1);

        expect(flu_list_get(env->res->headers, "X-Accel-Redirect") === ""
          "../spec/www/a/b/hello.txt");
        expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
          "12");
        expect(flu_list_get(env->res->headers, "content-type") === ""
          "text/plain");

        expect(env->res->status_code i== 200);
      }

      it "goes 404 if the file is not found"
      {
        env = fshv_env_prepare(
          "GET /x/y/a/b/nada.txt HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        flu_list_set(env->bag, "**", rdz_strdup("a/b/nada.txt"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 0);
        expect(env->res->status_code i== 404);
      }

      it "goes 404 if the file is a dir"
      {
        env = fshv_env_prepare(
          "GET /x/y/a HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        flu_list_set(env->bag, "**", rdz_strdup("a"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 0);
        expect(env->res->status_code i== 404);
      }

      it "goes 403 when the request goes ../"
      {
        env = fshv_env_prepare(
          "GET /x/y/../www/a/b/hello.txt HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        flu_list_set(env->bag, "**", rdz_strdup("../www/a/b/hello.txt"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 0);
        expect(env->res->status_code i== 403);
      }

      it "accepts an alternative sendfile \"accel-header\" header via the conf"
      {
        conf = flu_d("accel-header", "X-Sendfile", NULL);

        env = fshv_env_prepare(
          "GET /x/y/a/b/hello.txt HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          conf);

        flu_list_set(env->bag, "**", rdz_strdup("a/b/hello.txt"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 1);

        expect(flu_list_get(env->res->headers, "X-Accel-Redirect") == NULL);

        expect(flu_list_get(env->res->headers, "X-Sendfile") === ""
          "../spec/www/a/b/hello.txt");
        expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
          "12");
        expect(flu_list_get(env->res->headers, "content-type") === ""
          "text/plain");

        expect(env->res->status_code i== 200);
      }

      it "serves a/b/index.html when asked for a/b"
      {
        env = fshv_env_prepare(
          "GET /web/a/b HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        flu_list_set(env->bag, "**", rdz_strdup("a/b"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 1);

        expect(flu_list_get(env->res->headers, "X-Accel-Redirect") === ""
          "../spec/www/a/b/index.html");
        expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
          "13");
        expect(flu_list_get(env->res->headers, "content-type") === ""
          "text/html");

        expect(env->res->status_code i== 200);
      }

      it "accepts alternative indexes"
      {
        conf = flu_d("index", "index.txt", NULL);

        env = fshv_env_prepare(
          "GET /web/a/ HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          conf);

        flu_list_set(env->bag, "**", rdz_strdup("a/"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 1);

        expect(flu_list_get(env->res->headers, "X-Accel-Redirect") === ""
          "../spec/www/a/index.txt");
        expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
          "21");
        expect(flu_list_get(env->res->headers, "content-type") === ""
          "text/plain");

        expect(env->res->status_code i== 200);
      }

      it "defaults to text/plain for unknown filetypes"
      {
        env = fshv_env_prepare(
          "GET /x/y/a/b/nada.nad HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        flu_list_set(env->bag, "**", rdz_strdup("a/b/nada.nad"));

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 1);

        expect(flu_list_get(env->res->headers, "X-Accel-Redirect") === ""
          "../spec/www/a/b/nada.nad");
        expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
          "10");
        expect(flu_list_get(env->res->headers, "content-type") === ""
          "text/plain");

        expect(env->res->status_code i== 200);
      }
    }

    context "no env->bag '**'"
    {
      it "serves successfully"
      {
        env = fshv_env_prepare(
          "GET /a/b/hello.txt HTTP/1.1\r\n"
          "Host: http://www.example.com\r\n"
          "\r\n",
          NULL);

        int r = fshv_serve_files(env, "../spec/www");

        expect(r i== 1);

        expect(flu_list_get(env->res->headers, "X-Accel-Redirect") === ""
          "../spec/www/a/b/hello.txt");
        expect(flu_list_get(env->res->headers, "fshv_content_length") === ""
          "12");
        expect(flu_list_get(env->res->headers, "content-type") === ""
          "text/plain");

        expect(env->res->status_code i== 200);
      }
    }
  }

  describe "fshv_mirror()"
  {
    it "mirrors the incoming request"
    {
      env = fshv_env_prepare(
        "GET /x/y/mirror HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "X-Whatever: hello whatever\r\n"
        "\r\n",
        NULL);

      flu_list_set(env->bag, "**", rdz_strdup("mirror"));

      int r = fshv_mirror(env, 0);

      expect(r i== 1);

      expect(fshv_response_body_to_s(env->res) ===f ""
        "GET /x/y/mirror HTTP/1.1\r\n"
        "x-whatever: hello whatever\r\n"
        "host: http://www.example.com\r\n"
        "method: GET\r\n"
        "path: /x/y/mirror\r\n"
        "uri: (uri s\"http\" h\"www.example.com\" p80 p\"/x/y/mirror\")\r\n"
        "\r\n");
    }

    it "logs the incoming request as well if do_log == 1"
    {
      fgaj_conf_get()->logger = fgaj_grey_logger;
      fgaj_conf_get()->level = 5;
      fgaj_conf_get()->out = stderr;
      fgaj_conf_get()->params = "5p";

      env = fshv_env_prepare(
        "GET /x/y/mirror HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "X-Whatever: hello whatever\r\n"
        "\r\n",
        NULL);

      flu_list_set(env->bag, "**", rdz_strdup("mirror"));

      int r = fshv_mirror(env, 1);

      expect(r i== 1);
    }

    it "mirrors POST requests"
    {
      fgaj_conf_get()->logger = fgaj_grey_logger;
      fgaj_conf_get()->level = 5;
      fgaj_conf_get()->out = stderr;
      fgaj_conf_get()->params = "5p";

      env = fshv_env_prepare(
        "POST /x/y/mirror HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "X-Whatever: hello server\r\n"
        "\r\n"
        "hello /mirror on the server",
        NULL);

      expect(env->req != NULL);

      flu_list_set(env->bag, "**", rdz_strdup("mirror"));

      int r = fshv_mirror(env, 1);

      expect(r i== 1);

      expect(fshv_response_body_to_s(env->res) ===f ""
        "POST /x/y/mirror HTTP/1.1\r\n"
        "x-whatever: hello server\r\n"
        "host: http://www.example.com\r\n"
        "method: POST\r\n"
        "path: /x/y/mirror\r\n"
        "uri: (uri s\"http\" h\"www.example.com\" p80 p\"/x/y/mirror\")\r\n"
        "\r\n"
        "hello /mirror on the server");
    }
  }
}

