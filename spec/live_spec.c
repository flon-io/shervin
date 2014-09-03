
//
// specifying shervin
//
// Thu Sep  4 06:51:40 JST 2014
//

#include <curl/curl.h>

#include "shervin.h"
//#include "shv_protected.h" // direct access to shv_request methods


context "live"
{
  describe "curl"
  {
    it "works"
    {
      CURL *curl;
      CURLcode res;

      curl = curl_easy_init();

      ensure(curl != NULL);

      curl_easy_setopt(curl, CURLOPT_URL, "http://lambda.io");

      res = curl_easy_perform(curl);

      ensure(res == CURLE_OK);
    }
  }
}

