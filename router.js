const body_parser = require("body-parser")
var express = require('express')
var router = express.Router()

var spawn = require('child_process').spawn

router.use(body_parser.json())

router.use(function timeLog(req, res, next) {
    console.log('Time: ', new Date())
    next()
})

router.post('/maze', function(req, res) {
    let data = req.body
    console.log(data)
    const l = spawn('lua', ['./create_maze.lua', data.w, data.h, data.l, data.x, data.y,
        data.ex_num, data.ex_limit])
    l.stdout.on('data', (data) => {
        console.log('out = ', data)
        res.end(data)
    })
    l.stderr.on('data', (data) => {
        console.log('err = ', data)
        res.end(data)
    })
})

module.exports = router
