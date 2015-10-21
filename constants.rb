#!/usr/bin/env ruby
#
# Produce the EEPROM data file
#
magic=0x5aa5

battery_cal = 16.0
solar_cal = 20.0

battery_crit=0.0
battery_low=11.0
battery_high=14.5
solar_high=18.0

# Constants: Kp, Ki, Kd, Emul, Ediv, Esigma(0), Eprev(0), Umul, Udiv
wind_consts = [60, 4, 20, 1, 1, 0, 0, 1, 122]
compass_consts = [80, 300, 260, 1, 1, 0, 0, 1, 1]

eeprom = Array(23)
eeprom[0] = magic
eeprom[1] = ((battery_crit * 1024.0 / battery_cal) + 0.5).to_i
eeprom[2] = ((battery_low * 1024.0 / battery_cal) + 0.5).to_i
eeprom[3] = ((battery_high * 1024.0 / battery_cal) + 0.5).to_i
eeprom[4] = ((solar_high * 1024.0 / solar_cal) + 0.5).to_i

idx = 5

wind_consts.each do |val|
  eeprom[idx] = val
  idx += 1
end

compass_consts.each do |val|
  eeprom[idx] = val
  idx += 1
end

def dump(addr, data, len)
  return if len < 1
  str = ":%02X%04X00" % [len, addr]
  csum = len
  csum += (addr >> 8) & 0xff
  csum += addr & 0xff
  len.times do |i|
    str += "%02X" % data[i]
    csum += data[i]
  end
  str += "%02X" % (-csum & 0xff)
  puts str
end

idx = 0
addr = 0
data = Array(16)
eeprom.each do |val|
  data[idx] = val & 0xff;
  data[idx + 1] = (val >> 8) & 0xff
  idx += 2
  if idx > 15
    dump(addr, data, idx)
    addr += 16
    idx = 0
  end
end

dump(addr, data, idx)
puts ":00000001FF"
