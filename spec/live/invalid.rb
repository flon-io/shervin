
require File.dirname(__FILE__) + '/helpers.rb'


head, body = hrequest(
  "GET /nada HTTP...\r\n" +
  "Host: localhost\r\n" +
  "\r\n")

print head
print body
$stdout.flush

