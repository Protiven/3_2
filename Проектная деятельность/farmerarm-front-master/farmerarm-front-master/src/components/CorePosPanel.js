import React, { Component, createRef } from 'react'
import { observer } from 'mobx-react'
import { Button, Input } from 'semantic-ui-react'

class CorePosPanel extends Component {
  componentDidMount() {
    const { core, url } = this.props
    core.subscribe(url + '/posx')
    core.subscribe(url + '/posy')
    core.subscribe(url + '/posz')
  }
  componentWillUnmount() {
    const { core, url } = this.props
    core.unsubscribe(url + '/posx')
    core.unsubscribe(url + '/posy')
    core.unsubscribe(url + '/posz')
  }
  targetx = 0
  inputXRef = createRef()
  inputYRef = createRef()
  inputZRef = createRef()
  handleClick = (e) => {
    const { core, url } = this.props
    let tx = this.inputXRef.current.inputRef.current.value
    let ty = this.inputYRef.current.inputRef.current.value
    let tz = this.inputZRef.current.inputRef.current.value
    let args = [tx, ty, tz]
    e.preventDefault();
    core.call(url + '/pos', args).then((ret) => {
      console.log('call success', ret)
    }).catch((er) => { console.log('call fail', er) })
  }
  render() {
    const { core, url } = this.props
    let cxpos = core.getter(url + '/posx')
    let cypos = core.getter(url + '/posy')
    let czpos = core.getter(url + '/posz')
    let online = core.state(url)

    return (
      <div style={{ position: 'relative', height: '50%', width: '100%', background: 'white' }} onMouseMove={(e, pos) => console.log('move', e.clientX, e.clientY)} onClick={(e) => console.log('click', e.clientX, e.clientY)}>
        {online}
        <Input ref={this.inputXRef} onChange={(e, data) => { this.targetx = data.value }} placeholder='x' />
        <Input ref={this.inputYRef} placeholder='y' />
        <Input ref={this.inputZRef} placeholder='z' />
        <Button content='focus' onClick={this.handleClick} />
        {cxpos},
        {cypos},
        {czpos}
      </div>
    );
  }
}
export default observer(CorePosPanel)