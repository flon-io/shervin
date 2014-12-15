
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

      expect(res->status_code i== 200);
      expect(res->body === "hello toto\n");
    }

    it "serves back the body it received in /mirror"
    {
      res = fcla_post("http://127.0.0.1:4001/mirror", NULL, "nada");

      char *body = strstr(res->body, "\r\n\r\n") + 4;

      expect(res->status_code i== 200);
      expect(body === "nada");
    }

    it "serves files"
    {
      res = fcla_get("http://127.0.0.1:4001/files/a/b/hello.txt");

      //printf("res: %i\n", res->status_code);
      //if (res->headers)
      //{
      //  for (flu_node *n = res->headers->first; n; n = n->next)
      //  {
      //    printf("* %s: %s\n", n->key, (char *)n->item);
      //  }
      //}

      expect(res->status_code i== 200);
      expect(res->body === "hello world\n");

      expect(flu_list_get(res->headers, "x-accel-redirect") === ""
        "../spec/www/a/b/hello.txt");
    }

    it "doesn't actually serve the file if x-real-ip is set"
    {
      res = fcla_get_d(
        "http://127.0.0.1:4001/files/a/b/hello.txt",
        "X-Real-IP", "127.0.0.1", NULL);

      //printf("res: %i\n", res->status_code);
      //if (res->headers)
      //{
      //  for (flu_node *n = res->headers->first; n; n = n->next)
      //  {
      //    printf("* %s: %s\n", n->key, (char *)n->item);
      //  }
      //}

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
  }

  describe "shervin and auth"
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

    it "starts a session upon sucessful login"
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
      res = fcla_get("http://127.0.0.1:4001/secret");

      flu_putf(fcla_response_to_s(res));
      expect(res->status_code i== 401);

      fcla_response_free(res);
      res = fcla_post(
        "http://127.0.0.1:4001/login", NULL, "u=toto;p=toto");

      expect(res->status_code i== 200);

      flu_putf(flu_list_to_sm(res->headers));
      char *cookie = rdz_strdup(flu_list_get(res->headers, "set-cookie"));
      *strchr(cookie, ';') = 0;

      fcla_response_free(res);
      res = fcla_get_d(
        "http://127.0.0.1:4001/secret", "cookie", cookie, NULL);

      flu_putf(fcla_response_to_s(res));
    }
  }
}

