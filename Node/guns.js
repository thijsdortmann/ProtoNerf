const server = require('./server');
const io = server.io;
const connection = require('./database');

const players = [];

module.exports.queueCommandForAll = function(command, data) {
    players.forEach(function(player) {
        player.queueCommand(command, data);
    });
};

module.exports.sendBroadcast = function(message) {
    players.forEach(function(player) {
        player.queueCommand('broadcast', message);
    });
};

module.exports.players = players;

// This callback handles the functions for a connected device.
io.on('connection', function(socket) {
    let playerdata = 0;

    console.log('client connected');

    socket.on('identify', function(uid) {
        console.log('user identified as:', uid, ', checking...');

        connection.query('SELECT * FROM guns WHERE `uid` = ?', [uid], function(error, results, fields) {
            if(error) throw error;
            if(results.length > 0) {
                playerdata = results[0];
                console.log(uid, 'identified as', playerdata.name);
                socket.emit('setNickname', playerdata.name);

                playerdata.socket = socket;

                // As the library on the Wemos doesn't like handling multiple emits per cycle
                // we have to queue the commands and send them a bit slower.
                // TODO: the command queue is not nicely implemented.
                playerdata.commandQueue = [];

                playerdata.queueCommand = function(command, data) {
                    this.commandQueue.push({
                        'command' : command,
                        'data' : data
                    });
                };

                playerdata.commandQueueHandler = setInterval(function() {
                    if(playerdata.commandQueue.length > 0) {
                        let command = playerdata.commandQueue.shift();
                        socket.emit(command.command, command.data);
                    }
                }, 100);

                players.push(playerdata);
                playerdata.queueCommand('allowColorcustomization', 'false');
                playerdata.queueCommand('setTeamColor', '255255255');
            }else{
                socket.emit('broadcast', 'Please register your gun with UID ' + uid);
            }
        });
    });

    socket.on('disconnect', function() {
        console.log('disconnected');
        if(playerdata) {
            clearInterval(playerdata.commandQueueHandler);
            players.splice(players.indexOf(playerdata), 1);
        }
    });
});