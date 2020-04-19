package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

lps305 = require("lps305")

print(lps305:help())

obj = lps305:connect("/dev/ttyUSB0", 9600)

if obj == nil then
  os.exit()
end

obj:setVoltage(1, 5)
obj:getVoltage(1)

obj:setCurrent(1, 0.5)
obj:getCurrent(1)

obj:setOutput(0)

obj:setModeTracking(1)

obj:setBeeper(1)

obj:getStatus()

obj:getModel()
obj:getVerion()

obj:close()
