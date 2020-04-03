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

aaa = {}
x = 1
for i = 1,tblring.size do tblring:add({0,0,0,0}) end
for i=1,10 do aaa[i] = {i,math.sin(i)} end


gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
print("GUI.WND = ", gui.wnd)
tblprint(gui.wnd)
btn = gui.wnd:NewButton(10,10,"Button")
print("btn = ", btn)
btn2 = gui.wnd:NewButton(10,50, "Кнопко")
print("btn2 = ", btn2)
ed = gui.wnd:NewEdit(10,100, "какой-то текст")
print("ed = ", ed)
tst = gui.wnd:NewTest()
t2 = gui.wnd.NewTest()
plot = gui.wnd:NewPlot(100,0, 500, 500)
print("plot = ", plot)
tblprint(plot)
plot.format={x=1}
rb1 = gui.wnd:NewRadioButton(10, 140, "Regilar plot", nil)
rb1.data = aaa
rb2 = gui.wnd:NewRadioButton(10, 170, "Circular buffer", rb1)
rb2.data = tblring
lbl = gui.wnd:NewLabel(10, 200, "Label")
--pb = gui.wnd:NewProgressBar(0, 230, 100, 20)
--pb.min, pb.max = 0, tblring.size
tc = gui.wnd:NewTabControl(600, 0, 200, 400);
tc[0] = {text="Tab1"}
tc[1] = "Tab2"

--name = gui.wnd:OpenFileDialog("TPATATA")
--res = gui.wnd:QuestionDialog()

check = tc[0]:NewCheckBox(10, 10, "BBB")
spined = tc[0]:NewSpinEdit(10, 30)
progress1 = tc[0]:NewProgressBar(10, 100, 30, 20)
progress2 = tc[0]:NewProgressBar(10, 150, 100, 100)
progress2.min = -1
progress2.max = 2

print(progress2.max, progress2.min)
function func(x)
  progress1.val = x
  progress2.val = x
end

mt = getmetatable(tc)
print("Tc.tab = ")
tblprint(mt.tab)

print("tab[0] = ", tc[0])
tblprint(tc[0])

ed.text = "TPATATA"

print("DELETE")
tst = nil
collectgarbage("collect")
print("place to GC")
print("GUI = ", gui)
tblprint(gui)

function btn.OnClick()
  --print(ed:GetText())
  local txt = lbl.text
  print(txt)
  lbl.text = "Text"
end

function btn2.OnClick()
  --tblprint(plot.data)
  --[[if plot.data == tblring then
    plot.data = aaa
  else
    plot.data = tblring
  end--]]
  --plot.data = 
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
  --[[if check:IsChecked() then
    pb.val = tblring.st
  else
    pb.val = tblring.size - tblring.st
  end--]]
  func(0.5+0.5*math.sin(x*0.1))
end

--[[
tblprint(gui)
gmt = getmetatable(gui)
print("GUI pool = ", gmt.pool)
if gmt.pool ~= nil then
  tblprint(gmt.pool)
end
--]]

print("\n-----------END-------------")
--]]
