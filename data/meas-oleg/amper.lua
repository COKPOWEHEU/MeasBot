local amper = {}

--[[
  @brief
	Подключение к приборам sr5105 и sr570
	
	@param sr5105_path:string, sr570_path:string - путь к терминалу, соединённого со стандартным вводом

	@return 1 - успешное подключение к приборам
	@return nil - неудача во время подключения к какому-то из прибору

  @example
    *table of amperemeter*:connect("/dev/ttyUSB0", "/dev/ttyACM0")
]]--
function amper:connect(sr5105_path, sr570_path)
	self.Usetup = require("sr5105"):connectNewDevice(sr5105_path)
	if self.Usetup == nil then return nil end

	self.Isetup = require("sr570"):connectNewDevice(sr570_path)
	if self.Isetup == nil then return nil end

	return 1
end

--[[
  @brief
	Вычисление тока, поступающего с образца на sr570
	
	@return Возвращает результат вычисления типа number

  @example
    *table of amperemeter*:getCurrent()
]]--
function amper:getCurrent()
	local sens = self.Isetup:getSens()
	local U = self.Usetup:getVoltage()
	self.U = U
	return U*sens
end

--[[
  @brief
  Инициализация приборов SR5105 и SR570

  @example
    *table of amperemeter*:init()
]]--
function amper:init()
	self.Usetup:setRange(tonumber(self.tab.vsens[self.tab.vsens.selected]))
  self.Usetup:setFilters(tonumber(self.tab.Hfilters[self.tab.Hfilters.selected]), tonumber(self.tab.Lfilters[self.tab.Lfilters.selected]))
  self.Usetup:setTimeConstant(tonumber(self.tab.TimeConstant[self.tab.TimeConstant.selected])*0.995)
	self.Usetup:setDynamicReserve(self.tab.DynReserve.selected-1)
	if self.tab.slope.selected == 1 then self.Usetup:setOutputSlope_6() else self.Usetup:setOutputSlope_12() end

	self.Isetup:setSens(self.tab.isens.value)
	self.Isetup:setTypeFilter(self.tab.TypeFilter.selected-1)
	self.Isetup:setHighFilter(self.tab.HighFilter.value)
	self.Isetup:setLowFilter(self.tab.LowFilter.value)
	self.Isetup:turnInOffsetCurr(false)
	self.Isetup:setCalOffsetCurrentLVL(2e-12)
	self.Isetup:setUncalInOffsetVernier(0)
	self.Isetup:setSenCal(false)
	self.Isetup:setInOffsetCurrSign(true)
	self.Isetup:setInOffsetCalMode(false)
	self.Isetup:turnBiasVolt(false)
	self.Isetup:setSigInvertSense(true)
	self.Isetup:setBlanksOutAmplifier(false)
	self.Isetup:setUncalSensVernier(0)
	self.Isetup:setBiasVoltLVL(0)
	self.Isetup:setGainMode(0)
end

--[[
  @brief
	Настройка элементов инициализации в окне/вкладке
	*Перед вызовом этой функции присвойте полю tab таблицы амперметра значение окна/вкладки

	@example
		*table of amperemeter*.tab = gui.window
		или
		*table of amperemeter*.tab = gui.window.tab[n]
    *table of amperemeter*:setup()
]]--
function amper:setup()

	self.tab.label_amp2 = self.tab:NewLabel(10,0,"Введите путь к sr570")
  self.tab.amp2_path = self.tab:NewEdit(10, 20, "/dev/ttyACM0")

	self.tab.label_isens = self.tab:NewLabel(10, 60, "Чувствительность")
	self.tab.isens = self.tab:NewSpinEdit(10, 80, 1e-12, 1e-3)
	self.tab.isens.value = 1e-3
	self.tab.isens.step = 1e-12
	self.tab.isens.digits = 12

	self.tab.label_TypeFilter = self.tab:NewLabel(10, 120, "Тип фильтра")
	self.tab.TypeFilter = self.tab:NewComboBox(10, 140)
	self.tab.TypeFilter:SetItems({
		"6 dB highpass", "12 dB highpass",
		"6 dB bandpass", "6 dB lowpass",
		"12 dB lowpass", "none"
	})
	self.tab.TypeFilter.selected = 3

	self.tab.label_HighFilter = self.tab:NewLabel(10, 180, "Фильтр высоких частот")
	self.tab.HighFilter = self.tab:NewSpinEdit(10, 200, 0.03, 1e4)
	self.tab.HighFilter.value = 0.3
	self.tab.HighFilter.step = 0.01
	self.tab.HighFilter.digits = 2

	self.tab.label_LowFilter = self.tab:NewLabel(10, 240, "Фильтр низких частот")
	self.tab.LowFilter = self.tab:NewSpinEdit(10, 260, 0.03, 1e6)
	self.tab.LowFilter.value = 10e+3
	self.tab.LowFilter.step = 0.01
	self.tab.LowFilter.digits = 2


	--sr5105
	self.tab.label_amp1 = self.tab:NewLabel(650,0,"Введите путь к sr5105")
	self.tab.amp1_path = self.tab:NewEdit(650, 20, "/dev/tty_pl2302_0")
	
	self.tab.label_vsens = self.tab:NewLabel(650, 60, "Чувствительность")
	self.tab.vsens = self.tab:NewComboBox(650, 80)
	self.tab.vsens:SetItems({
		10e-6, 31.6e-6,
    100e-6, 316e-6,
    1e-3, 3.16e-3,
    10e-3, 31.6e-3,
    100e-3, 316e-3,
    1e0
	})
	self.tab.vsens.selected = 11

	self.tab.label_filters = self.tab:NewLabel(650, 120, "Настройка фильтров (High и Low)")
	self.tab.Hfilters = self.tab:NewComboBox(660, 140)
  self.tab.Hfilters:SetItems({1, 10, 100, 1000})
  self.tab.Hfilters.selected = 1
	self.tab.Lfilters = self.tab:NewComboBox(760, 140)
  self.tab.Lfilters:SetItems({50, 500, 5000, 50000})
  self.tab.Lfilters.selected = 3

	self.tab.label_TimeConstant = self.tab:NewLabel(650, 190, "Time const")
	self.tab.TimeConstant = self.tab:NewComboBox(730, 180)
	self.tab.TimeConstant:SetItems({
		300e-6, 1e-3, 3e-3,
		10e-3, 30e-3, 100e-3,
		300e-3, 1, 3, 10
	})
	self.tab.TimeConstant.selected = 6

	self.tab.label_DynReserve = self.tab:NewLabel(650, 240, "DynReserve")
	self.tab.DynReserve = self.tab:NewComboBox(740, 230)
	self.tab.DynReserve:SetItems({"High Stability", "Normal", "High Reserve"})
	self.tab.DynReserve.selected = 2

	self.tab.label_slope = self.tab:NewLabel(650, 290, "Slope mode")
	self.tab.slope = self.tab:NewComboBox(740, 280)
	self.tab.slope:SetItems({"6 dB/oct", "12 dB/oct"})
	self.tab.slope.selected = 2
end

--[[
  @brief
	Деинициализация - установка настроек приборов в безопасное состояние

	@example
    *table of amperemeter*:deinit()
]]--
function amper:deinit() 
	self.Usetup:setRange(1)
	self.Isetup:setSens(0.001)
end

return amper