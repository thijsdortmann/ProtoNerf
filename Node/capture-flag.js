const guns = require('./guns');
const website = require('./website');

const team1 = [];
const team2 = [];

module.exports.prepareCapture = function() {
    shuffle(guns.players);

    for(let i = 0; i < Math.floor(guns.players.length / 2); i++) {
        team1.push(guns.players[i]);
        guns.players[i].queueCommand('allowColorcustomization', 'false');
        guns.players[i].setTeamColor('255000000');
        guns.players[i].setRole('TEAM 1');
    }

    for(let i = team1.length; i < guns.players.length; i++) {
        team2.push(guns.players[i]);
        guns.players[i].queueCommand('allowColorcustomization', 'false');
        guns.players[i].setTeamColor('000000255');
        guns.players[i].setRole('TEAM 2');
    }

    website.playersUpdate();
};

/**
 * Shuffles array in place. ES6 version
 * @param {Array} a items The array containing the items.
 */
function shuffle(a) {
    for (let i = a.length; i; i--) {
        let j = Math.floor(Math.random() * i);
        [a[i - 1], a[j]] = [a[j], a[i - 1]];
    }
}