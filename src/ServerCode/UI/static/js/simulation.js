$(document).ready(function(){
    var socket = new WebSocket('http://127.0.0.1/test');
    socket.on('response', function(msg) {
        $('#log').append('<p>Received: ' + msg.data + '</p>');
    });
});