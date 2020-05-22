import React, { Component } from 'react'
import { observer } from 'mobx-react'
import { Button } from 'semantic-ui-react'

class CoreButton extends Component {
  render() {
    const { core, url, args } = this.props

    function onClick(e) {
      console.log('CALL ', url, '--', args)
      e.preventDefault();
      core.call(url, args).then((ret) => {
        console.log('call success', ret)
      }).catch((er) => { console.log('call fail', er) })
    }
    return (
      <Button {...this.props} onClick={onClick} />
    );
  }
}
export default observer(CoreButton)