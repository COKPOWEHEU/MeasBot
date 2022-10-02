--Хак с путями в винде и линуксе. Ужасная штука, но вроде работает
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

function tblprint(tbl, pref, postf)
  if tbl == nil then print("---nil---"); return; end
  if pref == nil then pref="[" end
  if postf== nil then postf="]" end
  local mt = getmetatable(tbl)
  if mt ~= nil then
    tblprint(mt, pref..pref, postf..postf)
  end
  mt = tbl
  print("(", tbl, ")")
  for idx,val in pairs(mt) do
    print(pref, idx, postf, " = ", val)
  end
  if pref == "" or pref == nil then
    print("-------------------------------")
  end
end

function delay(sec)
  local cur = os.time()
  local fin = cur + sec
  repeat
    cur = os.time()
  until cur >= fin
end

gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(1000, 1000, "Test gtk")
plot = gui.wnd:NewPlot(0,0, 1000, 1000)
plot.format={x=2, 3}

dev = require("akip_114x"):connectNewDevice("/dev/ttyUSB0")
tblprint(dev)

print("ID ", dev:getID())
print("Err", dev:error())
print("U=", dev:getVoltage())
print("I=", dev:getCurrent())

print("---SET---")

dev:setVoltage(0)
dev:setCurrent(0.5)
print("Err", dev:error())
print("U=", dev:getVoltage())
print("I=", dev:getCurrent())

print("---set output ---")
dev:setOutput(1)
print("U=", dev:getVoltage())
print("I=", dev:getCurrent())
print("P=", dev:getPower())


local Umax = 200
local N = 15
local dU = Umax / N
local i=0

for U=0, Umax, dU do
  i = i+1
  dev:setVoltage(U)
  gui:delay_ms(300)
  local rU = dev:getVoltage()
  local rI = dev:getCurrent()
  plot.data[i] = {U, rU, rI}
  print(U, rU, rI)
  plot:Refresh()
  gui:update()
end
for U=Umax, 0, -dU do
  i = i+1
  dev:setVoltage(U)
  gui:delay_ms(300)
  local rU = dev:getVoltage()
  local rI = dev:getCurrent()
  plot.data[i] = {U, rU, rI}
  print(U, rU, rI)
  plot:Refresh()
  gui:update()
end

while(gui:update()) do
  
end

dev:setVoltage(0)
dev:setCurrent(0)

dev = nil
print("Done")