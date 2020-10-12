package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.so"
package.cpath = package.cpath .. ";modules/lib/lib\\dyn/?.dll"

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

pwr_supl = {}
vmeter = {}
ameter = {}

function pwr_supl:VisualTab(tab)
  self.ui={}
  self.ui.tab = tab
  self.ui.port = tab:NewEdit(100, 10, "/dev/ttyUSB0")
  self.ui.port.label = tab:NewLabel(10, 10, "SR830 port")
  
  self.ui.volt = tab:NewSpinEdit(100, 90, 0.004, 5, 3)
  self.ui.volt.value = 0.004
  self.ui.volt.label = tab:NewLabel(0, 90, "Voltage (V)")
  
  self.ui.freq = tab:NewSpinEdit(100, 130, 1, 10000)
  self.ui.freq.value = 133
  self.ui.freq.label = tab:NewLabel(0, 130, "Frequency (Hz)")
end
function pwr_supl:connect()
  if self.srs == nil then
    if vmeter.srs ~= nil then 
      self.srs = vmeter.srs
    else
      self.srs = require("sr830"):connectNewDevice(self.ui.port.text)
    end
  end
  if self.srs == nil then return "SR830\n" end
  
  self.dev = self.srs.SineOut
  self.dev:setFreq(self.ui.freq.value)
  self.dev:setVoltage(self.ui.volt.value)
  return ""
end


function vmeter:VisualTab(tab)
  self.ui={}
  self.ui.tab = tab
  self.ui.port = tab:NewEdit(100, 10, "/dev/ttyUSB0")
  self.ui.port.label = tab:NewLabel(0, 10, "SR830 port")
  
  self.ui.timeconst = tab:NewSpinEdit(100, 50, 0, 10, 6)
  self.ui.timeconst.value = 100e-3
  self.ui.timeconst.label = tab:NewLabel(0, 50, "Time Const (s)")
  
  self.ui.sens = tab:NewEdit(100, 90, "1e-3")
  self.ui.sens.label = tab:NewLabel(0, 90, "Sensetivity (V)")
  
  self.ui.input = tab:NewComboBox(100, 130)
  self.ui.input:SetItems({"A", "A-B", "Curr 1 MOhm", "Curr 100 MOhm"})
  self.ui.input.label = tab:NewLabel(0, 130, "Input")
  
  self.ui.float = tab:NewCheckBox(100, 170, "Floating input")
  
  self.ui.acmode = tab:NewCheckBox(100, 210, "AC input mode")
  
  self.ui.slope = tab:NewComboBox(100, 250)
  self.ui.slope:SetItems({"6 dB/oct", "12 dB/oct", "18 dB/oct", "24 dB/oct"})
  self.ui.slope.selected = 2
end
function vmeter:connect()
  local sens = tonumber(self.ui.sens.text)
  if sens == nil then return self.ui.tab.text .. " sensitivity: uncorrect value\n" end
  if self.srs == nil then
    if pwr_supl.srs ~= nil then 
      self.srs = pwr_supl.srs
    else
      self.srs = require("sr830"):connectNewDevice(self.ui.port.text)
    end
  end
  if self.srs == nil then return "SR830\n" end
  
  self.dev = self.srs.SineIn
  self.dev:setInputMode(self.ui.input.selected)
  self.dev:setInputGrounded(not self.ui.float)
  self.dev:setInputCap(self.ui.acmode)
  self.dev:setRange(sens)
  self.dev:setTimeConst(self.ui.timeconst.value)
  self.dev:setSlope(self.ui.slope.selected)
  return ""
end


function ameter:VisualTab(tab)
  self.ui={}
  self.ui.tab = tab
  self.ui.iport = tab:NewEdit(120, 10, "/dev/ttyUSB1")
  self.ui.iport.label = tab:NewLabel(0, 10, "SR570 port")
  
  self.ui.isens = tab:NewEdit(120, 50, "1e-3")
  self.ui.isens.label = tab:NewLabel(0, 50, "Sensitivity (A/V)")
  
  
  self.ui.vport = tab:NewEdit(400, 10, "/dev/ttyUSB2")
  self.ui.vport.label = tab:NewLabel(310, 10, "SR5105 port")
end
function ameter:connect()
  return ""
end

gui = require("mygui_gtk")
gui.wnd = gui:NewWindow(800, 600, "Test gtk")
gui.wnd.tabs = gui.wnd:NewTabControl(0, 0, 800, 500)
btnOk = gui.wnd:NewButton(100, 550, "OK")
btnClose = gui.wnd:NewButton(300, 550, "Close")
gui.wnd.tabs[1] = "Measure"
gui.wnd.tabs[2] = "PowerSupl"
pwr_supl:VisualTab(gui.wnd.tabs[2])
gui.wnd.tabs[3] = "Voltmeter"
vmeter:VisualTab(gui.wnd.tabs[3])
gui.wnd.tabs[4] = "Ampermeter"
ameter:VisualTab(gui.wnd.tabs[4])

-- Device connection stage
while gui:update() do
  if btnOk:WasClicked() then
    local conn = ""
    conn = conn .. pwr_supl:connect()
    conn = conn .. vmeter:connect()
    conn = conn .. ameter:connect()
    if conn == "" then break end
    print("Connection failed:\n".. conn)
  end
  if btnClose:WasClicked() then goto PROG_END end
  if not gui:update() then goto PROG_END end
end

print("Connected!")

-- Measure stage

::PROG_END::
print("END")
