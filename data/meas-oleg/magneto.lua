require("math")

local magneto = {}

-- сохранение предыдущего значения магнитного поля
magneto.old_field = 0

local delay = require("utils").sleep

--[[
  @brief
	Подключение к источнику питания для магнита
	
	@param path:string - путь к терминалу, соединённого со стандартным вводом

	@return 1 - успешное подключение к приборам
	@return nil - неудача во время подключения к какому-то из прибору

  @example
    *table of magnet*:connect("/dev/ttyUSB2")
]]--
function magneto:connect(path)
	self.source = require("lps305"):connectNewDevice(path)
	if self.source == nil then return nil end
	self.magnetometr = require("e24")[1] --change to number of channel
	if self.magnetometr == nil then return nil end
	self.source:setOutput(1)
	return 1
end

--[[
  @brief
	Изменение полярности магнита
	
	@param polarity:boolean
		*true - положительная
		*false - отрицательная

  @example
    *table of magnet*:setPolarity(true)
]]--
function magneto:setPolarity(polarity)
  if polarity then self.source:setDigitalOut(0)
  else self.source:setDigitalOut(5)
  end
end

--[[
  @brief
	Преобразование значения магнитного поля в напряжение
	
	@param field:number - значение магнитного поля
	@return Значение напряжения типа number

  @example
    *table of magnet*:fieldToU(0.004)
]]--
function magneto:fieldToU(field)
	return field * 89.25 -- коэф. вычислен опытным путём
end

--[[
  @brief
	Изменение магнитного поля
	
	@param field:number - требуемое магнитное поле

  @example
    *table of magnet*:setField(0.01)
]]--
function magneto:setField(field)
	if field < 0 and self.old_field == 0 then self.old_field = 0.01 end
	if(field*self.old_field < 0) then
		self:setField(0)
		delay(0.5)
		self:setPolarity(field > 0)
	end
	
	local new_field = math.abs(field)
	self.old_field = math.abs(self.old_field)
	local diff

	delay(magneto.tab.delay.value)
	
	if(new_field > self.old_field) then diff = 0.5
	else diff = -0.5
	end
		
  local old_U = self:fieldToU (self.old_field)
	local new_U = self:fieldToU (new_field)
	
	for i = old_U, new_U, diff do
		self.source.both:setVoltage(i)
		delay(0.5)
	end
	self.source.both:setVoltage(new_U)
	delay(0.5)
	delay(magneto.tab.delay.value)
	self.old_field = field
end

--[[
  @brief
	Вычисление значения магнитного поля
	
	@return Возвращает результат вычисления типа number

  @example
    *table of magnet*:getField()
]]--
function magneto:getField()
	local Uout = self.magnetometr:getVoltage()
	local recount_ratio = 70--some value
	local field_T = Uout * recount_ratio * 0.1
	return field_T
end

--[[
  @brief
	Настройка элементов инициализации в окне/вкладке
	*Перед вызовом этой функции присвойте полю tab таблицы магнита значение окна/вкладки

	@example
		*table of magnet*.tab = gui.window
		или
		*table of magnet*.tab = gui.window.tab[n]
    *table of magnet*:setup()
]]--
function magneto:setup()
	self.tab.label_mag = self.tab:NewLabel(10,30,"Введите путь к lps305")
  self.tab.mag_path = self.tab:NewEdit(10, 50, "/dev/tty_cp210x_0")

	self.tab.label_delay = self.tab:NewLabel(200, 30, "Delay(ms)")
	self.tab.delay = self.tab:NewSpinEdit(200, 50, 0, 1000000)
	self.tab.delay.step = 100
	self.tab.delay.digits = 0
end

--[[
  @brief
	Деинициализация - установка настроек приборов в безопасное состояние

	@example
    *table of magnet*:deinit()
]]--
function magneto:deinit()
	self.source.both:setVoltage(0)
end

return magneto
