
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
  }
  after each
  {
    fshv_env_free(env);
  }

  describe "fshv_serve_files()"
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
    }
  }

//    it "returns 0 if the file is not found"
//    {
//      req = fshv_parse_request_head(
//        "GET /x/y/a/b/nada.txt HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/nada.txt"));
//
//      params = flu_d("root", "../spec/www", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 0);
//    }
//
//    it "returns 0 when the file is a dir"
//    {
//      req = fshv_parse_request_head(
//        "GET /x/y/a HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a"));
//
//      params = flu_d("root", "../spec/www", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 0);
//    }
//
//    it "returns 0 when the request goes ../"
//    {
//      req = fshv_parse_request_head(
//        "GET /x/y/../www/a/b/hello.txt HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("../www/a/b/hello.txt"));
//
//      params = flu_d("root", "../spec/www", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 0);
//    }
//
//    it "accepts an alternative sendfile \"h\" header via the params"
//    {
//      req = fshv_parse_request_head(
//        "GET /x/y/a/b/hello.txt HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/hello.txt"));
//
//      params = flu_d("r", "../spec/www", "h", "X-Sendfile", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 1);
//
//      expect(flu_list_get(res->headers, "X-Accel-Redirect") == NULL);
//
//      expect(flu_list_get(res->headers, "X-Sendfile") === ""
//        "../spec/www/a/b/hello.txt");
//      expect(flu_list_get(res->headers, "fshv_content_length") === ""
//        "12");
//      expect(flu_list_get(res->headers, "content-type") === ""
//        "text/plain");
//    }
//
//    it "serves a/b/index.html when asked for a/b"
//    {
//      req = fshv_parse_request_head(
//        "GET /web/a/b HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a/b"));
//
//      params = flu_d("root", "../spec/www", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 1);
//
//      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/index.html");
//      expect(flu_list_get(res->headers, "fshv_content_length") === ""
//        "13");
//      expect(flu_list_get(res->headers, "content-type") === ""
//        "text/html");
//    }
//
//    it "accepts alternative indexes"
//    {
//      req = fshv_parse_request_head(
//        "GET /web/a/ HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a/"));
//
//      params = flu_d("root", "../spec/www", "index", "index.txt", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 1);
//
//      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/index.txt");
//      expect(flu_list_get(res->headers, "fshv_content_length") === ""
//        "21");
//      expect(flu_list_get(res->headers, "content-type") === ""
//        "text/plain");
//    }
//
//    it "defaults to text/plain for unknown filetypes"
//    {
//      req = fshv_parse_request_head(
//        "GET /x/y/a/b/nada.nad HTTP/1.1\r\n"
//        "Host: http://www.example.com\r\n"
//        "\r\n");
//
//      flu_list_set(req->routing_d, "**", rdz_strdup("a/b/nada.nad"));
//
//      params = flu_d("root", "../spec/www", NULL);
//      int r = fshv_dir_handler(req, res, 0, params);
//
//      expect(r i== 1);
//
//      expect(flu_list_get(res->headers, "X-Accel-Redirect") === ""
//        "../spec/www/a/b/nada.nad");
//      expect(flu_list_get(res->headers, "fshv_content_length") === ""
//        "10");
//      expect(flu_list_get(res->headers, "content-type") === ""
//        "text/plain");
//    }
//  }
//
//  describe "fshv_serve_file()"
//  {
//    before each
//    {
//      fshv_request *req = NULL;
//      flu_dict *params = NULL;
//      fshv_response *res = fshv_response_malloc(200);
//
//      params = flu_list_malloc();
//    }
//    after each
//    {
//      fshv_request_free(req);
//      flu_list_free(params);
//      fshv_response_free(res);
//    }
//
//    it "returns -1 when it doesn't find a file"
//    {
//      ssize_t r = fshv_serve_file(res, params, "../spec/www/a/b/nada.html");
//
//      expect(r li== -1);
//    }
//
//    it "returns 0 when the file is a directory"
//    {
//      ssize_t r = fshv_serve_file(res, params, "../spec/www/a/b");
//      expect(r li== 0);
//
//      r = fshv_serve_file(res, params, "../spec/www/a/b/");
//      expect(r li== 0);
//    }
//
//    it "returns the size of the file and sets headers when it's a regular file"
//    {
//      ssize_t r = fshv_serve_file(res, params, "../spec/www/a/b/index.html");
//
//      expect(r li== 13);
//
//      expect(flu_list_get(res->headers, "fshv_content_length") === ""
//        "13");
//      expect(flu_list_get(res->headers, "fshv_file") === ""
//        "../spec/www/a/b/index.html");
//      expect(flu_list_get(res->headers, "content-type") === ""
//        "text/html");
//    }
//  }
//
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
}

