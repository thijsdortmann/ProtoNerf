const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

const guns = require('./guns');

// Make app listen on port 3000.
http.listen(3000, function() {
    console.log('ProtoNerf Server is listening on port 3000.');
});

// Make app serve static files from public_http directory
app.use('/static', express.static('public_http'));

app.get('/', function(req, res) {
    res.sendFile(__dirname + '/public_http/root.html');
});

module.exports.app = app;
module.exports.http = http;
module.exports.io = io;

app.get('/broadcast', function(req, res) {
    guns.sendBroadcast(req.query.msg);
    res.send('OK');
});

app.get('/setRole', function(req, res) {
    guns.queueCommandForAll('setRole', req.query.role);
    res.send('OK');
});

app.get('/startTimer', function(req, res) {
    guns.queueCommandForAll('startTimer', req.query.time);
    res.send('OK');
});

app.get('/allowColorcustomization', function(req, res) {
    guns.queueCommandForAll('allowColorcustomization', req.query.allow);
    res.send('OK');
});

app.get('/setTeamColor', function(req, res) {
    // Color needs to formatted as 255255255 (RGB, three digits per color)
    guns.queueCommandForAll('setTeamColor', req.query.color);
    res.send('OK');
});

app.get('/setGameState', function(req, res) {
    // Color needs to formatted as 255255255 (RGB, three digits per color)
    guns.queueCommandForAll('setGameState', req.query.state);
    res.send('OK');
});

app.get('/players', function(req, res) {
    res.send(players);
});