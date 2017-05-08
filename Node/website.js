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

            socket.on('prepareTTT', function() {
                game.prepareTTT();
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

function generatePlayersList() {
    const playersWeb = [];

    for (let i=0; i < guns.players.length; i++) {
        playersWeb.push({
            'name' : guns.players[i].name,
            'role' : guns.players[i].role,
            'teamColor' : guns.players.teamcolor
        });
    }

    return playersWeb;
}