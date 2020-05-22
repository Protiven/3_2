const { core, Node } = require('botcorejs')

const timetick = 100
class Rmd extends Node {
  constructor(cfg) {
    super(cfg, core)

    this.addVar('pos', { type: 'number', label: '', units: 'mm' })
    this.addVar('posmin', { type: 'number', label: 'min', units: 'mm', default: 0, store: true })
    this.addVar('posmax', { type: 'number', label: 'max', units: 'mm', default: 800, store: true })

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
module.exports = Rmd