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
for i = 1,tblring.size do tblring:add({0,0,0,0}) end
for i=1,10 do aaa[i] = {i,math.sin(i)} end


gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
btn = gui.wnd:NewButton(10,10,"Button")
btn2 = gui.wnd:NewButton(10,50, "Apply radiobutton")
ed = gui.wnd:NewEdit(10,100, "какой-то текст")
plot = gui.wnd:NewPlot(100,0, 500, 500)
plot.format={x=1}
rb1 = gui.wnd:NewRadioButton(10, 140, "Regular plot", nil)
rb1.data = aaa
rb2 = gui.wnd:NewRadioButton(10, 170, "Circular buffer", rb1)
rb2.data = tblring
lbl = gui.wnd:NewLabel(10, 200, "Label")
tc = gui.wnd:NewTabControl(600, 0, 200, 400);
tc[0] = {text="Tab1"}
tc[1] = "Tab2"
sc = gui.wnd:NewScroller(5, 250)

function sc:OnChange(val)
  print(val)
end

check = tc[0]:NewCheckBox(10, 10, "Inversion progress")
spined = tc[0]:NewSpinEdit(10, 30)
pb = tc[0]:NewProgressBar(10, 100, 100, 20)
pb.min, pb.max = 0, tblring.size

ed.text = "TPATATA"

function btn:OnClick()
  name = gui.wnd:OpenFileDialog("TPATATA")
  print("Selected file: ",name)
end

function btn2.OnClick()
  plot.data = rb1:GetSelected().data
end

plot.data = tblring
--plot.data = aaa

while gui:update() do
  collectgarbage("collect")
  --io.write("\r", tostring(collectgarbage("count")*1024))
  --io.flush()
  tblring:add({math.sin(x*0.1), math.sin(x*0.15), math.sin(x*0.13), math.sin(x*0.17)})
  plot:Refresh()
  x = x+1
  gui:delay_ms(10)
  if check.checked then
    pb.val = tblring.st
  else
    pb.val = tblring.size - tblring.st
  end
end
