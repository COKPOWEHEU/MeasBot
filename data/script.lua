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
  
gui = require("mygui_gtk")
print("GUI = ", gui)
tblprint(gui)
wnd = gui:NewWindow(500, 500, "Test window")

function wnd:OnDestroy()
  print("Destroy")
end

while gui:update() do
  collectgarbage("collect")
  io.write("\r", tostring(collectgarbage("count")*1024))
  io.flush()
  gui:delay_ms(10)
end


gmt = getmetatable(gui)
print("GUI pool = ", gmt.pool)
if gmt.pool ~= nil then
  tblprint(gmt.pool)
end

print("\n-----------END-------------")
