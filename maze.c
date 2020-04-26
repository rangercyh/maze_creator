#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))

typedef struct link_node {
    int x;
    int y;
    unsigned char m[0];
} PATH_NODE;

typedef struct path_stack {
    int capacity;
    int top;
    PATH_NODE *arr[0];
} PATH_STACK;

static inline void
push(PATH_STACK *s, PATH_NODE *node) {
    s->top++;
    s->arr[s->top] = node;
}

static inline PATH_NODE *
pop(PATH_STACK *s) {
    if (s->top < 0) {
        return NULL;
    } else {
        return s->arr[s->top--];
    }
}

static inline PATH_NODE *
top(PATH_STACK *s) {
    if (s->top < 0) {
        return NULL;
    } else {
        return s->arr[s->top];
    }
}

static inline int
full(PATH_STACK *s) {
    return s->top == s->capacity - 1;
}

static inline int
getfield(lua_State *L, const char *f) {
    if (lua_getfield(L, -1, f) != LUA_TNUMBER) {
        return luaL_error(L, "invalid type %s", f);
    }
    int v = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return v;
}

static inline int check_in_map(int x, int y, int w, int h) {
    return x >= 0 && y >= 0 && x < w && y < h;
}

static PATH_NODE *construct(int x, int y, int w, int h, unsigned char *father_map) {
    int map_len = w * h;
    int mem_len = BITSLOT(map_len) + 1;
    PATH_NODE *node = (PATH_NODE *)malloc(sizeof(PATH_NODE) + mem_len * sizeof(node->m[0]));
    node->x = x;
    node->y = y;
    if (father_map == NULL) {
        memset(node->m, 0, mem_len * sizeof(node->m[0]));
    } else {
        memcpy(node->m, father_map, mem_len * sizeof(node->m[0]));
    }
    BITSET(node->m, y * w + x);
    return node;
}

static int *get_neighbor(int x, int y, int dir) {
    static int pos[2] = {0};
    switch (dir) {
        case 1: { // up
            pos[0] = x;
            pos[1] = y - 1;
            break;
        }
        case 2: { // down
            pos[0] = x;
            pos[1] = y + 1;
            break;
        }
        case 3: { // left
            pos[0] = x - 1;
            pos[1] = y;
            break;
        }
        case 4: { // right
            pos[0] = x + 1;
            pos[1] = y;
            break;
        }
    }
    return pos;
}

static int *shuffe() {
    int i, s, c;
    static int r[4] = { 1, 2, 3, 4 };
    for (i = 0; i < 4; i++) {
        r[i] = i + 1;
    }
    for (i = 4; i > 0; i--) {
        s = rand() % i;
        c = r[s];
        r[s] = r[i - 1];
        r[i - 1] = c;
    }
    return r;
}

static int
find_path(PATH_STACK *s, int w, int h) {
    PATH_NODE *p = top(s);
    if (p != NULL) {
        int *arr = shuffe(), mark = 0;
        for (int i = 0; i < 4; i++) {
            int *pos = get_neighbor(p->x, p->y, arr[i]);
            int nx = pos[0], ny = pos[1];
            if (check_in_map(nx, ny, w, h) && !BITTEST(p->m, ny * w + nx)) {
                PATH_NODE *cur = construct(nx, ny, w, h, p->m);
                push(s, cur);
                mark = 1;
                if (full(s)) {
                    return 1;
                }
                if (find_path(s, w, h)) {
                    return 1;
                } else {
                    mark = 0;
                }
            }
        }
        if (!mark) {
            PATH_NODE *p = pop(s);
            if (p != NULL) {
                free(p);
            }
        }
    }
    return 0;
}

static void
back_tracking(lua_State *L, PATH_STACK *s) {
    lua_newtable(L);
    int num = 0;
    int x, y;
    PATH_NODE *p;
    while (s->top >= 0) {
        p = s->arr[s->top--];
        x = p->x;
        y = p->y;
        lua_newtable(L);
        lua_pushinteger(L, x);
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, y);
        lua_rawseti(L, -2, 2);
        lua_rawseti(L, -2, ++num);
        free(p);
    }
}

static int
create_maze(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);
    int w = getfield(L, "w");
    int h = getfield(L, "h");
    int path_limit = getfield(L, "l");
    int sx = getfield(L, "sx");
    int sy = getfield(L, "sy");
    lua_assert(w > 0 && h > 0 && check_in_map(sx, sy, w, h));
    if (path_limit <= 0 || path_limit > w * h) {
        return 0;
    }
    PATH_STACK *s = (PATH_STACK *)malloc(sizeof(PATH_STACK) + path_limit * sizeof(s->arr[0]));
    s->capacity = path_limit;
    s->top = -1;
    PATH_NODE *first = construct(sx, sy, w, h, NULL);
    push(s, first);
    if (path_limit == 1) {
        back_tracking(L, s);
    } else {
        find_path(s, w, h);
        back_tracking(L, s);
    }
    free(s);
    return 1;
}

static int
set_rseed(lua_State *L) {
    int seed = lua_tointeger(L, -1);
    srand((unsigned)seed);
    return 0;
}

LUAMOD_API int
luaopen_maze(lua_State *L) {
    luaL_checkversion(L);
    luaL_Reg l[] = {
        { "create_maze", create_maze },
        { "set_rseed", set_rseed },
        { NULL, NULL },
    };
    luaL_newlib(L, l);
    return 1;
}
