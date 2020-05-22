const { core, Node } = require('botcorejs')

const timetick = 100
class Servo extends Node {
  constructor(cfg) {
    super(cfg, core)

    this.addVar('pos', { type: 'number', label: '', units: 'radians' })
    this.addVar('posmin', { type: 'number', label: 'min', units: 'radians', default: -Math.PI, store: true })
    this.addVar('posmax', { type: 'number', label: 'max', units: 'radians', default: Math.PI, store: true })
    this.addVar('radianstous', { type: 'number', label: '', units: 'us/radians', default: 1000 / Math.PI, store: true })
    this.addVar('zerous', { type: 'number', label: '', units: 'us', default: 1500, store: true })

    this.populate(['setpos'])

    this.pos = 0;
    this.speed = 0;
    this.targetpos = 0;
    this.set('pos', this.pos)
  }

  start() {
    this.tm = setInterval(() => {
      if (this.pos != this.targetpos) {
        if (Math.abs(this.pos - this.targetpos) <= Math.abs(this.speed)) this.pos = this.targetpos
        else this.pos += this.speed
        this.set('pos', this.pos)
        //let radianstous = this.get('radianstous')
        //let zerous = this.get('zerous')
        //settime(zerous + radianstous * this.pos)
      }
    }, timetick);
  }

  setpos(newpos, time) {
    let min = this.get('posmin')
    let max = this.get('posmax')
    if (newpos < min) newpos = min
    if (newpos > max) newpos = max

    this.targetpos = +newpos;
    this.speed = (this.targetpos - this.pos) * timetick / (time * 1000);
  }
}
module.exports = Servo