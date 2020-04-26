package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

lps305 = require("lps305")

print(lps305:help())

obj = lps305:connectNewDevice("/dev/ttyUSB0")

print(obj:getModel())
print(obj:getVersion())
print(obj:getDeviceHelp())

obj:setOutput(0)

obj.pos:setVoltage(5)
print(obj.pos:getVoltage())
obj.pos:setCurrent(0.1)
print(obj.pos:getCurrent())

obj.pos:setVoltage(2)
print(obj.pos:getVoltage())
obj.pos:setCurrent(0.05)
print(obj.pos:getCurrent())

obj:disconnect()