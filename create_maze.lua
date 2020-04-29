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
    r.errmsg = "没找到路径!"
    return
end

r.w = t.w
r.h = t.h
r.path = {}
r.ex = {}
r.time = os.clock() - t1

local UP = 1
local DOWN = 2
local LEFT  = 4
local RIGHT  = 8

-- local dir = {
--     1, -- UP
--     2, -- DOWN
--     4, -- LEFT
--     8, -- RIGHT
--     3, -- UP DOWN
--     5, -- UP LEFT
--     9, -- UP RIGHT
--     6, -- DOWN LEFT
--     10, -- DOWN RIGHT
--     12, -- LEFT RIGHT
--     7, -- UP DOWN LEFT
--     11, -- UP DOWN RIGHT
--     13, -- UP LEFT RIGHT
--     14, -- DOWN LEFT RIGHT
--     15, -- UP DOWN LEFT RIGHT
-- }

local char = {
    [-1] = 'S',
    [UP] = '↑',
    [DOWN] = '↓',
    [LEFT] = '←',
    [RIGHT] = '→',
}

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

function sum_dir(t, x, y, v)
    for i = 1, t.num do
        if x == t[i][1] and y == t[i][2] then
            if not(t[i].sum) then
                t[i].sum = v
            else
                t[i].sum = t[i].sum + v
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
    if t[i][3] == UP then
        add_show_char(t, t[i][1], t[i][2] + 1, char[UP])
        add_show_char(t, t[i][1], t[i][2], char[DOWN])
        sum_dir(t, t[i][1], t[i][2] + 1, UP)
        sum_dir(t, t[i][1], t[i][2], DOWN)
    end
    if t[i][3] == DOWN then
        add_show_char(t, t[i][1], t[i][2] - 1, char[DOWN])
        add_show_char(t, t[i][1], t[i][2], char[UP])
        sum_dir(t, t[i][1], t[i][2] - 1, DOWN)
        sum_dir(t, t[i][1], t[i][2], UP)
    end
    if t[i][3] == LEFT then
        add_show_char(t, t[i][1] + 1, t[i][2], char[LEFT])
        add_show_char(t, t[i][1], t[i][2], char[RIGHT])
        sum_dir(t, t[i][1] + 1, t[i][2], LEFT)
        sum_dir(t, t[i][1], t[i][2], RIGHT)
    end
    if t[i][3] == RIGHT then
        add_show_char(t, t[i][1] - 1, t[i][2], char[RIGHT])
        add_show_char(t, t[i][1], t[i][2], char[LEFT])
        sum_dir(t, t[i][1] - 1, t[i][2], RIGHT)
        sum_dir(t, t[i][1], t[i][2], LEFT)
    end
end
add_show_char(t, t[1][1], t[1][2], 'E')

for i = 1, t.num do
    if t[i][4] == 1 then
        table.insert(r.path, { t[i][1] + t[i][2] * t.w, t[i].show, t[i].sum } )
    else
        table.insert(r.ex, { t[i][1] + t[i][2] * t.w, t[i].show, t[i].sum } )
    end
end
r.path_num = #r.path
r.ex_num = #r.ex

print(Json:encode_pretty(r, nil, options))
