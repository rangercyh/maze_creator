var dir = {
    1:[ // UP
        {img: "url(img/1.webp)", r: "rotate(90deg)"},
        {img: "url(img/2.webp)", r: "rotate(90deg)"}
    ],
    2:[ // DOWN
        {img: "url(img/1.webp)", r: "rotate(-90deg)"},
        {img: "url(img/2.webp)", r: "rotate(-90deg)"}
    ],
    4:[ // LEFT
        {img: "url(img/1.webp)", r: "rotate(0deg)"},
        {img: "url(img/2.webp)", r: "rotate(0deg)"}
    ],
    8:[ // RIGHT
        {img: "url(img/1.webp)", r: "rotate(180deg)"},
        {img: "url(img/2.webp)", r: "rotate(180deg)"}
    ],
    3:[ // UP DOWN
        {img: "url(img/3.webp)", r: "rotate(90deg)"},
        {img: "url(img/3.webp)", r: "rotate(-90deg)"}
    ],
    5:[ // UP LEFT
        {img: "url(img/4.webp)", r: "rotate(90deg)"}
    ],
    9:[ // UP RIGHT
        {img: "url(img/4.webp)", r: "rotate(180deg)"}
    ],
    6:[ // DOWN LEFT
        {img: "url(img/4.webp)", r: "rotate(0deg)"}
    ],
    10:[ // DOWN RIGHT
        {img: "url(img/4.webp)", r: "rotate(-90deg)"}
    ],
    12:[ // LEFT RIGHT
        {img: "url(img/3.webp)", r: "rotate(0deg)"},
        {img: "url(img/3.webp)", r: "rotate(180deg)"}
    ],
    7:[ // UP DOWN LEFT
        {img: "url(img/5.webp)", r: "rotate(0deg)"}
    ],
    11:[ // UP DOWN RIGHT
        {img: "url(img/5.webp)", r: "rotate(180deg)"}
    ],
    13:[ // UP LEFT RIGHT
        {img: "url(img/5.webp)", r: "rotate(90deg)"}
    ],
    14:[ // DOWN LEFT RIGHT
        {img: "url(img/5.webp)", r: "rotate(-90deg)"}
    ],
    15:[ // UP DOWN LEFT RIGHT
        {img: "url(img/6.webp)", r: "rotate(0deg)"},
        {img: "url(img/6.webp)", r: "rotate(90deg)"},
        {img: "url(img/6.webp)", r: "rotate(180deg)"},
        {img: "url(img/6.webp)", r: "rotate(-90deg)"}
    ],
}

function show_char(grids, data) {
    for(let i = 0; i < grids.length; i++){
        grids[i].innerHTML = i + 1;
    }
    let g
    for (let i in data.path) {
        g = grids[data.path[i][1]]
        g.innerHTML = data.path[i][2]
        if (i == 1) {
            g.style["background"] = "yellow"
        } else if (i == data.path_num) {
            g.style["background"] = "orange"
        } else {
            g.style["background"] = "green"
        }
    }
    for (let i in data.ex) {
        g = grids[data.ex[i][1]]
        g.innerHTML = data.ex[i][2]
        g.style["background"] = "red"
    }
}

function set_style(g, sum) {
    let arr = dir[sum]
    let idx = Math.floor((Math.random()*arr.length))
    let v = arr[idx]
    g.style["background-image"] = v.img
    g.style["-webkit-transform"] = v.r
    g.style["background-size"] = "100% 100%"
    g.style["background-repeat"] = "no-repeat"

    g.style["line-height"] = "80px"
    g.style["text-align"] = "center"
    g.style["vertical-align"] = "middle"
    g.style["font-size"] = "20px"
    g.style["font-weight"] = "bold"
    g.style["color"] = "red"
}

function show_img(grids, data) {
    let g
    for (let i in data.path) {
        g = grids[data.path[i][1]]
        set_style(g, data.path[i][3])
        if (i == 1) {
            g.innerHTML = "起点"
        }
        if (i == data.path_num) {
            g.innerHTML = "终点"
        }
    }
    for (let i in data.ex) {
        g = grids[data.ex[i][1]]
        set_style(g, data.ex[i][3])
        g.innerHTML = "扩展"
    }
}

var form = angular.module('input_form', [])
form.controller('input_controller', function($scope, $http) {
    let w = 3, h = 3, l = 5, x = 1, y = 1, ex_num = 0, ex_limit = 1
    batch_set_max(w, h)
    $scope.w = w
    $scope.h = h
    $scope.l = l
    $scope.x = x
    $scope.y = y
    $scope.ex_num = ex_num
    $scope.ex_limit = ex_limit

    input_cb($('#w'), w, 1)
    input_cb($('#h'), h, 1)
    input_cb($('#l'), l, 0, 1)
    input_cb($('#x'), x, 0, 1)
    input_cb($('#y'), y, 0, 1)
    input_cb($('#ex_num'), ex_num)
    input_cb($('#ex_limit'), ex_limit)

    $scope.set_span = function(val) {
        return get_little_span(val)
    }

    $scope.submit = function() {
        $http.post('/maze', {
            w: $scope.w,
            h: $scope.h,
            l: $scope.l,
            x: $scope.x - 1,
            y: $scope.y - 1,
            ex_num: $scope.ex_num,
            ex_limit: $scope.ex_limit
        }).then(function(resp) {
            let data = resp.data
            document.getElementById("map").innerHTML=""
            if (typeof(data.errmsg) != "undefined") {
                alert(data.errmsg)
            } else {
                let grid_len = 100
                let grid = new Grid({
                    container: document.getElementById("map"),
                    colCount: data.w,
                    rowCount: data.h,
                    width: data.w * grid_len,
                    height: data.h * grid_len,
                })
                let grids = grid.getGrids()
                // show_char(grids, data)
                show_img(grids, data)
                document.getElementById("time").innerHTML= "use time: " + data.time + " ms"
            }
        }, function(resp) {
            // $scope.data = resp.data || 'Request failed';
            // $scope.status = resp.status;
            console.log('error call')
            console.log(resp)
        })
    }
})

function batch_set_max(w, h) {
    $('#l').attr('max', w * h)
    $('#x').attr('max', w)
    $('#y').attr('max', h)
}

function batch_trigger_event() {
    $('#l').trigger('refresh')
    $('#x').trigger('refresh')
    $('#y').trigger('refresh')
}

function get_little_span(val) {
    if (val > 40 && val < 80) {
        return parseInt(val * 0.6)
    }
    if (val >= 80) {
        return parseInt(val * 0.5)
    }
    return val
}

function get_cur_val(cur, max) {
    if (cur > max) {
        return max
    }
    return cur
}

function input_cb(obj, deval, pop_event, sub_event) {
    obj.on('input', function() {
        let max = parseInt($(this).attr('max'))
        let min = parseInt($(this).attr('min'))
        let cur = parseInt($(this).val())
        if (cur > max) {
            $(this).val(max)
        }
        if (cur < min) {
            $(this).val(min)
        }
        if (isNaN(cur)) {
            $(this).val(deval)
        }
        setTimeout(function() {
            obj.trigger('ng-change')
        }, 0)
        if (pop_event == 1) {
            setTimeout(function() {
                batch_trigger_event()
            }, 0)
        }
    })
    if (sub_event == 1) {
        obj.on('refresh', function(e, w , h) {
            let scope = angular.element($('div[ng-app="input_form"]')).scope()
            scope.l = get_cur_val(scope.l, get_little_span(scope.w * scope.h))
            scope.x = get_cur_val(scope.x, scope.w)
            scope.y = get_cur_val(scope.y, scope.h)
            batch_set_max(scope.w, scope.h)
            scope.$apply()
        })
    }
}

