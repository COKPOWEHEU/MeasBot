package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

sr570 = require("sr570")

sr570:help()

sr_object = sr570:connectNewDevice("/dev/ttyUSB0");

--sr_object:reset()

sr_object:setSens(1e-3)
sr_object:setSenCal(0)
sr_object:setSigInvertSense(1)
sr_object:setUncalSensVernier(0)

sr_object:setCalOffsetCurrentLVL(2e-12)
sr_object:setUncalInOffsetVernier(0)
sr_object:setInOffsetCalMode(0)
sr_object:turnInOffsetCurr(0)
sr_object:setInOffsetCurrSign(1)

sr_object:setTypeFilter(2)
sr_object:setHighFilter(0.3)
sr_object:setLowFilter(10*1000)

sr_object:turnBiasVolt(0)
sr_object:setBiasVoltLVL(0)
sr_object:setGainMode(0)