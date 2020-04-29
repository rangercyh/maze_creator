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
                let grid = new Grid({
                    container: document.getElementById("map"),
                    colCount: data.w,
                    rowCount: data.h,
                    width: data.w * 50,
                    height: data.h * 50,
                })
                let grids = grid.getGrids();
                for(let i = 0; i < grids.length; i++){
                    grids[i].innerHTML = i + 1;
                }
                for (let i in data.path) {
                    grids[data.path[i][1]].innerHTML = data.path[i][2]
                    if (i == 1) {
                        grids[data.path[i][1]].style["background"] = "yellow"
                    } else if (i == data.path_num) {
                        grids[data.path[i][1]].style["background"] = "orange"
                    } else {
                        grids[data.path[i][1]].style["background"] = "green"
                    }
                }
                for (let i in data.ex) {
                    grids[data.ex[i][1]].innerHTML = data.ex[i][2]
                    grids[data.ex[i][1]].style["background"] = "red"
                }
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

