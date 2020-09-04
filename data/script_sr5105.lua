--Хак с путями в винде и линуксе. Ужасная штука, но вроде работает
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"
--package.cpath = package.cpath .. ModuleSuffix()
--TODO эту загрузку надо как-то починить, а то больно криво смотрится

function tblprint(tbl, pref, postf)
  if pref == nil then pref="[" end
  if postf== nil then postf="]" end
  if type(tbl) ~= "table" then 
    print("wrong table:", type(tbl))
    return
  end
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

sr5105_lib = require("sr5105")
tblprint(sr5105_lib)

sr5105 = sr5105_lib:connectNewDevice("/dev/ttyUSB0", 4800)
if sr5105 == nil then os.exit(); end
--[[
tblprint(sr5105)

gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
fldX = gui.wnd:NewLabel(0,0,"")
fldY = gui.wnd:NewLabel(0,20,"")
fldSens = gui.wnd:NewEdit(0,40,"1")
fldSens.prev = 1
resSens = gui.wnd:NewLabel(200,40,"a")

while gui:update() do
  fldSens.cur = tonumber(fldSens.text)
  if fldSens.cur ~= nil then
    if fldSens.cur ~= fldSens.prev then
      local s = sr5105:setSens(fldSens.cur)
      resSens.label = "Real: " .. tostring(s)
      fldSens.prev = fldSens.cur
    end
  end
  local r,a,m = sr5105:getMagPhase()
  fldX.label = tostring(r)
  fldY.label = tostring(m)
end
--]]

print(sr5105:getXY()) --OK



--print(sr5105:getSens())
--sens = sr5105:setSens(1e-1) --OK
--print(sens)
--print(sr5105:rawSend("SEN\r")) --OK
--x,y = sr5105:getXY()
--print(x, y, x*sens/1000)
--print(sr5105:getErrors())
