/* eslint-disable no-console */
var app = require('http').createServer(handler)
var io = require('socket.io')(app);

app.listen(3000);

function handler(req, res) {
    res.writeHead(200);
    res.end('Proto Nerfserv');
}

io.on('connection', function(socket) {
    console.log('client connected');
    socket.emit('hi there', 'we\'re connected!');
});