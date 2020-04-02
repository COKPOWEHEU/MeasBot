package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

e24 = require("e24")

print(e24:help())

--print(e24:readADC(1), e24:readADC(2), e24:readADC(3), e24:readADC(4))

print(e24[1]:getVoltage())
e24[1]:setRange(4)
print(e24[2]:getVoltage())
e24[2]:setRange(1.5)
print(e24[3]:getVoltage())
e24[3]:setRange(3)
print(e24[4]:getVoltage())
e24[4]:setRange(3.4)

print()

print(e24[1]:getVoltage())
print(e24[2]:getVoltage())
print(e24[3]:getVoltage())
print(e24[4]:getVoltage())
