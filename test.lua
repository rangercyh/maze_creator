local maze = require "maze"
local t1 = os.clock()
-- if w h above 6 then search for full path slow down
local w, h, sx, sy = 4, 10, 0, 0
local l = 32
local ex_num, ex_limit = 11, 3

local UP = 1
local DOWN = 2
local LEFT  = 4
local RIGHT  = 8

local char = {
    [-1] = 'S',
    [UP] = '↑',
    [DOWN] = '↓',
    [LEFT] = '←',
    [RIGHT] = '→',
}

maze.set_rseed(123444)
function get_path_idx(t, x, y)
    for i = 1, t.num do
        if x == t[i][1] and y == t[i][2] then
            return t.num - i
        end
    end
end

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

function test(w, h, sx, sy, l, ex_num, ex_limit)
    local t = maze.create_maze({
        w = w,
        h = h,
        sx = sx,
        sy = sy,
        l = l,
        ex_num = ex_num,
        ex_limit = ex_limit,
    })
    if not(t) or (t.num and t.num <= 0) then
        print('no found path!')
        return
    end
    print('===== w =', t.w, 'h =', t.h, 'num =', t.num)
    w = t.w
    h = t.h
    -- { x, y, dir, mark }
    -- { 1, 1, [-1-3] [1,2] }
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
        end
        if t[i][3] == DOWN then
            add_show_char(t, t[i][1], t[i][2] - 1, char[DOWN])
        end
        if t[i][3] == LEFT then
            add_show_char(t, t[i][1] + 1, t[i][2], char[LEFT])
        end
        if t[i][3] == RIGHT then
            add_show_char(t, t[i][1] - 1, t[i][2], char[RIGHT])
        end
    end
    t[1].show = 'E'
    local s = ""
    for i = 0, h - 1 do
        for j = 0, w - 1 do
            local ch = get_show(t, j, i)
            -- local ch = get_path_idx(t, j, i)
            if ch then
                local a = 5 - utf8.len(ch)
                local p = ''
                while a > 0 do
                    p = p .. ' '
                    a = a - 1
                end
                s = s .. ch .. p
            else
                s = s .. '*    '
            end
        end
        s = s .. '\n'
    end
    return s
end
for i = 1, 1 do
    print(test(w, h, sx, sy, l, ex_num, ex_limit))
end

print('cost time:', os.clock() - t1, 'ms')
