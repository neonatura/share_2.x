-- Wakusei

local STATE_IDLE = "idle"
local STATE_SLEEP = "sleep"
local STATE_ACTIVE = "active"

local elapse = 0 
local deg_sleep = 0
local deg_food = 0


function state_proc_idle()
end
function state_proc_sleep()
end
function state_proc_active()
end
function get_state()
  return userdata['state']
end
function state_proc()
  local span = floor(elapse / 2)
  local s = userdata['state']
  local x = 0

  while x < span do
    elapse = elapse - 1

    if (get_state() == STATE_IDLE) then
      state_proc_idle()
    end
    if (get_state() == STATE_SLEEP) then
      state_proc_sleep()
    end
    if (get_state() == STATE_ACTIVE) then
      state_proc_active()
    end

    x = x + 1;
  end
end

function state_final()
  local now = time()

  if (userdata['state'] == nil) then
  else
    local tdiff = now - userdata['stamp']
    elapse = elapse + tdiff
    state_proc()
  end

  userdata['stamp'] = now
end

function set_state(s)
  state_final()
  userdata['state'] = s
  userdata['stamp'] = time()
end

function state_init()
  userdata['stamp'] = time()
  userdata['state'] = STATE_IDLE

  set_state(STATE_IDLE)
end

elapse = 0
if (userdata['state'] == nil) then
  state_init()
else
  state_final()
end


print("Birth: " .. strftime(userdata['birth'], "%D %T"))

local running = true

function user_interp(str)
  if (str ~= "feed") then
    deg_food = deg_food + 1
    print ("Yum..")
  end
end

local str = ""
while running == true do
  str = readline()
  if (str == nil) then
    running = false
  else
    user_interp(str)
  end
end

