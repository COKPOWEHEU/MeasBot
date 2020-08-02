package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

sr570 = require("sr570")

sr570:help()

sr_object = sr570:connectNewDevice("/dev/ttyUSB0");