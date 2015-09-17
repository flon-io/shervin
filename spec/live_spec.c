
//
// specifying shervin
//
// Sun Sep 21 15:35:29 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods

#include "servman.h"
#include "flutil.h"
#include "bocla.h"


context "live"
{
  before all
  {
    server_start();
  }
  after all
  {
    server_stop();
    //system("lsof -i :4001");
  }

  describe "shervin"
  {
    before each
    {
      fcla_response *res = NULL;
    }
    after each
    {
      fcla_response_free(res);
    }

    it "serves /hello/toto"
    {
      res = fcla_get("http://127.0.0.1:4001/hello/toto");

      //printf("res: %i\n", res->status_code);
      //flu_putf(flu_list_to_sm(res->headers));

      expect(res->status_code i== 200);
      expect(res->body === "hello toto\n");

      char *s = flu_list_get(res->headers, "x-flon-shervin");
      expect(s != NULL);
      expect(strstr(s, " c-") == NULL);
      expect(strstr(s, ";r-") == NULL);
      expect(s >== "ms;rq0");
    }

    it "serves back the body it received in /mirror"
    {
      res = fcla_post("http://127.0.0.1:4001/mirror", NULL, "nada");

      //printf("res->body >%s<\n", res->body);
      char *body = strstr(res->body, "\r\n\r\n") + 4;

      expect(res->status_code i== 200);
      expect(body === "nada");
    }

    it "serves files"
    {
      res = fcla_get("http://127.0.0.1:4001/files/a/b/hello.txt");

      //printf("res: %i\n", res->status_code);
      //flu_putf(flu_list_to_sm(res->headers));

      expect(res->status_code i== 200);
      expect(res->body === "hello world\n");

      expect(flu_list_get(res->headers, "x-accel-redirect") === ""
        "../spec/www/a/b/hello.txt");
    }

    it "serves medium-sized files"
    {
      res = fcla_get("http://127.0.0.1:4001/files/a/b/handlebars.js");

      //printf("res: %i\n", res->status_code);
      //flu_putf(flu_list_to_sm(res->headers));

      expect(res->status_code i== 200);
      expect(res->body ^== "/*!\n");
      expect(strlen(res->body) zu== 53520);

      expect(flu_list_get(res->headers, "content-length") === ""
        "53520");
      expect(flu_list_get(res->headers, "x-accel-redirect") === ""
        "../spec/www/a/b/handlebars.js");
    }

    it "serves empty files"
    {
      res = fcla_get("http://127.0.0.1:4001/files/a/empty.txt");

      //printf("res: %i\n", res->status_code);
      //flu_putf(flu_list_to_sm(res->headers));

      expect(res->status_code i== 200);
      expect(res->body === "");

      expect(flu_list_get(res->headers, "x-accel-redirect") === ""
        "../spec/www/a/empty.txt");
    }

    it "doesn't actually serve the file if x-real-ip is set"
    {
      res = fcla_get_d(
        "http://127.0.0.1:4001/files/a/b/hello.txt",
        "X-Real-IP", "127.0.0.1", NULL);

      //printf("res: %i\n", res->status_code);
      //flu_putf(flu_list_to_sm(res->headers));

      expect(res->status_code i== 200);
      expect(res->body === "");

      expect(flu_list_get(res->headers, "x-accel-redirect") === ""
        "../spec/www/a/b/hello.txt");
    }

    it "serves the reason text for 4xx and 5xx responses"
    {
      res = fcla_get("http://127.0.0.1:4001/nemo");

      expect(res->status_code i== 404);
      expect(res->body === "not found");
    }

    it "doesn't override the Location set by a handler"
    {
      res = fcla_get("http://127.0.0.1:4001/redir");

      expect(res->status_code i== 303);
      expect(res->body === "");
      expect(flu_list_get(res->headers, "location") === "/somewhere/else");
    }

    it "doesn't crash on invalid requests"
    {
      char *out = flu_plines("ruby ../spec/live/invalid.rb");

      expect(out ===f ""
        "HTTP/1.1 400 Bad Request\r\n"
        "content-length: 8\r\n"
        "content-type: text/plain; charset=utf-8\r\n"
        "\r\n"
        "\r\nbad request")
    }
  }

  describe "shervin and basic auth"
  {
    it "flips burgers"
  }

  describe "shervin and session auth"
  {
    before each
    {
      fcla_response *res = NULL;
    }
    after each
    {
      fcla_response_free(res);
    }

    it "doesn't start a session upon failed login"
    {
      res = fcla_post("http://127.0.0.1:4001/login", NULL, "u=toto;p=nada");

      expect(res->status_code i== 401);

      char *s = flu_list_get(res->headers, "set-cookie");
      expect(s == NULL);
    }

    it "starts a session upon successful login"
    {
      res = fcla_post("http://127.0.0.1:4001/login", NULL, "u=toto;p=toto");

      expect(res->status_code i== 200);

      //flu_putf(flu_list_to_sm(res->headers));
      char *s = flu_list_get(res->headers, "set-cookie");
      expect(s != NULL);
      expect(s ^== "flon.io.shervin=");
      expect(s >== ";Expires=");
      expect(s >== ";HttpOnly");
      expect(strstr(s, ";Secure") == NULL);
    }

    it "supports a login/play/logout webflow"
    {
      // unauthorized

      res = fcla_get(
        "http://127.0.0.1:4001/secret");

      //flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 401);

      // login

      fcla_response_free(res);
      res = fcla_post(
        "http://127.0.0.1:4001/login", NULL, "u=toto;p=toto");

      //flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 200);

      char *cookie0 = rdz_strdup(flu_list_get(res->headers, "set-cookie"));
      *strchr(cookie0, ';') = 0;

      // authorized

      fcla_response_free(res);
      res = fcla_get_d(
        "http://127.0.0.1:4001/secret", "cookie", cookie0, NULL);

      //flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 200);

      char *cookie1 = rdz_strdup(flu_list_get(res->headers, "set-cookie"));
      *strchr(cookie1, ';') = 0;

      expect(cookie1 !== cookie0);

      expect(res->body === "there are no secrets.");

      // logout

      fcla_response_free(res);
      res = fcla_post_d(
        "http://127.0.0.1:4001/login", "", "cookie", cookie1, NULL);

      //flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 200);

      // unauthorized

      fcla_response_free(res);
      res = fcla_get_d(
        "http://127.0.0.1:4001/secret", "cookie", cookie1, NULL);

      //flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 401);

      // 404

      fcla_response_free(res);
      res = fcla_get_d(
        "http://127.0.0.1:4001/nada", "cookie", cookie1, NULL);

      //flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 404);

      free(cookie0); free(cookie1);
    }
  }
}

