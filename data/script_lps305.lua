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

obj.neg:setVoltage(2)
print(obj.neg:getVoltage())
obj.neg:setCurrent(0.05)
print(obj.neg:getCurrent())

obj.both:setVoltage(2)
print(obj.both:getVoltage())

obj:disconnect()