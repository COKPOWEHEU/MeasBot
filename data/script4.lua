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
rbtn1 = gui.wnd:NewRadioButton(10, 50, "Line 1", nil)
rbtn1.val = 1
print("rbtn1 = ", rbtn1)
rbtn2 = gui.wnd:NewRadioButton(10, 80, "Line 2", rbtn1)
rbtn2.val = 2
print("rbtn2 = ", rbtn2)
rbtn3 = gui.wnd:NewRadioButton(10, 110,"Line 3", rbtn1)
rbtn3.val = 3
print("rbtn3 = ", rbtn3)

function btn:OnClick()
  sel = rbtn1:GetSelected()
  if sel ~= nil then
    print(sel.val)
  end
end

while gui:update() do
  collectgarbage("collect")
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
