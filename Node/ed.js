const guns = require('./guns');

module.exports.prepareED = function() {
    for(let i = 0; i < guns.players.length; i++) {
        guns.players[i].queueCommand('allowColorcustomization', 'false');
        guns.players[i].setRole('ENTREPRENEUR');
        guns.players[i].setAllowReloading('true');
        guns.players[i].setAllowFiringmode('true');
        guns.players[i].canRespawn = false;
    }
};