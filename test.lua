local maze = require "maze"
local t1 = os.clock()
local w, h, sx, sy = 50, 50, 0, 0
local l = 158

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
    -- local s = ""
    -- for i = 0, h - 1 do
    --     for j = 0, w - 1 do
    --         local idx = get_path_idx(t, j, i)
    --         if idx then
    --             local a = 4 - utf8.len(idx)
    --             local p = ''
    --             while a > 0 do
    --                 p = p .. ' '
    --                 a = a - 1
    --             end
    --             s = s .. idx .. p
    --         else
    --             s = s .. '*   '
    --         end
    --     end
    --     s = s .. '\n'
    -- end
    -- return s
end
for i = 1, 1 do
    test(w, h, sx, sy, l, i)
end

print('cost time:', os.clock() - t1, 'ms')
