-- standard checksum algorythms

local val = arg[1]

if (val == nil) then
  print("usage: hash.sx <text>")
  return
end

local crc = crc32(val)
local adler = adler32(val)

print("VALUE: " .. val)
print("CRC32: " .. crc)
print("ADLER32: " .. adler)

