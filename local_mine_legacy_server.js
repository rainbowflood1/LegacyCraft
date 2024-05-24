const express = require('express')
const app = express()
const port = 3000

var users = {};

app.get('/addplayer/:username', (req, res) => {
    users[req.params.username] = {"x": 0, "y": 0, "z": 0};
    res.send(users);
})

app.get('/setposition/:username/:x/:y/:z', (req, res) => {
    var x = req.params.x;
    var y = req.params.y;
    var z = req.params.z;
    users[req.params.username]["x"] = Number(x);
    users[req.params.username]["y"] = Number(y);
    users[req.params.username]["z"] = Number(z);
    res.send(users);
})

app.get('/getdata', (req, res) => {
    res.send(JSON.stringify(users));
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
})
