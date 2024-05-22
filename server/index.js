const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 3000 });

wss.on('connection', function connection(ws) {
    console.log('New client connected');

    ws.on('message', function incoming(message) {
        console.log('Received message: %s', message);

        // Broadcast the message to all connected clients except the sender
        wss.clients.forEach(function each(client) {
            if (client !== ws && client.readyState === WebSocket.OPEN) {
                client.send(`${message}`);
            }
        });
    });

    ws.on('close', function() {
        console.log('Client disconnected');
    });
});

console.log('WebSocket server is running on ws://localhost:3000');
