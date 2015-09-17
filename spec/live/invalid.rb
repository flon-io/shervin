
require File.dirname(__FILE__) + '/helpers.rb'


head, body = hrequest(
  "GET /nada HTTP...\r\n" +
  "Host: localhost\r\n" +
  "\r\n")

puts "=" * 80
puts head
puts body
puts "=" * 80
$stdout.flush

