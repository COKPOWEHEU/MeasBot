--Хак с путями в винде и линуксе. Ужасная штука, но вроде работает
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"
--package.cpath = package.cpath .. ModuleSuffix()
--TODO эту загрузку надо как-то починить, а то больно криво смотрится

function tblprint(tbl, pref, postf)
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

tblring={size=100, st=0}
tblm={data={}}
function tblm:__index(idx)
  local mt = getmetatable(self)
  idx = (idx + self.st) % self.size
  local res = rawget(mt.data, idx)
  if res == nil then res = rawget(mt.data, 0) end
  return res
end
function tblm:__newindex(idx, val)
  local mt = getmetatable(self)
  local i = tonumber(idx)
  if i == nil then
    rawset(self, idx, val)
  else
    i = i % self.size
    rawset(mt.data, i, val)
  end
end
function tblm:__len()
  return self.size
end
setmetatable(tblring, tblm)
function tblring:add(val)
  self[self.st] = val
  self.st = (self.st-1)%self.size
end

aaa = {}
x = 1
for i = 1,tblring.size do tblring:add({0,0}) end

gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
plot = gui.wnd:NewPlot(100,0, 500, 500)
plot.format={x=1}
plot.data = tblring

lps = require("lps305"):connectNewDevice("/dev/ttyUSB0").pos
if lps == nil then goto __exit__ end
print(lps.device:getModel())
print(lps.device:getVersion())
print(lps.device:getDeviceHelp())
lps.device:setOutput(1)
lps:setCurrent(1)


for i=1,10 do
  if gui:update() == false then break end
  collectgarbage("collect")
  lps:setVoltage(i)
  v = {i, lps:getVoltage()}
  print(v[1], v[2])
  tblring:add(v)
  plot:Refresh()
  --gui:delay_ms(1000)
end
print("Done")

while gui:update() do end

::__exit__::
print("Done")
