package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

e24 = require("e24")

print(e24:help())

print(e24:readADC(1), e24:readADC(2), e24:readADC(3), e24:readADC(4))
