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

function func(tbl)
  print(tbl)
  tblprint(tbl)
end

function OnImport(a)
  print("On import:", a)
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
  
gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
print("GUI = ", gui)
tblprint(gui)
btn = gui.wnd:NewButton(10,10,"Button")
print("btn = ", btn)
btn2 = gui.wnd:NewButton(10,50, "Кнопко")
print("btn2 = ", btn2)
ed = gui.wnd:NewEdit(10,100, "какой-то текст")
print("ed = ", ed)
tst = gui.wnd:NewTest()
print("tst = ", tst)
t2 = gui.wnd.NewTest()
print("t2 = ", t2)
plot = gui.wnd:NewPlot(100,0, 500, 500)
print("plot = ", plot)
tblprint(plot)
plot.format={x=1}
--plot.data = {}
tblprint(plot.format)

print(btn:WasClicked())
ed:SetText("TPATATA")


print("DELETE")
tst = nil
collectgarbage("collect")
print("place to GC")
print("GUI = ", gui)
tblprint(gui)


function btn.OnClick()
  print(ed:GetText())
end

function btn2.OnClick()
  --tblprint(plot.data)
  if plot.data == tblring then
    plot.data = aaa
  else
    plot.data = tblring
  end
end

aaa = {}
x = 1
for i = 1,tblring.size do tblring:add({0,0,0,0}) end
for i=1,10 do aaa[i] = {i,math.sin(i)} end

plot.data = tblring
--plot.data = aaa

while gui:update() do
  collectgarbage("collect")
  io.write("\r", tostring(collectgarbage("count")*1024))
  io.flush()
  tblring:add({math.sin(x*0.1), math.sin(x*0.15), math.sin(x*0.13), math.sin(x*0.17)})
  plot:Refresh()
  x = x+1
  gui:delay_ms(10)
end


tblprint(gui)
gmt = getmetatable(gui)
print("GUI pool = ", gmt.pool)
if gmt.pool ~= nil then
  tblprint(gmt.pool)
end

print("\n-----------END-------------")
--]]
