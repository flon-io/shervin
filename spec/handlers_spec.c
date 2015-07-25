
//
// specifying shervin
//
// Thu Nov  6 15:37:01 JST 2014
//

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
      it "works"
    }
  }

//  describe "fshv_dir_handler()"
//  {
//    before each
//    {
//      struct ev_io *eio = calloc(1, sizeof(ev_io));
//      fshv_con *con = calloc(1, sizeof(fshv_con));
//      eio->data = con;
//    }
//    after each
//    {
//      for (size_t i = 0; con->routes[i] != NULL; ++i)
//      {
//        flu_list_free(con->routes[i]->params);
//        free(con->routes[i]);
//      }
//      //free(con->routes); // not malloc'ed
//      fshv_request_free(con->req);
//      fshv_response_free(con->res);
//      free(con);
//      free(eio);
//    }
//
//    it "serves files with a wildcard path"
//    {
//      con->routes = (fshv_route *[]){
//        fshv_rp("/x/y/**", fshv_dir_handler, "root", "../spec/www", NULL),
//        NULL // do not forget it
//      };
//
//      con->req = fshv_parse_request_head(
//        "GET /x/y/a/b/index.html HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      fshv_handle(NULL, eio);
//
//      expect(flu_list_get(con->res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/index.html");
//      expect(flu_list_get(con->res->headers, "fshv_content_length") === ""
//        "13");
//      expect(flu_list_get(con->res->headers, "content-type") === ""
//        "text/html");
//    }
//
//    it "serves files without a wildcard path"
//    {
//      con->routes = (fshv_route *[]){
//        fshv_rp("/a/b/index.html", fshv_dir_handler, "root", "../spec/www", NULL),
//        NULL // do not forget it
//      };
//
//      con->req = fshv_parse_request_head(
//        "GET /a/b/index.html HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      fshv_handle(NULL, eio);
//
//      expect(flu_list_get(con->res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/index.html");
//      expect(flu_list_get(con->res->headers, "fshv_content_length") === ""
//        "13");
//      expect(flu_list_get(con->res->headers, "content-type") === ""
//        "text/html");
//    }
//
//    it "serves files without a wildcard path (GET)"
//    {
//      con->routes = (fshv_route *[]){
//        fshv_rp("GET /a/b/index.html",
//          fshv_dir_handler,
//            "root", "../spec/www", NULL),
//        NULL // do not forget it
//      };
//
//      con->req = fshv_parse_request_head(
//        "GET /a/b/index.html HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      fshv_handle(NULL, eio);
//
//      expect(flu_list_get(con->res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/index.html");
//      expect(flu_list_get(con->res->headers, "fshv_content_length") === ""
//        "13");
//      expect(flu_list_get(con->res->headers, "content-type") === ""
//        "text/html");
//    }
//
//    it "serves files without a wilcard path but with a start param"
//    {
//      con->routes = (fshv_route *[]){
//        fshv_rp("/x/y/a/b/index.html",
//          fshv_dir_handler,
//            "root", "../spec/www", "s", "/x/y", NULL),
//        NULL // do not forget it
//      };
//
//      con->req = fshv_parse_request_head(
//        "GET /x/y/a/b/index.html HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      fshv_handle(NULL, eio);
//
//      expect(flu_list_get(con->res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/index.html");
//      expect(flu_list_get(con->res->headers, "fshv_content_length") === ""
//        "13");
//      expect(flu_list_get(con->res->headers, "content-type") === ""
//        "text/html");
//    }
//  }

  describe "fshv_mirror()"
  {
    it "mirrors the incoming request"
  }
}

