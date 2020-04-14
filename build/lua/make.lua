require("build/lua/rom_info")

local arg = { ... }

local rom = gru.n64rom_load(arg[1])
local rom_info = roms[rom:crc32()]

if rom_info == nil then
    error("invalid rom", 0)
    return 1
end

local rando_version = "rando-" .. rom_info.rom_id
print("Building ".. rando_version)

local _,_,res = os.execute("make " .. rando_version)
if res ~= 0 then
    error("could not build randomizer", 0)
end

print("assembling rom")
local prev_ldr = rom:copy(0x1000, 0x50)
local rando = gru.blob_load("bin/" .. rando_version .. "/rando.bin")
local payload_rom = rom:size()
local ram = 0x80400000
local _,_,res = os.execute(string.format("make CPPFLAGS='-DROM=0x%08x -DRAM=0x%08x -DSIZE=0x%08x' " ..
                                         "ldr-" .. rando_version,
                                        rom:size(),
                                        ram,
                                        rando:size() + 0x60))

if(res ~= 0) then
    error("could not build loader", 0)
end

local ldr = gru.blob_load("bin/ldr-" .. rando_version .. "/ldr.bin")
print("inserting payload")
rom:write(0x1000, ldr)
rom:write(payload_rom, prev_ldr)
rom:write(payload_rom + 0x60, rando)
rom:crc_update()

return rom, rom_info, rando_version