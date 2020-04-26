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
    int dir;
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

static PATH_NODE *construct(int x, int y, int dir, int w, int h, unsigned char *father_map) {
    int map_len = w * h;
    int mem_len = BITSLOT(map_len) + 1;
    PATH_NODE *node = (PATH_NODE *)malloc(sizeof(PATH_NODE) + mem_len * sizeof(node->m[0]));
    node->x = x;
    node->y = y;
    node->dir = dir;
    if (father_map == NULL) {
        memset(node->m, 0, mem_len * sizeof(node->m[0]));
    } else {
        memcpy(node->m, father_map, mem_len * sizeof(node->m[0]));
    }
    BITSET(node->m, y * w + x);
    return node;
}

#define DIR_NUM 4
#define EMPTY_DIR -1
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

static int *get_neighbor(int x, int y, int dir) {
    static int pos[2] = {0};
    switch (dir) {
        case UP: {
            pos[0] = x;
            pos[1] = y - 1;
            break;
        }
        case DOWN: {
            pos[0] = x;
            pos[1] = y + 1;
            break;
        }
        case LEFT: {
            pos[0] = x - 1;
            pos[1] = y;
            break;
        }
        case RIGHT: {
            pos[0] = x + 1;
            pos[1] = y;
            break;
        }
    }
    return pos;
}

static void shuffe(int *r) {
    int i, s, c;
    for (i = DIR_NUM; i > 0; i--) {
        s = rand() % i;
        c = r[s];
        r[s] = r[i - 1];
        r[i - 1] = c;
    }
}

static int
find_path(PATH_STACK *s, int w, int h) {
    PATH_NODE *p = top(s), *cur;
    if (p != NULL) {
        int *pos, nx, ny, mark = 0;
        int arr[DIR_NUM] = { UP, DOWN, LEFT, RIGHT };
        shuffe(arr);
        for (int i = 0; i < DIR_NUM; i++) {
            pos = get_neighbor(p->x, p->y, arr[i]);
            nx = pos[0];
            ny = pos[1];
            // printf("check nei x = %d, y = %d, nx = %d, ny = %d, dir = %d\n", p->x, p->y, nx, ny, arr[i]);
            if (check_in_map(nx, ny, w, h) && !BITTEST(p->m, ny * w + nx)) {
                cur = construct(nx, ny, arr[i], w, h, p->m);
                push(s, cur);
                // printf("push create node x = %d, y = %d, nx = %d, ny = %d\n", p->x, p->y, nx, ny);
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
                printf("pop node x = %d, y = %d\n", p->x, p->y);
                free(p);
                p = NULL;
            }
        }
    }
    return 0;
}

typedef struct candidate_node {
    int x;
    int y;
    int dir_unit[DIR_NUM]; // UP DOWN LEFT RIGHT
    int choose_dir;
    struct candidate_node *next;
} CANDIDATE_NODE;

typedef struct candidate_list {
    int num;
    CANDIDATE_NODE *head;
    CANDIDATE_NODE *tail;
} CANDIDATE_LIST;

typedef struct expand_list {
    int num;
    CANDIDATE_NODE *arr[0];
} EXPAND_LIST;

static int
check_in_path(int x, int y, int w, int h, unsigned char *m) {
    return check_in_map(x, y, w, h) && BITTEST(m, y * w + x);
}

static CANDIDATE_NODE *
check_in_candidate(CANDIDATE_LIST *c, int x, int y) {
    CANDIDATE_NODE *p = c->head;
    while (p != NULL) {
        if (p->x == x && p->y == y) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

static int
check_in_expand(EXPAND_LIST *l, int x, int y) {
    for (int i = 0; i < l->num; i++) {
        if (l->arr[i]->x == x && l->arr[i]->y == y) {
            return 1;
        }
    }
    return 0;
}

static CANDIDATE_NODE *
construct_candidate(int x, int y, int dir, int dir_num) {
    CANDIDATE_NODE *cp = (CANDIDATE_NODE *)malloc(sizeof(CANDIDATE_NODE));
    cp->x = x;
    cp->y = y;
    cp->choose_dir = EMPTY_DIR;
    cp->next = NULL;
    memset(cp->dir_unit, 0, DIR_NUM * sizeof(int));
    cp->dir_unit[dir] = dir_num + 1;
    return cp;
}

static void
push_candidate(CANDIDATE_LIST *c, CANDIDATE_NODE *p) {
    c->num++;
    if (c->head == NULL) {
        c->head = c->tail = p;
    } else {
        c->tail->next = p;
        c->tail = p;
    }
}

static void
search_candidate_neighbor(int x, int y, int w, int h, unsigned char *m,
    CANDIDATE_LIST *c, EXPAND_LIST *l, int limit, int dir, int dir_num) {
    int *pos;
    int cx, cy;
    for (int i = 0; i < DIR_NUM; i++) {
        pos = get_neighbor(x, y, i);
        cx = pos[0];
        cy = pos[1];
        if (dir == i && (dir_num + 1) > limit) {
            break;
        }
        if (check_in_path(cx, cy, w, h, m)) {
            break;
        }
        if (check_in_expand(l, cx, cy)) {
            break;
        }
        CANDIDATE_NODE *p = check_in_candidate(c, cx, cy);
        if (p != NULL) {
            p->dir_unit[i] += 1;
            break;
        }
        push_candidate(c, construct_candidate(cx, cy, i, ((i == dir) ? dir_num : 0)));
    }
}

static CANDIDATE_LIST *
search_for_candidate(PATH_STACK *s, EXPAND_LIST *l, int limit, int w, int h) {
    CANDIDATE_LIST *c = (CANDIDATE_LIST *)malloc(sizeof(CANDIDATE_LIST));
    c->num = 0;
    c->head = c->tail = NULL;
    PATH_NODE *p, *end = top(s);
    for (int i = 1; i < s->top; i++) {
        p = s->arr[i];
        search_candidate_neighbor(p->x, p->y, w, h, end->m, c, l, limit, EMPTY_DIR, 0);
    }
    return c;
}

static void release_candidate(CANDIDATE_LIST *c) {
    if (c != NULL) {
        CANDIDATE_NODE *p;
        while (c->head != c->tail) {
            p = c->head;
            c->head = c->head->next;
            free(p);
        }
        if (c->head != NULL) {
            free(c->head);
        }
        free(c);
        c = NULL;
    }
}

static int add_expand(EXPAND_LIST *l, CANDIDATE_NODE *p) {
    // choose a dir
    int total_dir = 0;
    for (int i = 0; i < DIR_NUM; i++) {
        if (p->dir_unit[i] > 0) {
            total_dir++;
        }
    }
    int idx = rand() % total_dir;
    int dir_num = 0;
    for (int j = 0; j < DIR_NUM; j++) {
        if (p->dir_unit[j] > 0) {
            idx--;
            if (idx < 0) {
                p->choose_dir = j;
                dir_num = p->dir_unit[j];
                break;
            }
        }
    }
    l->arr[l->num++] = p;
    return dir_num;
}

static EXPAND_LIST *
expand(PATH_STACK *s, int ex_num, int ex_limit, int w, int h) {
    PATH_NODE *end = top(s);
    if (end == NULL || ex_num <= 0 || ex_limit <= 0) {
        return NULL;
    }
    EXPAND_LIST *l = (EXPAND_LIST *)malloc(sizeof(EXPAND_LIST) + ex_num * sizeof(CANDIDATE_NODE *));
    l->num = 0;
    printf("create l\n");
    // CANDIDATE_LIST *c = search_for_candidate(s, l, ex_limit, w, h);
    // int pos;
    // CANDIDATE_NODE *p, *q;
    // while (ex_num-- > 0) {
    //     pos = rand() % c->num;
    //     p = c->head;
    //     if (pos == 0) {
    //         c->head = c->head->next;
    //         c->num--;
    //     } else {
    //         pos--;
    //         while (pos-- > 0) {
    //             p = p->next;
    //         }
    //         q = p->next;
    //         p->next = p->next->next;
    //         p = q;
    //     }
    //     int dir_num = add_expand(l, p);
    //     search_candidate_neighbor(p->x, p->y, w, h, end->m, c, l, ex_limit, p->choose_dir, dir_num);
    // }
    // release_candidate(c);
    return l;
}

static void release_expand(EXPAND_LIST *l) {
    if (l != NULL) {
        for (int i = 0; i < l->num; i++) {
            free(l->arr[i]);
            l->arr[i] = NULL;
        }
        free(l);
        l = NULL;
    }
}

static inline int
check_max_bound(int cur, int bound) {
    return cur > bound ? cur : bound;
}

static inline int
check_min_bound(int cur, int bound) {
    return cur < bound ? cur : bound;
}

static inline void
add_lua_table(lua_State *L, int x, int y, int dir, int pos) {
    lua_newtable(L);
    lua_pushinteger(L, x);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, y);
    lua_rawseti(L, -2, 2);
    lua_pushinteger(L, dir);
    lua_rawseti(L, -2, 3);
    lua_rawseti(L, -2, pos);
}

static void
dump_to_lua(lua_State *L, PATH_STACK *s, EXPAND_LIST *l, int w, int h, int xshift, int yshift) {
    lua_newtable(L);
    lua_pushinteger(L, w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, h);
    lua_setfield(L, -2, "h");
    int num = 0;
    int x, y;
    PATH_NODE *p;
    while (s->top >= 0) {
        p = s->arr[s->top--];
        x = p->x + xshift;
        y = p->y + yshift;
        // printf("push = %d %d %d\n", x, y, p->dir);
        add_lua_table(L, x, y, p->dir, ++num);
        free(p);
        p = NULL;
    }
    CANDIDATE_NODE *q;
    if (l != NULL) {
        for (int i = 0; i < l->num; i++) {
            q = l->arr[i];
            x = q->x + xshift;
            y = q->y + yshift;
            add_lua_table(L, x, y, q->choose_dir, ++num);
            free(q);
            q = NULL;
        }
    }
    lua_pushinteger(L, num);
    lua_setfield(L, -2, "num");
}

static void
form_path(lua_State *L, PATH_STACK *s, EXPAND_LIST *l, int w, int h) {
    int x, y, pos = s->top;
    int min_x = w, max_x = -1, min_y = h, max_y = -1;
    PATH_NODE *p;
    while (pos >= 0) {
        p = s->arr[pos--];
        x = p->x;
        y = p->y;
        max_x = check_max_bound(x, max_x);
        max_y = check_max_bound(y, max_y);
        min_x = check_min_bound(x, min_x);
        min_y = check_min_bound(y, min_y);
        // printf("stack trace = %d %d\n", x, y);
    }
    if (l != NULL) {
        printf("l num = %d \n", l->num);
        for (int i = 0; i < l->num; i++) {
            x = l->arr[i]->x;
            y = l->arr[i]->y;
            max_x = check_max_bound(x, max_x);
            max_y = check_max_bound(y, max_y);
            min_x = check_min_bound(x, min_x);
            min_y = check_min_bound(y, min_y);
            printf("l trace = %d %d\n", x, y);
        }
    }
    printf("form_path = %d %d  %d  %d\n", min_x, max_x, min_y, max_y);
    dump_to_lua(L, s, l, max_x - min_x + 1, max_y - min_y + 1, -min_x, -min_y);
    release_expand(l);
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
    int ex_num = getfield(L, "ex_num");
    int ex_limit = getfield(L, "ex_limit");
    lua_assert(w > 0 && h > 0 && check_in_map(sx, sy, w, h));
    if (path_limit <= 0 || path_limit > w * h) {
        return 0;
    }
    PATH_STACK *s = (PATH_STACK *)malloc(sizeof(PATH_STACK) + path_limit * sizeof(s->arr[0]));
    s->capacity = path_limit;
    s->top = -1;
    PATH_NODE *first = construct(sx, sy, EMPTY_DIR, w, h, NULL);
    push(s, first);
    if (path_limit > 1) {
        printf("found path = %d\n", find_path(s, w, h));
    }
    form_path(L, s, expand(s, ex_num, ex_limit, w, h), w, h);
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
