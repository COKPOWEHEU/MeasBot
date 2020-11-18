local voltmeter = {}   --sr830.SineIn
local source = {}      --sr830.SineOut


-- сохранение полей с указателями таблиц друг на друга
-- *Данная вещь сделана чтобы избежать двойного/повторного подключения к прибору*
voltmeter.source = source
source.voltmeter = voltmeter


--[[
  @brief
	Подключение к вольтметру/источнику питания
	
	@param path:string - путь к терминалу, соединённого со стандартным вводом

	@return 1 - успешное подключение к приборам
	@return nil - неудача во время подключения к какому-то из прибору

  @example
    *table of voltmeter/source*:connect("/dev/ttyUSB1")
]]--
function voltmeter:connect(path)
	if self.srs ~= nil then return end
	local sr830 = require("sr830"):connectNewDevice(path)
	print(sr830)
	if sr830 == nil then return nil end
	self.srs = sr830.SineIn
	self.source.srs = sr830.SineOut
	return 1
end

function source:connect(path)
	if self.srs ~= nil then return end
	local sr830 = require("sr830"):connectNewDevice(path)
	if sr830 == nil then return nil end
	self.srs = sr830.SineOut
	self.voltmeter.srs = sr830.SineIn
	return 1
end

--[[
  @brief
  Инициализация вольтметра/источника питания

  @example
    *table of voltmeter/source*:init()
]]--
function voltmeter:init()
	self.srs:setInputMode(self.tab.input.selected-1)
	self.srs:setInputGrounded(self.tab.inGND.checked)
	self.srs:setInputCap(self.tab.inCap.checked)
	self.srs:setRange(self.tab.inRange.value)
	self.srs:setDynamicReserve(self.tab.dynRes.selected - 1)
	self.srs:setStateOfSyncFilter(self.tab.syncFilter.checked)
	self.srs:bandpassFiltersEnable(self.tab.bandpassFilt.checked)
	self.srs:setTimeConst(self.tab.timeconst.value)
	self.srs:setSlope(self.tab.slope.value)

end

function source:init()
	self.srs:setFreq(self.tab.oFreq.value)
  self.srs:setVoltage(self.tab.oVolt.value)
end

--[[
  @brief
	Настройка элементов инициализации в окне/вкладке
	*Перед вызовом этой функции присвойте полю tab таблицы вольтметра/источника питания значение окна/вкладки

	@example
		*table of voltmeter/source*.tab = gui.window
		или
		*table of voltmeter/source*.tab = gui.window.tab[n]
    *table of voltmeter/source*:setup()
]]--
function voltmeter:setup()
	self.tab.label_volt = self.tab:NewLabel(10,10,"Введите путь к sr830")
  self.tab.volt_path = self.tab:NewEdit(10, 30, "/dev/tty_hl340_0")

	self.tab.lab1 = self.tab:NewLabel(300,10, "Вольтметр")
	self.tab.inGND = self.tab:NewCheckBox(300, 30, "Заземление входа")
	self.tab.inGND.checked = false

	self.tab.inCap = self.tab:NewCheckBox(300, 60, "Режим входа AC(true)/DC(false)")
	self.tab.inCap.checked = false

	self.tab.label_range = self.tab:NewLabel(300, 100, "Чувствительность")
	self.tab.inRange = self.tab:NewSpinEdit(300, 120, 0, 1)
	self.tab.inRange.value = 60e-6
	self.tab.inRange.step = 1e-9
	self.tab.inRange.digits = 10

	self.tab.label_DynReserve = self.tab:NewLabel(300, 160, "DynReserve")
	self.tab.dynRes = self.tab:NewComboBox(300, 180)
	self.tab.dynRes:SetItems({"High Reserve", "Normal", "Low Noise"})
	self.tab.dynRes.selected = 2

	self.tab.syncFilter = self.tab:NewCheckBox(300, 220, "Синхронный фильтр")
	self.tab.syncFilter.checked = false

	self.tab.bandpassFilt = self.tab:NewCheckBox(300, 250, "Широкополосный фильтр")
	self.tab.bandpassFilt.checked = false


	self.tab.label_timeconst = self.tab:NewLabel(300, 280, "Time const") -- change
	self.tab.timeconst = self.tab:NewSpinEdit(300, 300, 0, 3e+4)
	self.tab.timeconst.value = 0.1
	self.tab.timeconst.step = 1e-5
	self.tab.timeconst.digits = 5

	self.tab.label_slope = self.tab:NewLabel(300, 340, "Slope")
	self.tab.slope = self.tab:NewSpinEdit(300, 360, 6, 24)
	self.tab.slope.value = 12--24
	self.tab.slope.step = 6
	self.tab.slope.digits = 0

	self.tab.label_input = self.tab:NewLabel(300, 400, "Input Mode")
	self.tab.input = self.tab:NewComboBox(300, 420)
	self.tab.input:SetItems({"A", "A-B", "Curr 1 MOhm", "Curr 100 MOhm"})
  self.tab.input.selected = 2

end

function source:setup()
	self.tab.lab2 = self.tab:NewLabel(600,10, "Источник напряжения")

	self.tab.label_freq = self.tab:NewLabel(600, 30, "Частота")
	self.tab.oFreq = self.tab:NewSpinEdit(600, 50, 0, 102000)
	self.tab.oFreq.value = 133
	self.tab.oFreq.step = 0.001
	self.tab.oFreq.digits = 3

	self.tab.label_volt = self.tab:NewLabel(600, 90, "Амплитуда")
	self.tab.oVolt = self.tab:NewSpinEdit(600, 110, 0.000, 5)
	self.tab.oVolt.value = 0.4
	self.tab.oVolt.step = 0.001
	self.tab.oVolt.digits = 3

end

--[[
  @brief
	Деинициализация - установка настроек приборов в безопасное состояние

	@example
    *table of voltmeter/source*:deinit()
]]--
function voltmeter:deinit()
	self.srs:setRange(1)
end

function source:deinit()
	self.srs:setVoltage(0)
end

return {voltmeter, source}