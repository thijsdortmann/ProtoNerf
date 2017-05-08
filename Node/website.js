const server = require('./server');
const app = server.app;
const express = server.express;
const io = server.io;

const guns = require('./guns');
const game = require('./game');

const nsp = io.of('/website');

// Make app serve static files from public_http directory
app.use('/static', express.static('public_http'));

app.get('/', function(req, res) {
    res.sendFile(__dirname + '/public_http/root.html');
});

nsp.on('connection', function(socket) {
    console.log('website client connected');
    socket.on('identify', function(data) {
        if(data == process.env.ADMIN_PASS) {
            socket.emit('players', generatePlayersList());

            socket.on('startTTT', function() {
                game.startTTT();
            });

            socket.on('startCapture', function() {
                game.startCapture();
            });

            socket.on('startGame', function() {
                game.startGame();
            });
        }
    });
});

module.exports.playersUpdate = function() {
    nsp.emit('players', generatePlayersList());
};

module.exports.log = function(message) {
    nsp.emit('log', message);
};

module.exports.setTime = function(time) {
    nsp.emit('timer', str_pad_left(Math.floor(time / 60),'0',2)+':'+str_pad_left((time - (Math.floor(time / 60) * 60)),'0',2));
};

function generatePlayersList() {
    const playersWeb = [];

    for (let i=0; i < guns.players.length; i++) {
        playersWeb.push({
            'name' : guns.players[i].name,
            'role' : guns.players[i].role,
            'teamColor' : guns.players[i].teamcolor
        });
    }

    return playersWeb;
}

function str_pad_left(string,pad,length) {
    return (new Array(length+1).join(pad)+string).slice(-length);
}