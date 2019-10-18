const app = require('express')()
const server =  require('http').Server(app)
var io = require("socket.io")(server);
const next = require('next')
var net = require('net');

var tcp_port = 1337;
var ui_port = 3000;

var TEST = 1;


/**********************************************************************
            Create Web Server
***********************************************************************/

const dev = process.env.NODE_ENV !== 'production'
const nextApp = next({ dev })
const nextHandler = nextApp.getRequestHandler()


/**********************************************************************
            Connect to TCP Socket (Controller Process)
***********************************************************************/

if(TEST) {
//tcp socket client
  var client = new net.Socket();
  client.connect(tcp_port, '127.0.0.1', function() {
    console.log('Connected');
  })

  client.on('data', function(data){

    var res = data.toString().split("_");

    if(data.toString() == 'SET') { io.emit('CONFIRM', 'SET'; }
    else if(data.toString() == 'RUN') { io.emit('CONFIRM', 'RUN'; }
    else if(res[0] == 'HOME') { io.emit('CONFIRM', 'HOME'; }
    else if(res[0] == 'CAL') { io.emit('CONFIRM', 'CAL'; }
    else if(data.toString() == 'TRAJ') { io.emit('CONFIRM', 'TRAJ'; }
    else if(data.toString() == 'END') { io.emit('CONFIRM', 'END'; }
    if(res[0] == 'INFO') {
      io.emit('INFO',data.toString());
      console.log('INFO' + res[1] + '  ' + data)
    }
  });



}


/**********************************************************************
            Websocket Server 
***********************************************************************/

// socket.io server
io.on('connection', socket => {
  console.log("server socket connect")
  //socket.broadcast.emit('message', 'world')

  socket.on('MESSAGE', function (data) {
    client.write(data)
    console.log(data);
  });

})

/**********************************************************************
            Start Server
***********************************************************************/

nextApp.prepare().then(() => {

  app.get('*', (req, res) => {
    return nextHandler(req, res)
  })

  server.listen(ui_port, (err) => {
    if (err) throw err
    console.log('> Ready on http://localhost:3000')
  })

})
