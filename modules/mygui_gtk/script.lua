--package.cpath = package.cpath .. ";./?.so"
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

gui = require("mygui_gtk")
print("GUI = ", gui)
tblprint(gui)
btn = gui:NewButton(10,10,"Button")
print("btn = ", btn)
btn2 = gui:NewButton(10,50, "Кнопко")
print("btn2 = ", btn2)
ed = gui:NewEdit(10,100, "какой-то текст")
print("ed = ", ed)
tst = gui:NewTest()
print("tst = ", tst)
t2 = gui.NewTest()
print("t2 = ", t2)

print(btn:WasClicked())
ed:SetText("TPATATA")

--[[mt = getmetatable(gui)
print("MT=")
mt = mt.pool
setmetatable(mt, {__mode="v"})
tblprint(mt)
print("-MT")
--]]

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
  print("Button 2 AAA")
  btn = nil
  t2 = nil
  ed = nil
end

while gui:update() do
  collectgarbage("collect")
  io.write("\r", tostring(collectgarbage("count")*1024))
  io.flush()
  
  gui:delay_ms(100)
end


tblprint(gui)
gmt = getmetatable(gui)
print("GUI pool = ", gmt.pool)
if gmt.pool ~= nil then
  tblprint(gmt.pool)
end

print("\n-----------END-------------")
