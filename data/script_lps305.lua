package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

lps305 = require("lps305")

print(lps305:help())

obj = lps305:connectNewDevice("/dev/ttyUSB0")

print(obj:getModel())
print(obj:getVersion())
print(obj:getDeviceHelp())

obj:setOutput(0)

obj:disconnect()