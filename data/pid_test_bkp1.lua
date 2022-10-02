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

tblring={size=1000, st=0}
tblm={data={}}
function tblm:__index(idx)
  local mt = getmetatable(self)
  idx = idx % self.size
  idx = (self.size + self.st + 1 - idx) % self.size
  --idx = (idx + self.st) % self.size
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

for i = 1,tblring.size do tblring:add({0,0,0}) end


gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(1500, 600, "Test gtk")
btn = gui.wnd:NewButton(10,550,"Button")
plot = gui.wnd:NewPlot(0,0, 1500, 500)
plot.format={}
plot.data = tblring

Th = 0
lh = 1
ch = 10

T = 0
lambda = 0.1
c = 1

P_max = 100000
P = P_max
dt = 0.01

function calc_T()
  local Ph = lh*(Th-T)
  local Phc = lh*Th*Th/100
  Th = Th + ((P-Ph-Phc)/ch)*dt
  if Th > 2000 then Th = 2000 end
  local Pc = lambda*T*T/100
  T = T + ((Ph-Pc)/c)*dt
  --local Pc = lambda*T
  --T = T + ((P-Pc)/c)*dt
end

function btn:OnClick()
  if P == 0 then
    P = P_max
  else
    P = 0
  end
end

Te = 300
function stab()
  if T > Te then
    P = 0
  else
    P = P_max
  end
end

t=0
while gui:update() do
  plot:Refresh()
  gui:delay_ms(10)
  t = t+dt
  calc_T()
  stab()

  local pt = 0
  if P ~= 0 then pt = Te end
  tblring:add({pt, T, Th})
end
