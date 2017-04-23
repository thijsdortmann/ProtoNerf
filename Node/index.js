const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

// Make app listen on port 3000.
http.listen(3000, function() {
    console.log('ProtoNerf Server is listening on port 3000.');
});

// Make app serve static files from public_http directory
app.use('/static', express.static('public_http'));

app.get('/', function(req, res) {
    res.sendFile(__dirname + '/public_http/root.html');
});

// This callback handles the functions for a connected device.
io.on('connection', function(socket) {
    let nick = 0;

    console.log('client connected');
    socket.emit('hello', '');
    socket.on('nick', function(data) {
        console.log('user identified as:', data);
        nick = data;
    });

    socket.on('disconnect', function() {
        console.log('disconnected');
    });
});

app.get('/test', function(req, res) {
    io.emit('p', req.query.msg);
    res.send('OK');
});