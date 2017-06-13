const guns = require('./guns');
const ttt = require('./ttt');
const captureFlag = require('./capture-flag');
const ED = require('./ed');
const website = require('./website');

module.exports.gameState = false;
module.exports.allowReloading = true;

module.exports.startGame = function() {
    module.exports.gameState = true;
    guns.queueCommandForAll('setGameState', 'true');
};

module.exports.startTTT = function() {
    ttt.prepareTTT();
    module.exports.startGame();
    module.exports.startGameTimer(900,
        function(timer) {
            website.setTime(timer);
        }, function() {
            guns.queueCommandForAll('setGameState', 'false');
        });
    guns.queueCommandForAll('startTimer', '1500');
};

module.exports.startCapture = function() {
    captureFlag.prepareCapture();
    module.exports.startGame();
    module.exports.startGameTimer(900,
        function(timer) {
            website.setTime(timer);
        }, function() {
            guns.queueCommandForAll('setGameState', 'false');
        });
    guns.queueCommandForAll('startTimer', '1500');
};

module.exports.startED = function() {
    ED.prepareED();
    module.exports.startGame();
    module.exports.startGameTimer(90,
        function(timer) {
            website.setTime(timer);
        }, function() {
            guns.queueCommandForAll('setGameState', 'false');
        });
    guns.queueCommandForAll('startTimer', '0130');
};

let gameTimer = 0;
let gameTimerInterval;

module.exports.startGameTimer = function(seconds, interval, callback) {
    clearInterval(gameTimerInterval);
    gameTimer = seconds;
    gameTimerInterval = setInterval(function() {
        gameTimer--;
        interval(gameTimer);
        if(gameTimer == 0) {
            clearInterval(gameTimerInterval);
            callback();
        }
    }, 1000);
};