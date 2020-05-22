import React, { Component } from 'react'
import { observer } from 'mobx-react'
import { Checkbox } from 'semantic-ui-react'

class CoreSwitch extends Component {
  componentDidMount() {
    const { core, url } = this.props
    core.subscribe(url)
  }
  componentWillUnmount() {
    const { core, url } = this.props
    core.unsubscribe(url)
  }
  render() {
    const { core, url } = this.props

    function onChange(e) {
      let current = core.getter(url)
      console.log('ON CHANGE ', current, url, '--', e)
      e.preventDefault();
      core.call(url, core.getter(url) ? 0 : 1).then((ret) => {
        console.log('call success', ret)
      }).catch((er) => { console.log('call fail', er) })
    }
    return (
      <Checkbox {...this.props} slider checked={core.getter(url)} onChange={onChange} />
    );
  }
}
export default observer(CoreSwitch)