-- standard checksum algorythms

local val = arg[1]

local crc = crc32(val)
local adler = adler32(val)

print("VALUE: " .. val)
print("CRC32: " .. crc)
print("ADLER32: " .. adler)

