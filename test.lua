local maze = require "maze"
local t1 = os.clock()
local w, h, sx, sy = 10, 10, 4, 1
local l = 20

maze.set_rseed(123444)
function get_path_idx(t, x, y)
    for i = #t, 1, -1 do
        if x == t[i][1] and y == t[i][2] then
            return #t - i
        end
    end
end

function test(w, h, sx, sy, l, i)
    local t = maze.create_maze({
        w = w,
        h = h,
        sx = sx,
        sy = sy,
        l = l,
    })
    if not(t) or not(#t > 0) then
        print('no found path!')
        return
    end
    local s = ""
    for i = 0, h - 1 do
        for j = 0, w - 1 do
            local idx = get_path_idx(t, i, j)
            if idx then
                s = s .. idx .. ' '
            else
                s = s .. '* '
            end
        end
        s = s .. '\n'
    end
    return s
end
for i = 1, 100 do
    print(test(w, h, sx, sy, l, i))
end

print('cost time:', os.clock() - t1, 'ms')
