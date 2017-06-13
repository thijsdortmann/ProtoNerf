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

app.get('/playerData', function(req, res) {
    res.send(ConvertToCSV(JSON.stringify(generatePlayersList())));
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

            socket.on('startED', function() {
                game.startED();
            });

            socket.on('startGame', function() {
                game.startGame();
            });

            socket.on('color', function(data) {
                for(let i = 0; i < guns.players.length; i++) {
                    guns.players[i].setTeamColor(data);
                }
            });

            socket.on('registerKill', function(index) {
                guns.players[index].isKilled();
            });

            socket.on('kick', function(index) {
                guns.players[index].kick();
            });
        }
    });
});

module.exports.playersUpdate = function() {
    nsp.emit('players', generatePlayersList());
};

setInterval(module.exports.playersUpdate, 500);

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
            'index': i,
            'name' : guns.players[i].name,
            'role' : guns.players[i].role,
            'teamColor' : guns.players[i].teamcolor,
            'bullets' : guns.players[i].bullets,
            'deaths' : guns.players[i].deaths,
            'alive' : guns.players[i].alive
        });
    }

    return playersWeb;
}

function str_pad_left(string,pad,length) {
    return (new Array(length+1).join(pad)+string).slice(-length);
}



// JSON to CSV Converter
function ConvertToCSV(objArray) {
    var array = typeof objArray != 'object' ? JSON.parse(objArray) : objArray;
    var str = '';

    for (var i = 0; i < array.length; i++) {
        var line = '';
        for (var index in array[i]) {
            if (line != '') line += ','

            line += array[i][index];
        }

        str += line + '\r\n';
    }

    return str;
}