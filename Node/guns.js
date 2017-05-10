const server = require('./server');
const io = server.io;
const connection = require('./database');
const website = require('./website');
const game = require('./game');

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
    let player = 0;

    console.log('client connected');

    socket.on('identify', function(uid) {
        console.log('user identified as:', uid, ', checking...');

        connection.query('SELECT * FROM guns WHERE `uid` = ?', [uid], function(error, results, fields) {
            if(error) throw error;
            if(results.length > 0) {
                if(results[0].allowed && !game.gameState) {
                    player = results[0];

                    player.alive = true;
                    player.allowReloading = game.allowReloading;
                    player.allowFiringmode = game.gameState;
                    player.role = '';
                    player.teamcolor = '000000000';
                    player.bullets = 20;
                    player.deaths = 0;
                    player.canRespawn = true;

                    console.log(uid, 'identified as', player.name);

                    player.queueCommand('setNickname', player.name);

                    player.setAllowFiringmode(player.allowFiringmode);

                    // As the library on the Wemos doesn't like handling multiple emits per cycle
                    // we have to queue the commands and send them a bit slower.
                    // TODO: the command queue is not nicely implemented.
                    player.commandQueue = [];

                    player.queueCommand = function(command, data) {
                        this.commandQueue.push({
                            'command' : command,
                            'data' : data
                        });
                    };

                    player.commandQueueHandler = setInterval(function() {
                        if(player.commandQueue.length > 0) {
                            let command = player.commandQueue.shift();
                            socket.emit(command.command, command.data);
                        }
                    }, 200);

                    player.setTeamColor = function(color) {
                        player.teamcolor = color;
                        player.queueCommand('setTeamColor', color);
                    };

                    player.setAllowReloading = function(allowReloading) {
                        player.allowReloading = allowReloading;
                        player.queueCommand('allowReloading', allowReloading);
                    };

                    player.setAllowFiringmode = function(allowFiringmode) {
                        player.allowFiringmode = allowFiringmode;
                        player.queueCommand('allowFiringmode', allowFiringmode);
                    };

                    player.setRole = function(role) {
                        player.role = role;
                        player.queueCommand('setRole', role);
                    };

                    player.isKilled = function() {
                        console.log('player', player.name, 'has been killed');
                        player.queueCommand('broadcast', 'Your death has been registered.');
                        player.alive = false;
                        player.deaths++;
                        player.setAllowFiringmode('false');
                        if(player.canRespawn) {
                            setTimeout(function() {
                                player.alive = true;
                                player.setAllowFiringmode('true');
                                player.queueCommand('broadcast', 'You are alive again.');
                            }, 10000);
                        }
                    };

                    player.kick = function() {
                        console.log('kicking');
                        socket.disconnect();
                    };

                    players.push(player);
                    //website.playersUpdate();

                    socket.on('shotFired', function(bullets) {
                        player.bullets = bullets;
                        website.log('shot was fired by ' + player.name);
                    });

                    socket.on('reloaded', function(bullets) {
                        player.bullets = bullets;
                        website.log('gun was reloaded by ' + player.name);
                    });
                }else{
                    socket.emit('broadcast', 'You\'re currently not allowed to join.');
                }
            }else{
                socket.emit('broadcast', 'Please register your gun with UID ' + uid);
            }
        });
    });

    socket.on('disconnect', function() {
        console.log('disconnected');
        if(player) {
            clearInterval(player.commandQueueHandler);
            players.splice(players.indexOf(player), 1);
            website.playersUpdate();
        }
    });
});