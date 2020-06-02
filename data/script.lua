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

function OnImport(a)
  print("On import:", a)
end

tblring={size=20, st=0}
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

for i = 1,tblring.size do tblring:add({0}) end

gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
btn = gui.wnd:NewButton(10,10,"Button")
ed = gui.wnd:NewEdit(0, 100, "")
ed.enabled = false
plot = gui.wnd:NewPlot(100,0, 500, 500)
plot.format={}

TTYlib = require("tty")
tty = TTYlib:OpenTTY("/dev/ttyUSB0", 9600)
tty:timeout(100)

plot.data = tblring

while gui:update() do
  dat = tty:gets()
  if dat ~= nil then
    dat = dat:gsub("%.", ",")
    dat = tonumber(dat)
    if dat ~= nil then
      tblring:add( {dat} )
      plot:Refresh()
      ed.text = string.format("%.1f", dat)
    end
  end
  gui:delay_ms(10)
end
