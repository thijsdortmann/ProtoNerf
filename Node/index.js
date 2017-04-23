var app = require('http').createServer(handler)
var io = require('socket.io')(app);

app.listen(3000);

function handler(req, res) {
    res.writeHead(200);
    res.end('Proto Nerfserv');
}

io.on('connection', function(socket) {
    var nick = 0;

    console.log('client connected');
    socket.emit('hello', '');
    socket.on('nick', function(data) {
        console.log("user identified as:", data);
        nick = data;
    })
});