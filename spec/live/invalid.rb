
require 'socket'

def hrequest(req)

  head = []
  length = nil
  body = nil

  TCPSocket.open('localhost', 4001) do |s|

    s.print(req)

    while l = s.gets

      head << l

      break if l == "\r\n"

      if m = l.match(/content-length: *(\d)+\r/i)

        length = m[1].to_i
      end
    end

    body = s.read(length)
  end

  [ head.join, body ]
end

head, body = hrequest(
  "GET /nada HTTP/1.1\r\n" +
  "Host: localhost\r\n" +
  "\r\n")

puts "=" * 80
puts head
puts body
puts "=" * 80
$stdout.flush

