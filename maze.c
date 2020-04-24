#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

typedef struct link_node {
    int x;
    int y;
    struct link_node *pre;
    struct link_node *gc;
} PATH_NODE;

typedef struct gc_list {
    PATH_NODE *first;
    PATH_NODE *last;
} GC_LIST;

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

static PATH_NODE *construct(int x, int y, GC_LIST *l) {
    PATH_NODE *node = (PATH_NODE *)malloc(sizeof(PATH_NODE));
    node->x = x;
    node->y = y;
    node->pre = NULL;
    node->gc = NULL;
    if (!l->first) {
        l->first = node;
    } else {
        l->last->gc = node;
    }
    l->last = node;
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
        r[s] = r[i];
        r[i] = c;
    }
    return r;
}

static int check_expanded(int x, int y, PATH_NODE *pre) {
    PATH_NODE *p = pre;
    while (p) {
        if (p->x == x && p->y == y) {
            return 1;
        }
        p = p->pre;
    }
    return 0;
}

static PATH_NODE *
find_path(int path_limit, int len, int x, int y, int w, int h, PATH_NODE *pre, GC_LIST *gc) {
    int *arr = shuffe();
    for (int i = 0; i < 4; i++) {
        int *pos = get_neighbor(x, y, arr[i]);
        int nx = pos[0], ny = pos[1];
        if (check_in_map(nx, ny, w, h) && !check_expanded(nx, ny, pre)) {
            PATH_NODE *cur = construct(nx, ny, gc);
            cur->pre = pre;
            if (len + 1 >= path_limit) {
                return cur;
            }
            PATH_NODE *r = find_path(path_limit, len + 1, nx, ny, w, h, cur, gc);
            if (r) {
                return r;
            }
        }
    }
    return NULL;
}

static void
form_path(lua_State *L, PATH_NODE *last) {
    lua_newtable(L);
    int num = 0;
    PATH_NODE *p = last;
    int x, y;
    while (p) {
        x = p->x;
        y = p->y;
        lua_newtable(L);
        lua_pushinteger(L, x);
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, y);
        lua_rawseti(L, -2, 2);
        lua_rawseti(L, -2, ++num);
        p = p->pre;
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
    GC_LIST *list = (GC_LIST *)malloc(sizeof(GC_LIST));
    list->first = list->last = NULL;
    PATH_NODE *first = construct(sx, sy, list);
    if (path_limit == 1) {
        form_path(L, first);
    } else {
        PATH_NODE *last = find_path(path_limit, 1, sx, sy, w, h, first, list);
        form_path(L, last);
    }
    PATH_NODE *p = list->first;
    while (p) {
        list->first = list->first->gc;
        free(p);
        p = list->first;
    }
    free(list);
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
