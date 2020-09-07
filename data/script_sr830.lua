--Хак с путями в винде и линуксе. Ужасная штука, но вроде работает
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

sr830_lib = require("sr830")

sr830_lib.help()