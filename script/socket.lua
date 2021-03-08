local socket = require("socket")

local c = assert(socket.connect("localhost", 9080))

c:send("ddd")

c:close()
