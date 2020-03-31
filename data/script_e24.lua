package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

e24 = require("e24")

print(e24:help())

print(e24:readADC(1), e24:readADC(2), e24:readADC(3), e24:readADC(4))

print(e24.channel1:getVoltage())
e24.channel1:setRange(1)
print(e24.channel2:getVoltage())
e24.channel2:setRange(2)
print(e24.channel3:getVoltage())
e24.channel3:setRange(3)
print(e24.channel4:getVoltage())
e24.channel4:setRange(4)

print()

print(e24.channel1:getVoltage())
print(e24.channel2:getVoltage())
print(e24.channel3:getVoltage())
print(e24.channel4:getVoltage())