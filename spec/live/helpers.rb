
#
# helpers for spec/live/
#
# Fri Sep 18 06:01:28 JST 2015
#

require 'socket'


def hrequest(req)

  head = []
  length = nil
  body = nil

  TCPSocket.open('localhost', 4001) do |s|

    s.print(req)

    while l = s.gets

      head << l unless [
        /^date:/i, /^server:/i, /^location:/i, /^x-flon-shervin:/i
      ].find { |r| l.match(r) }

      break if l == "\r\n"

      if m = l.match(/content-length: *(\d+)\r/i)

        length = m[1].to_i
      end
    end

    body = s.read(length)
  end

  [ head.join, body ]
end

