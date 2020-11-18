package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"
-- package.path = package.path .. ";/home/user/scripts/hall_oleg/?.lua"
package.path = package.path .. ";./data/?.lua"
package.path = package.path .. ";./data/meas-oleg/?.lua"

-- Debug functional

--[[
  @brief
  Вывод всех полей таблицы и их значений

  @param tbl:table
  @param [pref]:string, [postf]:string - префиксный и постфиксный символы (такие как  "(", "[", "{", "*", "\"" )

  @example
    tblprint(measure)
]]--
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

service = {}

--[[
  @brief
  Отрисовка окна service с возможностью получения отладочных данных

  @param tab объект окна/вкладки

  @example
    service:VisualTab(gui.window)
    service:VisualTab(gui.tab[n])
]]--
function service:VisualTab(tab)
  self.ui = {}
  self.ui.tab = tab
  self.ui.cmdline = tab:NewEdit(10,10,"print(123)")
  self.ui.cmdline.width = 500
  self.ui.run = tab:NewButton(20, 50, "Run")
  self.ui.run.ui = self.ui
  self.ui.cmdline.ui = self.ui
  self.ui.outstr = tab:NewLabel(10, 130, "")
  function self.ui.run:OnClick()
    local str = self.ui.cmdline.text
    local func, err = load(str)
    if func then
      print(">>> ", str)
      local resstr = ""
      local res = {pcall(func)}
      for i, v in pairs(res) do
        resstr = resstr .. "[ " .. tostring(i) .. " ] = " .. tostring(v) .. "\n"
      end
      self.ui.outstr.text = resstr
    else
      self.ui.outstr.text = "ERROR: " .. tostring(err)
    end
  end
  self.ui.cmdline.OnEnter = self.ui.run.OnClick
  
  self.ui.pausebtn = tab:NewButton(10, 90, "Pause")
  self.ui.pausebtn.ui = self.ui
  function self.ui.pausebtn:OnClickFunc()
    self.ui.pausebtn.OnClick = nil
    while not self.ui.pausebtn:WasClicked() do
      gui:update()
    end
    self.ui.pausebtn.OnClick = self.ui.pausebtn.OnClickFunc
  end
  self.ui.pausebtn.OnClick = self.ui.pausebtn.OnClickFunc
end

-- GUI

-- Создание основного окна измерения
gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(950, 650, "Measure_1")
gui.wnd.tabs = gui.wnd:NewTabControl(0, 0, 900, 650);
gui.wnd.tabs[0] = "Main Plot"
gui.wnd.tabs[1] = "Additional Plots"
gui.wnd.tabs[2] = "Setting"
gui.wnd.tabs[3] = "Voltmeter"
gui.wnd.tabs[4] = "Amperemeter"
gui.wnd.tabs[5] = "Magnet"

-- Создание окна для отладки
gui.srvwnd = gui:NewWindow(800, 300, "Service")

gui:SetMainWindow(gui.wnd)
service:VisualTab(gui.srvwnd)


-- INIT

-- Таблица измерения и её поля
measure = {}
measure.count = 0
measure.step = 0
measure.max_field = 0
measure.enabled = 0
measure.maintab = gui.wnd.tabs[0]
measure.secplot = gui.wnd.tabs[1]
measure.setuptab = gui.wnd.tabs[2]

-- Для графика sr5105
measure.res_sr5105 = {count=0}
function measure.res_sr5105:add(value)
  self.count = self.count + 1
  self[ self.count ] = value
end

-- Для графика sr830
measure.res_sr830 = {count=0}
function measure.res_sr830:add(value)
  self.count = self.count + 1
  self[ self.count ] = value
end

-- Для графика lps305
measure.res_lps305 = {count=0}
function measure.res_lps305:add(value)
  self.count = self.count + 1
  self[ self.count ] = value
end

-- Инициализация таблиц устройств
-- {

amperemeter = require("amper")
amperemeter.tab = gui.wnd.tabs[4]

magnet = require("magneto")
magnet.tab = gui.wnd.tabs[5]

voltmeter, sourceOfSample = table.unpack(require("volt"))
voltmeter.tab = gui.wnd.tabs[3]
sourceOfSample.tab = gui.wnd.tabs[3]

-- }

-- Хранилище данных, получаемых с устройств
vault = require("vault")

--[[
  @brief
	Настройка элементов инициализации в окне/вкладке
	*Перед вызовом этой функции присвойте полям - mainplot, secplot, setuptab таблицы амперметра значения окна/вкладки

	@example
		measure.maintab = gui.wnd0
    measure.secplot = gui.wnd1
    measure.setuptab = gui.wnd2

    или
    
		measure.maintab = gui.wnd.tabs[0]
    measure.secplot = gui.wnd.tabs[1]
    measure.setuptab = gui.wnd.tabs[2]

    measure.setup()
]]--
function measure.setup()
  --инициализация основного графика
  measure.maintab.plot = measure.maintab:NewPlot(20,20,700, 600)
  measure.maintab.plot.format={x=1,4}
  measure.maintab.plot.data = vault
  measure.maintab.btn_start = measure.maintab:NewButton(740,20,"Start")
  measure.maintab.btn_stop = measure.maintab:NewButton(800,20,"Stop")

  -- отладочная информация
  measure.maintab.sr5105_label = measure.maintab:NewLabel(740, 60, "Volt of 5105")
  measure.maintab.label_scale = measure.maintab:NewLabel(740, 100, "Overload of 5105(%)")
  measure.maintab.scale = measure.maintab:NewProgressBar(740, 140, 100, 20)
  measure.maintab.scale.max = 100
  
  measure.maintab.pscale = measure.maintab:NewProgressBar(740, 180, 100, 20)
  measure.maintab.max = 10

  measure.maintab.timer = measure.maintab:NewLabel(740, 220, "Timer")

  -- Дополнительные графики
  measure.secplot.sr5105_plot = measure.secplot:NewPlot(10, 10, 400, 300)
  measure.secplot.sr5105_plot.format = {1}
  measure.secplot.sr5105_plot.data = measure.res_sr5105

  measure.secplot.sr830_plot = measure.secplot:NewPlot(420, 10, 400, 300)
  measure.secplot.sr830_plot.format = {1}
  measure.secplot.sr830_plot.data = measure.res_sr830

  measure.secplot.lps305_plot = measure.secplot:NewPlot(10, 315, 400, 300)
  measure.secplot.lps305_plot.format = {1}
  measure.secplot.lps305_plot.data = measure.res_lps305

  -- установка кол-ва точек на графике
  measure.setuptab.label_pt = measure.setuptab:NewLabel(10, 30, "Введите кол-во точек:")
  measure.setuptab.points = measure.setuptab:NewSpinEdit(10, 50, 0, 1000)

  -- установка максимального значения магнитного поля
  measure.setuptab.label_max = measure.setuptab:NewLabel(10, 90, "Введите максимальное\nзначение магнитного поля")
  measure.setuptab.max_field = measure.setuptab:NewSpinEdit(10, 125, -10, 10)

  -- работа с результатами и конфигурационным файлом
  measure.setuptab.save_chbox = measure.setuptab:NewCheckBox(5, 160, "Сохранить результат")
  measure.setuptab.label_save = measure.setuptab:NewLabel(250, 10, "Имя файла")
  measure.setuptab.save_name = measure.setuptab:NewEdit(250, 30, "result.txt")

  measure.setuptab.comment_label = measure.setuptab:NewLabel(250, 70, "Комментарий")
  measure.setuptab.comment = measure.setuptab:NewEdit(250, 90, "Comment")
  
  measure.setuptab.format_label = measure.setuptab:NewLabel(250, 130, "Формат вывода")
  measure.setuptab.Format = measure.setuptab:NewEdit(250, 150, " B | U | I | R | U5105 ")

  measure.setuptab.save_res = measure.setuptab:NewButton(250, 190, "Сохранить результат")
  measure.setuptab.save_config = measure.setuptab:NewButton(450, 30, "Сохранить конфигурацию")
  measure.setuptab.load_config = measure.setuptab:NewButton(450, 70, "Загрузить конфигурацию")
  measure.setuptab.restore_config = measure.setuptab:NewButton(450, 110, "Восстановить конфигурацию")

  -- считывание значений из конфигурационного файла
  measure.startConfig()
end

--[[
  @brief
	Считывание начальных значений из конфига

  @param [filename]:string

  @return 53 - код ошибки, возникает при попытке открыть не существующий файл

  @example
    measure.startConfig() - будет использоваться конфиг по умолчанию

    или
    
    measure.startConfig("config_file.txt")
]]--
function measure.startConfig(filename)
  if filename == nil then 
    filename  = "data/.current_settings.txt"
  end
  local config = io.open(filename, "r")
  if config == nil then return 53 end

  -- загрузка основных настроек эксперимента
  local readData = config:read("*l")
  measure.setuptab.points.value = tonumber(readData)
  readData = config:read("*l")
  measure.setuptab.points.step = tonumber(readData)
  readData = config:read("*l")
  measure.setuptab.points.digits = tonumber(readData)

  readData = config:read("*l")
  measure.setuptab.max_field.val = tonumber(readData)
  readData = config:read("*l")
  measure.setuptab.max_field.step = tonumber(readData)
  readData = config:read("*l")
  measure.setuptab.max_field.digits = tonumber(readData)

  readData = config:read("*l")
  magnet.tab.mag_path.text = readData
  readData = config:read("*l")
  voltmeter.tab.volt_path.text = readData
  readData = config:read("*l")
  amperemeter.tab.amp1_path.text = readData
  readData = config:read("*l")
  amperemeter.tab.amp2_path.text = readData

  readData = config:read("*l")
  if readData == "true" then
    measure.setuptab.save_chbox.checked = true
  else
    measure.setuptab.save_chbox.checked = false
  end

  readData = config:read("*l")
  measure.setuptab.save_name.text = readData

  readData = config:read("*l")
  measure.setuptab.comment.text = readData

  readData = config:read("*l")
  measure.setuptab.Format.text = readData

  -- загрузка данных для вольтметра и источника питания для образца(sourceOfSample)
  readData = config:read("*l")
  if readData == "true" then
    voltmeter.tab.inGND.checked = true
  else
    voltmeter.tab.inGND.checked = false
  end
  
  readData = config:read("*l")
  if readData == "true" then
    voltmeter.tab.inCap.checked = true
  else
    voltmeter.tab.inCap.checked = false
  end

  readData = config:read("*l")
  voltmeter.tab.inRange.value = tonumber(readData)

  readData = config:read("*l")
  voltmeter.tab.timeconst.value = tonumber(readData)

  readData = config:read("*l")
  voltmeter.tab.slope.value = tonumber(readData)

  readData = config:read("*l")
  voltmeter.tab.input.selected = tonumber(readData)

  readData = config:read("*l")
  sourceOfSample.tab.oFreq.value = tonumber(readData)

  readData = config:read("*l")
  sourceOfSample.tab.oVolt.value = tonumber(readData)

  -- загрузка данных для амперметра
  readData = config:read("*l")
  amperemeter.tab.isens.value = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.TypeFilter.selected = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.HighFilter.value = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.LowFilter.value = tonumber(readData)
  
  readData = config:read("*l")
  amperemeter.tab.vsens.selected = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.Hfilters.selected = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.Lfilters.selected = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.TimeConstant.selected = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.DynReserve.selected = tonumber(readData)

  readData = config:read("*l")
  amperemeter.tab.slope.selected = tonumber(readData)

  -- load data for magnet
  readData = config:read("*l")
  magnet.tab.delay.value = tonumber(readData)

  config:close()
end

--[[
  @brief
	Сохранение текущих значений на GUI в конфиг-файл

  @param [filename]:string

  @return 53 - код ошибки, возникает при попытке открыть не существующий файл

  @example
    measure.saveConfig() - будет использоваться конфиг по умолчанию

    или
    
    measure.saveConfig("config_file.txt")
]]--
function measure.saveConfig(filename)
  if filename == nil then 
    filename  = "data/.current_settings.txt"
  end
  local config = io.open(filename, "w+")
  if config == nil then return 53 end
  config:write(measure.setuptab.points.value .. '\n')
  config:write(measure.setuptab.points.step .. '\n')
  config:write(measure.setuptab.points.digits .. '\n')
  config:write(measure.setuptab.max_field.val .. '\n')
  config:write(measure.setuptab.max_field.step .. '\n')
  config:write(measure.setuptab.max_field.digits .. '\n')
  config:write(magnet.tab.mag_path.text .. '\n')
  config:write(voltmeter.tab.volt_path.text .. '\n')
  config:write(amperemeter.tab.amp1_path.text .. '\n')
  config:write(amperemeter.tab.amp2_path.text .. '\n')
  config:write(tostring(measure.setuptab.save_chbox.checked) .. '\n')
  config:write(measure.setuptab.save_name.text .. '\n')
  config:write(tostring(voltmeter.tab.inGND.checked) .. '\n')
  config:write(tostring(voltmeter.tab.inCap.checked) .. '\n')
  config:write(tostring(voltmeter.tab.inRange.value) .. '\n')
  config:write(tostring(voltmeter.tab.timeconst.value) .. '\n')
  config:write(tostring(voltmeter.tab.slope.value) .. '\n')
  config:write(tostring(voltmeter.tab.input.selected) .. '\n')
  config:write(tostring(sourceOfSample.tab.oFreq.value) .. '\n')
  config:write(tostring(sourceOfSample.tab.oVolt.value) .. '\n')
  config:write(tostring(amperemeter.tab.isens.value) .. '\n')
  config:write(tostring(amperemeter.tab.TypeFilter.selected) .. '\n')
  config:write(tostring(amperemeter.tab.HighFilter.value) .. '\n')
  config:write(tostring(amperemeter.tab.LowFilter.value) .. '\n')
  config:write(tostring(amperemeter.tab.vsens.selected) .. '\n')
  config:write(tostring(amperemeter.tab.Hfilters.selected) .. '\n')
  config:write(tostring(amperemeter.tab.Lfilters.selected) .. '\n')
  config:write(tostring(amperemeter.tab.TimeConstant.selected) .. '\n')
  config:write(tostring(amperemeter.tab.DynReserve.selected) .. '\n')
  config:write(tostring(amperemeter.tab.slope.selected) .. '\n')
  config:write(tostring(magnet.tab.delay.value) .. '\n')
  config:flush()
  config:close()
end

--[[
  @brief
	Загрузка данных из выбранного конфиг-файла

  @example
    measure.loadConfig()
]]--
function measure.loadConfig()
  local filename = gui.wnd:OpenFileDialog("Выберите конфиг-файл")
  if filename ~= nil then
    measure.startConfig(filename)
  end
end

--[[
  @brief
	Восстановление данных на gui загрузив из конфиг-файла по умолчанию

  @example
    measure.restoreConfig()
]]--
function measure.restoreConfig()
  measure.startConfig()
end

--[[
  @brief
  Сохранение данных с приборов при определённом магнитном поле
  
  @param field_in:number - значение для изменения магнитного поля

  @example
    measure.addPoint(0.003)
]]--
function measure.addPoint(field_in)
  magnet:setField(field_in)
  local field_out = magnet:getField()
  local volt = voltmeter.srs:getVoltage()
  local curr = amperemeter:getCurrent()
  local hall = volt / curr

  print(field_out, volt, curr, hall, amperemeter.U)
  vault.push_back({field_out, volt, curr, hall, amperemeter.U})

  -- изменение значений отладки на основной вкладке
  measure.maintab.sr5105_label.text = tostring(amperemeter.U)
  local perc = amperemeter.Usetup:getScalePercentage()
  measure.maintab.label_scale.text =  tostring(perc)
  measure.maintab.scale.val = perc
  measure.maintab.pscale.val = measure.maintab.pscale.val + 1
  measure.res_lps305:add(magnet.source.both:getVoltage())
  measure.res_sr830:add(volt)
  measure.res_sr5105:add(amperemeter.U)
  measure.endtime = measure.endtime - 1.5
  measure.maintab.timer.text = tostring(measure.endtime)
end

-- MAIN

-- init gui of measure and devices
voltmeter:setup()
sourceOfSample:setup()
amperemeter:setup()
magnet:setup()
measure.setup()
--[[
  @brief
  Callback функция работающая по кнопке относящейся к полю restore_config
  Восстановление данных на gui загрузив из конфиг-файла по умолчанию
]]--
function measure.setuptab.restore_config:OnClick()
  measure.restoreConfig()
end

--[[
  @brief
  Callback функция работающая по кнопке относящейся к полю load_config
  Загрузка данных из выбранного конфиг-файла
]]--
function measure.setuptab.load_config:OnClick()
  measure.loadConfig()
end

--[[
  @brief
  Callback функция работающая по кнопке относящейся к полю save_config
  Сохранение текущих значений на GUI в конфиг-файл по умолчанию
]]--
function measure.setuptab.save_config:OnClick()
  measure.saveConfig()
end

--[[
  @brief
  Callback функция работающая по кнопке относящейся к полю save_res
  Сохранение текущих результатов в выбранный файл
]]--
function measure.setuptab.save_res:OnClick()
  local filename = gui.wnd:SaveFileDialog()
  if filename == nil then return end
  somefile = io.open(tostring(filename), "w")
  vault:saveData(somefile)
end

-- инициализация и начало измерений
while gui:update() do
  --нажатие кнопки начала измерения
  if measure.maintab.btn_start:WasClicked() then
    --настройка параметров измерения
    measure.count = measure.setuptab.points.val
    measure.max_field = measure.setuptab.max_field.val
    measure.step = measure.max_field/measure.count
    measure.maintab.pscale.max = measure.count * 4
    measure.endtime = (measure.count * 4) * 1.5
    measure.maintab.timer.text = tostring(measure.endtime)

    -- подключение к магниту
    if measure.enabled == 0 or measure.enabled == 1 then
      if magnet:connect(magnet.tab.mag_path.text) == nil then
        gui.wnd:MessageDialog("Wrong name of lps305 path")
        measure.enabled = bit32.lshift(1, 0)
      else
        measure.enabled = 0
      end
    end

    -- подключение к вольтметру
    if measure.enabled == 0 or measure.enabled == 2 then
      if voltmeter:connect(voltmeter.tab.volt_path.text) == nil then 
        gui.wnd:MessageDialog("Wrong name of voltmeter path")
        measure.enabled = bit32.lshift(1, 1)
      else
        measure.enabled = 0
      end
    end

    -- подключение к амперметру
    if measure.enabled == 0 or measure.enabled == 4 then
      if amperemeter:connect(amperemeter.tab.amp1_path.text, amperemeter.tab.amp2_path.text) == nil then
        gui.wnd:MessageDialog("Wrong name of amperemeter path")
        measure.enabled = bit32.lshift(1, 2)
      else
        measure.enabled = 0
      end
    end

    print(measure.enabled)
    -- инициализация приборов
    if measure.enabled == 0 then
      amperemeter:init()
      voltmeter:init()
      sourceOfSample:init()
      break
    end
  end
end

-- Записываем шапку в файл для результатов
measure.resfile = io.open(tostring(measure.setuptab.save_name.text), "w")
measure.resfile:write("Umax = 8.000000  " .. "Uout = " .. tostring(sourceOfSample.tab.oVolt.value) .. '\n')
measure.resfile:write(tostring(measure.setuptab.comment.text) .. '\n')
measure.resfile:write(tostring('['.. measure.setuptab.Format.text ..']') .. '\n')
measure.resfile:close()

-- Начало измерения 

field_in = 0
measure.addPoint(field_in)
--первый замер от 0 Тл к 0.1 Тл
while gui:update() do
  if field_in < 0.01 then field_in = field_in + (measure.step / 2) -- изменить
  else field_in = field_in + measure.step end
  if field_in >= measure.max_field then break end
  if(measure.maintab.btn_stop:WasClicked()) then break end

  measure.addPoint(field_in)

  measure.maintab.plot:Refresh()
end

--второй замер от 0.1 Тл до -0.1 Тл
while gui:update() do
  if field_in <= 0.01 and field_in >= (-0.01) then field_in = field_in - (measure.step / 2) -- изменить
  else field_in = field_in - measure.step end
  if field_in <= -(measure.max_field) then break end
  if(measure.maintab.btn_stop:WasClicked()) then break end

  measure.addPoint(field_in)

  measure.maintab.plot:Refresh()
end

--третий замер от -0.1 Тл до 0 Тл
while gui:update() do
  field_in = field_in + measure.step
  if field_in >= 0 then break end
  if(measure.maintab.btn_stop:WasClicked()) then break end

  measure.addPoint(field_in)
  
  measure.maintab.plot:Refresh()
end

-- Демонстрация окончания измерения
measure.maintab.end_label = measure.maintab:NewLabel(740, 140, "END!!!")

-- Сохранение результатов
if measure.setuptab.save_chbox.checked then
  measure.resfile = io.open(tostring(measure.setuptab.save_name.text), "a")
  vault:saveData(measure.resfile)
end

magnet:deinit()
sourceOfSample:deinit()
amperemeter:deinit()
voltmeter:deinit()

while gui:update() do end

