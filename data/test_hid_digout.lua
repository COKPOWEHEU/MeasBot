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
--[[
gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(1000, 1000, "Test gtk")
plot = gui.wnd:NewPlot(0,0, 1000, 1000)
plot.format={x=2, 3}
]]
dev = require("hid_digout"):connectNewDevice()
tblprint(dev)

dev.A = 0xF
print(dev:help())
print(dev.A)

print("---")
tblprint(dev)
--[[
while(gui:update()) do
  
end
]]

dev = nil
print("Done")