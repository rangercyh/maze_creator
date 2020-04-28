var path = require('path')
var http = require('http')
var express = require('express')
var router = require('./router')

var app = express()

app.use(express.static(path.join(__dirname, 'public')))

app.use('/', router)

http.createServer(app).listen(8080)
