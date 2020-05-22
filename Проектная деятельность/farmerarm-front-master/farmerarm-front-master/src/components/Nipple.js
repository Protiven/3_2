import React, { Component } from "react"
import PropTypes from "prop-types"
import { throttle } from 'lodash'
import ReactNipple from "react-nipple"
import "react-nipple/lib/styles.css"

export default class Nipple extends Component {
  static propTypes = {
    title: PropTypes.string,
    width: PropTypes.any,
    height: PropTypes.any,
    options: PropTypes.object
  }
  state = {
    data: undefined,
    started: false
  }

  handleMove = throttle((position) => {
    const { core, url } = this.props
    core.emit(url, { x: position.x, y: position.y })
  }, 100)

  handleEnd = () => {
    this.handleMove({ x: 0, y: 0 })
  }
  handleStart = (evt, data) => {
    console.log(this)
    this.setState({ started: true })
    this.setState({ data });
  }
  render() {
    const { options, position } = this.props
    return (
      <>
        {!this.state.started && <span className={`tap ${position}`}>TAP TO JOYSTICK</span>}
        <ReactNipple
          className={`joystick ${position}`}
          options={options}
          onStart={this.handleStart}
          onEnd={this.handleEnd}
          onMove={(evt, data) => this.handleMove(data.instance.frontPosition)}
        />

      </>
    )
  }


}
