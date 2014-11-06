
//
// specifying shervin
//
// Thu Nov  6 15:37:01 JST 2014
//

#include "shervin.h"
//#include "shv_protected.h" // direct access to shv_request methods


context "handlers"
{
  before each
  {
    struct ev_io *eio = calloc(1, sizeof(shv_con));
    shv_con *con = calloc(1, sizeof(shv_con));
    eio->data = con;
  }
  after each
  {
    for (size_t i = 0; con->routes[i] != NULL; ++i)
    {
      flu_list_free(con->routes[i]->params);
      free(con->routes[i]);
    }
    //free(con->routes); // not malloc'ed
    shv_request_free(con->req);
    shv_response_free(con->res);
    free(con);
    free(eio);
  }

  describe "shv_dir_handler()"
  {
    it "serves files"
    it "returns 404 if the file is not found"
    it "returns 404 when the request goes ../"
  }
}

