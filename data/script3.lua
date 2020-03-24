package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"
TTY = require("tty")

tty = TTY:OpenTTY("/dev/ttyUSB0", 9600)
tty:timeout(100)
print(tty)

res = tty:puts("AAA")
print(res)
res = tty:gets()
print(res)
