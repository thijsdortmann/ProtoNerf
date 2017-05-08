const guns = require('./guns');
const ttt = require('./ttt');

module.exports.gameState = false;
module.exports.allowReloading = true;

module.exports.startGame = function() {
    module.exports.gameState = true;
    guns.queueCommandForAll('setGameState', 'true');
};

module.exports.prepareTTT = function() {
    ttt.prepareTTT();
};