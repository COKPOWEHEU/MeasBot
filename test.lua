--Хак с путями в винде и линуксе. Ужасная штука, но вроде работает
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";../modules/lib/lib\\dyn/?.dll"
--package.cpath = package.cpath .. ModuleSuffix()

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

gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(1024, 768, "Test gtk")
plot = gui.wnd:NewPlot(0,0,800,600)
plot.data = {{0,0}, {1,1}}
plot.format = {x=1, 2}
pnum = 2

gui.argX = gui.wnd:NewSpinEdit(850, 60, -10, 10, 2, 0.1)
gui.argX.value = 0
gui.argY = gui.wnd:NewSpinEdit(850, 110, -10, 10, 2, 0.1)
gui.argY.value = 0
gui.btn = gui.wnd:NewButton(850, 180, "Add point")

function gui.btn:OnClick()
  x = gui.argX.value
  y = gui.argY.value
  print(x, " ", y)
  plot.data[pnum] = {x, y}
  pnum = pnum + 1
end

while gui:update() do
  plot:Refresh()
  gui:delay_ms(10)
end
