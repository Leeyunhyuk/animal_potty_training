const express = require('express');
const bodyparser = require('body-parser');
const admin = require('firebase-admin');
const fs = require('fs');

var serviceAccount = require("./message-5c0a2-firebase-adminsdk-5l4ts-03f8e9eaa6.json");
let registrationToken = "put your device token";

var SerialPort = require('serialport').SerialPort;

var SerialPort = require('serialport');
var port = new SerialPort('/dev/ttyNode', {
  baudRate: 115200
});

const app = express();
app.use(express.static('app'));
app.use(bodyparser.json())
const server = app.listen(3000, function () {
    const host = server.address().address
    const port = server.address().port
    console.log("Example app listening at http://%s:%s", host, port)
});

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://javamsgs-5da73-default-rtdb.firebaseio.com"
  });

  port.on('data', (req, res) => {
    let payload = {
        notification: {
            title: "Potty pad is dirty!",
            body: "Please change potty pad!"
        }
    };
    admin.messaging().sendToDevice(registrationToken, payload)
  
})

app.use(express.static(__dirname + '/public'));
