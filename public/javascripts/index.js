var form = angular.module('input_form', [])
form.controller('input_controller', function($scope, $http) {
    let w = 3, h = 3, l = 5, x = 1, y = 1, ex_num = 0, ex_limit = 1
    $scope.w = w
    $scope.h = h
    $scope.l = l
    $scope.x = x
    $scope.y = y
    $scope.ex_num = ex_num
    $scope.ex_limit = ex_limit


    input_cb($('#w'), 1, w)
    input_cb($('#h'), 1, h)
    set_l_max(3 * 3)
    input_cb($('#l'), 0, l)
    input_cb($('#x'), 0, x)
    input_cb($('#y'), 0, y)
    input_cb($('#ex_num'), 0, ex_num)
    input_cb($('#ex_limit'), 0, ex_limit)

    $scope.set_span = function(val) {
        if (val > 40 && val < 80) {
            return parseInt(val * 0.6)
        }
        if (val >= 80) {
            return parseInt(val * 0.5)
        }
        return val
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
            console.log('succ call')
            console.log(resp)
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

function set_little_span(val) {
    if (val > 40 && val < 80) {
        return parseInt(val * 0.6)
    }
    if (val >= 80) {
        return parseInt(val * 0.5)
    }
    return val
}

function set_l_max(val) {
    let max = parseInt($('#l').attr('max'))
    let cur = parseInt($('#l').val())
    let v = set_little_span(val)
    $('#l').attr('max', v)
    if (cur > v) {
        $('#l').val(v).trigger('ng-change')
    }
}

function input_cb(obj, is_add, deval) {
    obj.on('input', function() {
        let max = parseInt($(this).attr('max'))
        let min = parseInt($(this).attr('min'))
        let cur = parseInt($(this).val())
        if (cur > max) {
            $(this).val(max)
        }
        if (cur < min) {
            $(this).val(max)
        }
        if (isNaN(cur)) {
            $(this).val(deval)
        }
        setTimeout(function() {
            obj.trigger('ng-change')
        }, 0)
    })
    if (is_add == 1) {
        obj.on('ng-change', function() {
            let scope = angular.element($('div[ng-app="input_form"]')).scope()
            console.log('change', scope.w, scope.h)
            set_l_max(scope.w * scope.h)
        })
    }
}

