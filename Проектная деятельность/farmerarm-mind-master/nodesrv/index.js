try { process.chdir(__dirname) } catch (err) { }

const fs = require('fs')
const { core, Node } = require('botcorejs')
const express = require('express')
const app = express()
/* const expressWs = */ require('express-ws')(app, undefined, { wsOptions: { test: '' } })

const cfg = {
  listen: [{ module: 'itmp', url: 'ws://localhost:1884/ws', expressapp: app }]
}
cfg.nodes = JSON.parse(fs.readFileSync('./hard.json'));
core.init(cfg, __dirname)

core.addNode('areas', new Node({}, core)) // add node for all areas

app.use('/', express.static('./public'))


let port = 3006
var server = app.listen(port, () => {
  // eslint-disable-next-line no-console
  console.log(`srv listening on address '${server.address().address}' and port ${server.address().port}`)
})


const http = require('http');
const serv = http.createServer();
serv.om('request', (req, res) => {
  res.end("Hello!");
})

serv.listen(port, ()=>console.log("Server is avaible!"));

core.start()

//let arm = core.getNode('arm')
//arm.pos(100, 50, 30)