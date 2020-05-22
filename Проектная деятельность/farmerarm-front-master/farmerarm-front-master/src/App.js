import React, { Component } from 'react'
import './index.css'
import './styles/semantic-extend.css'
import CorePosPanel from './components/CorePosPanel'


class App extends Component {

  render() {
    const { core } = this.props
    return (
      <div className='container'>
        <CorePosPanel core={core} url={`itmpws://${window.location.hostname}:3006/arm`} />
        {/* <CorePosPanel core={core} url={`itmpws://10.0.0.2:3006/arm`} /> */}
      </div>
    )
  }
}

export default App;
