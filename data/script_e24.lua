package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

e24 = require("e24")

e24:help()
print(e24[1]:getRange())
e24[1]:setRange(0.1)
print(e24[1]:getRange())
print(e24[1]:readVoltage())