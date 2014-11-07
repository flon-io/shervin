
//
// specifying shervin
//
// Sun Sep 21 15:35:29 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods

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
    system("lsof -i :4001");
  }

  describe "shervin"
  {
    before each
    {
      fcla_response *res = NULL;
    }
    after each
    {
      if (res) fcla_response_free(res);
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
      res = fcla_get_h(
        "http://127.0.0.1:4001/files/a/b/hello.txt",
        flu_d("X-Real-IP", "127.0.0.1", NULL));

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
  }
}

