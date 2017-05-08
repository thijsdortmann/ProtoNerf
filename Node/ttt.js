const guns = require('./guns');
const website = require('./website');

const traitors = [];
const innocent = [];
let detective;

module.exports.prepareTTT = function() {
    shuffle(guns.players);

    const amountTraitors = Math.ceil((guns.players.length / 4));

    for(let i = 0; i < amountTraitors; i++) {
        traitors.push(guns.players[i]);
        guns.players[i].queueCommand('allowColorcustomization', 'false');
        guns.players[i].setTeamColor('000255000');
        guns.players[i].setRole('TRAITOR');
    }

    for(let i = amountTraitors; i < guns.players.length - 1; i++) {
        innocent.push(guns.players[i]);
        guns.players[i].queueCommand('allowColorcustomization', 'false');
        guns.players[i].setTeamColor('000255000');
        guns.players[i].setRole('INNOCENT');
    }

    detective = guns.players[guns.players.length - 1];
    detective.queueCommand('allowColorcustomization', 'false');
    detective.setTeamColor('000000255');
    detective.setRole('DETECTIVE');

    website.playersUpdate();
};

module.exports.getTraitorNames = function() {
    let traitorNames = [];

    traitors.forEach(function(traitor) {
        traitorNames.push(traitor.name);
    });
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