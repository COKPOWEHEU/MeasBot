package.cpath = package.cpath .. ";?.so"
package.cpath = package.cpath .. ";?.dll"

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

dev = require("experimental")

print("DEV")
tblprint(dev)
t = dev:export()
print("export")
tblprint(t)

p = {}
p.Var1 = "asdf"
p.Var2 = 42
dev:import(p)
print("DEV")
tblprint(dev)

tbl2file(t, t)