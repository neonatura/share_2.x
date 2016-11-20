-- Some simple time examples supported by SEXE compiled bytecode.

local t = time()
print("Local Time: " .. ctime(t))

-- unix epoch time
print("Unix Timestamp: " .. utime(t))

-- time math example
local ONE_YEAR = 31536000
local ta = t + ONE_YEAR
print("Future Time: " .. strftime(ta, "%x %T"))

-- compatible with unix time-stamp conversion
local result = "FAIL"
if (utime(t) == utime(timeu(utime(t)))) then
  result = "OK"
end
print("Self-check: " .. result)
