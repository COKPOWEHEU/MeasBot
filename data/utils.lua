local utils = {}

--[[
  @brief
  Создание и инициализация массива

  @return table

  @example
    *variable* = utils:buffer()
]]--
function utils:buffer()
	local vector = {size = 0}
	local metavector = {data = {}}
	
	function metavector:__index(idx)
    local mt = getmetatable(self)
    local res = rawget(mt.data, idx)
    if res == nil then res = rawget(mt.data, 0) end
    return res
  end
  
  function metavector:__newindex(idx, val)
    local mt = getmetatable(self)
    local i = tonumber(idx)
    if i == nil then
      rawset(self, idx, val)
    else
      rawset(mt.data, i, val)
    end
  end

  function metavector:__len()
    return self.size
  end
	
	setmetatable(vector, metavector)
	
	function vector:add(val)
    local it = 1
    return function(val)
			self.size = it
      self[it] = val
      it = it + 1
      return it
    end
  end
  
  return vector	
end

--[[
  @brief
  Создание и инициализация кольцевого буффера с заданным размером

  @param size:number - размер буффера

  @return table

  @example
    *variable* = utils:circular_buff(20)
]]--
function utils:circular_buff(size)
  local vector = {size = size}
  local metavector = {data={}}

  function metavector:__index(idx)
    local mt = getmetatable(self)
    idx = idx%self.size
    local res = rawget(mt.data, idx)
    if res == nil then res = rawget(mt.data, 0) end
    return res
  end

  function metavector:_newindex(idx, val)
    local mt = getmetatable(self)
    local i = tonumber(idx)
    if i == nil then
      rawset(self, idx, val)
    else
      i = i % self.size
      rawset(mt.data, i, val)
    end
  end

  function metavector:__len()
    return self.size
  end

  setmetatable(vector, metavector)

  function vector:add(val)
    local it = 1
    return function(val)
      self[it] = val
      -- print(self[it], it)
      -- print(self[it][1])
      --print(self[it][2])
      it = (it)%self.size + 1
      return it
    end
  end

  return vector
end

--[[
  @brief
  Функция задержки с указанием времени в секундах

  @param n:number - кол-во секунд

  @example
    utils:sleep(1)
    или
    *variable* = require("utils").sleep
    *variable*(1)

]]--
function utils.sleep(n)
  local t0 = os.clock()
  while os.clock() - t0 <= n do end
end

return utils
