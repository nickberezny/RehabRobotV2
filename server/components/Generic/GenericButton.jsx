import React from 'react';
import Button from '@material-ui/core/Button';

import { connect } from "react-redux";
import {setValue} from "../../src/actions";

import io from "socket.io-client";

class GenericButton extends React.Component {
  
  constructor(props) {
    super(props);
    this.run = this.run.bind(this);

    this.state =
    {
      color: 'secondary'
    }
  }

  run() {

    var dataToSend = 0;

    if(this.props.err == 0 && this.props.response == 0){ 
      this.props.setValue('response', 1)
      switch(this.props.message)
      {
        case "RECORD":
          if(this.props.RUN == 0)
          {
            dataToSend = this.props.message
            this.props.setValue(this.props.message, 1)
            this.setState({ color: 'primary' });
          }
        
        case "SET":
          if(this.props.RUN == 0)
          {
            dataToSend = 'SET_exp' + this.props.exp + '_' + '_game' + this.props.game + '_'
            this.props.setValue(this.props.message, 1)
            this.setState({ color: 'primary' });
          }
          break;

        case "HOME":
          if(this.props.RUN == 0)
          {
            if(window.confirm('Are you sure you are ready to home the robot?'))
            {
              dataToSend = this.props.message
              this.props.setValue(this.props.message, 1)
              this.setState({ color: 'primary' });

            }
            
          }
          break;

        case "CAL":
          if(this.props.RUN == 0)
          {
            dataToSend = this.props.message
            this.props.setValue(this.props.message, 1)
            this.setState({ color: 'primary' });
          }
          break;

        case "RUN":
          if(this.props.SET == 2 && this.props.HOME == 2 && this.props.CAL == 2)
          {
            if(window.confirm('Are you sure you are ready to run the robot?'))
            {
              dataToSend = this.props.message
              this.props.setValue(this.props.message, 1)
              this.setState({ color: 'primary' });
            }
            
          }
          break;
    }
    
    if(dataToSend != 0) this.props.socket.emit('MESSAGE',dataToSend);
  }
  
  }

  render() {

    return (
      <div style={{padding: 12}}>
      <Button variant="contained" color={this.state.color} onClick={this.run} disabled={this.props.disabled}>
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
    CAL: state.CAL,
    err: state.param_error,
    response: state.response
  }
}

export default connect(
  mapStateToProps,
  { setValue }
)(GenericButton);

//