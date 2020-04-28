local maze = require "maze"
local Json = require "json"

local tointeger = math.tointeger

local w = tointeger(arg[1])
local h = tointeger(arg[2])
local l = tointeger(arg[3])
local x = tointeger(arg[4])
local y = tointeger(arg[5])
local ex_num = tointeger(arg[6])
local ex_limit = tointeger(arg[7])

local options = {
    pretty = false,
    align_keys = false,
    array_newline = false,
    null = 'null',
}

local r = {}

maze.set_rseed(tonumber(tostring(os.time()):reverse():sub(1,6)))
local t1 = os.clock()
local t = maze.create_maze({
    w = w,
    h = h,
    l = l,
    sx = x,
    sy = y,
    ex_num = ex_num,
    ex_limit = ex_limit,
})

if not(t) or (t.num and t.num <= 0) then
    r.errmsg = "没找到路径，联系蔡毅恒！"
    return
end

r.w = t.w
r.h = t.h
r.path = {}
r.ex = {}
r.time = os.clock() - t1

local char = {
    [-1] = 'S',
    [0] = '↑',
    [1] = '↓',
    [2] = '←',
    [3] = '→',
}
function get_show(t, x, y)
    for i = 1, t.num do
        if x == t[i][1] and y == t[i][2] then
            return t[i].show
        end
    end
end

function add_show_char(t, x, y, ch)
    for i = 1, t.num do
        if x == t[i][1] and y == t[i][2] then
            if not(t[i].show) then
                t[i].show = ch
            else
                t[i].show = t[i].show .. ch
            end
        end
    end
end

for i = 1, t.num do
    if t[i][4] == 1 then
        add_show_char(t, t[i][1], t[i][2], 'P')
    end
    if t[i][4] == 2 then
        add_show_char(t, t[i][1], t[i][2], 'X')
    end
    if t[i][3] == -1 then
        add_show_char(t, t[i][1], t[i][2], char[-1])
    end
end
for i = 1, t.num do
    if t[i][3] == 0 then
        add_show_char(t, t[i][1], t[i][2] + 1, char[0])
    end
    if t[i][3] == 1 then
        add_show_char(t, t[i][1], t[i][2] - 1, char[1])
    end
    if t[i][3] == 2 then
        add_show_char(t, t[i][1] + 1, t[i][2], char[2])
    end
    if t[i][3] == 3 then
        add_show_char(t, t[i][1] - 1, t[i][2], char[3])
    end
end
t[1].show = 'E'

for i = 1, t.num do
    if t[i][4] == 1 then
        table.insert(r.path, { t[i][1] + t[i][2] * t.w, t[i].show } )
    else
        table.insert(r.ex, { t[i][1] + t[i][2] * t.w, t[i].show } )
    end
end
r.path_num = #r.path
r.ex_num = #r.ex

print(Json:encode_pretty(r, nil, options))
