import React from 'react';
import Button from '@material-ui/core/Button';

import { connect } from "react-redux";
import {setValue} from "../../src/actions";

import io from "socket.io-client";

class GenericButton extends React.Component {
  
  constructor(props) {
    super(props);
    this.run = this.run.bind(this);
  }

  run() {

    var dataToSend = 0;

    switch(this.props.message)
    {
      
      case "SET":
        if(this.props.HOME == 0 && this.props.RUN == 0)
        {
          dataToSend = 'SET_exp' + this.props.exp + '_' + '_game' + this.props.game + '_'
          this.props.setValue(this.props.message, 1)
        }
        break;

      case "HOME":
        if(this.props.SET == 1 && this.props.RUN == 0)
        {
          dataToSend = this.props.message
          this.props.setValue(this.props.message, 1)
        }
        break;

      case "RUN":
        if(this.props.SET == 1 && this.props.HOME == 1)
        {
          dataToSend = this.props.message
          this.props.setValue(this.props.message, 1)
        }
        break;
    }
    
    if(dataToSend != 0) this.props.socket.emit('MESSAGE',dataToSend);

  
  }

  render() {

    return (
      <div style={{padding: 12}}>
      <Button variant="contained" color="primary" onClick={this.run}>
        {this.props.text}
      </Button>
      </div>
    )
  }
}

function mapStateToProps(state) {
  return {
    socket: state.socket,
    SET: state.SET,
    HOME: state.HOME,
    RUN: state.RUN,
  }
}

export default connect(
  mapStateToProps,
  { setValue }
)(GenericButton);

//