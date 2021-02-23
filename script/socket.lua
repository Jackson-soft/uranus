local socket = require("socket")

local c = assert(socket.connect("localhost", 8090))

c:send("ddd")

c:close()
