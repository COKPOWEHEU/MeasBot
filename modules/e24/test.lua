--package.cpath = package.cpath .. ";modules/e24/\\?.so"
e24 = require("e24")

check = e24.e24_init("test.txt")

if check == 0 then
  print("File don't open")
  os.exit()
end

print(e24.e24_read())
print(e24.e24_read())
print(e24.e24_read())

e24.e24_close()