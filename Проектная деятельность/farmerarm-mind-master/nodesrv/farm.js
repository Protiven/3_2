const { core, Node } = require('botcorejs')

const timetick = 100
class Arm extends Node {
  constructor(cfg) {
    super(cfg, core)

    this.addVar('posx', { type: 'number', label: 'X', units: 'mm' })
    this.addVar('posy', { type: 'number', label: 'Y', units: 'mm' })
    this.addVar('posz', { type: 'number', label: 'Z', units: 'mm' })

    this.servo1 = core.getNode(cfg.servo1)
    this.servo1.subscribe('pos', (_, value) => {
      this.r1 = value
      this.updateyz()
    })
    this.servo2 = core.getNode(cfg.servo2)
    this.servo2.subscribe('pos', (_, value) => {
      this.r2 = value
      this.updateyz()
    })
    this.rail = core.getNode(cfg.rail)
    this.rail.subscribe('pos', (_, value) => {
      this.x = value
      this.set('posx', value)
    })

    this.populate(['pos'])

    this.L1 = 100; // length of first rod (from start point to the first joint)
    this.L2 = 100; // length of second rod (from first joint to the end effector)

    this.x = 0;
    this.r1 = 0;
    this.r2 = 0;
    const { y, z } = this.direct(this.r1, this.r2)
    this.set('posx', this.x)
    this.set('posy', y)
    this.set('posz', z)
  }

  updateyz() {
    const { y, z } = this.direct(this.r1, this.r2)
    this.set('posy', y)
    this.set('posz', z)
  }

  start() {
  }

  pos(x, y, z, time) { // coordinates in mm time in sec
    if (time === undefined) {
      let curx = this.get('posx')
      let cury = this.get('posy')
      let curz = this.get('posz')
      let d = Math.sqrt((curx - x) * (curx - x) + (cury - y) * (cury - y) + (curz - z) * (curz - z))
      time = d / 50
    }

    const { alpha, beta } = this.inverse(y, z)
    if (isNaN(alpha) || isNaN(beta))
      return false
    this.servo1.setpos(+alpha, time)
    this.servo2.setpos(+beta, time)
    this.rail.setpos(+x, time)
    /*    console.log(x, r1, r2)
        let st = core.getValue('rmd/state')
        if (st === 'ready' || st === 'home') core.getNode('rmd').to(Math.round(x), 2000, 2000)
        if (st === 'needhome') core.getNode('rmd').home(1000, 5000)
    
        let s = core.getNode('servo1')
        s.setpos(Math.round(r1), 5)
        let s2 = core.getNode('servo2')
        s2.setpos(Math.round(r2), 5)*/
    return true
  }

  inverse(y, z) {// compute alpha, and beta from y,z
    let d = Math.sqrt(y * y + z * z)
    if (d > this.L1 + this.L2 || d < Math.abs(this.L1 - this.L2)) return { alpha: NaN, beta: NaN } // imposible to solve
    let a = (this.L1 * this.L1 - this.L2 * this.L2 + d * d) / (2 * d)
    let h = Math.sqrt(this.L1 * this.L1 - a * a)
    let ymiddle = a * y / d
    let zmiddle = a * z / d

    let y1 = ymiddle - h * z / d //coordinate of first joint (up position)
    let z1 = zmiddle + h * y / d

    let y12 = ymiddle + h * z / d //coordinate of first joint (down position)
    let z12 = zmiddle - h * y / d

    let alpha = Math.atan2(z1, y1)
    let beta = Math.atan2(z - z1, y - y1) - alpha
    if (beta > Math.PI) beta -= 2 * Math.PI
    if (beta < -Math.PI) beta += 2 * Math.PI
    return { alpha, beta }
  }

  direct(alpha, beta) {
    let y1 = this.L1 * Math.cos(alpha)
    let z1 = this.L1 * Math.sin(alpha)
    let beta2 = beta + alpha
    let y = y1 + this.L1 * Math.cos(beta2)
    let z = z1 + this.L1 * Math.sin(beta2)
    return { y, z }
  }

}
module.exports = Arm