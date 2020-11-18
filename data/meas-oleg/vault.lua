local vault = require("utils"):buffer()
vault.push_back = vault:add()

--[[
  @brief
  Сохранение данных из таблицы *table of vault* в файл и закрытие файла

  @param file:table - таблица указывающая на открытый файл

  @example
    file = io.open("some_file.txt", "w")
    *table of vault*:saveData(gui.tab[n])
]]--
function vault:saveData(file)
	file:seek("end")
	for i = 1, self.size do
		j = 1
		while true do
			if self[i][j] == nil then break end
			file:write(self[i][j] .. '\t')
			j = j + 1
		end
		file:write('\n')
	end
	file:close()
end

return vault