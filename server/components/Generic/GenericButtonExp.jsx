import React from 'react';
import Button from '@material-ui/core/Button';

import { connect } from "react-redux";
import {setValue} from "../../src/actions";

import io from "socket.io-client";

class GenericButtonExp extends React.Component {
  
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
            dataToSend = 'SETEXP_time' + this.props.time + '_trials_' + this.props.trials 
              + '_K1_' + this.props.k_exp[0] + '_B1_' + this.props.b_exp[0]
              + '_K2_' + this.props.k_exp[1] + '_B2_' + this.props.b_exp[1]
              + '_K3_' + this.props.k_exp[2] + '_B3_' + this.props.b_exp[2]
              + '_K4_' + this.props.k_exp[3] + '_B4_' + this.props.b_exp[3]
              + '_K5_' + this.props.k_exp[4] + '_B5_' + this.props.b_exp[4]
              + '_K6_' + this.props.k_exp[5] + '_B6_' + this.props.b_exp[5]
              + '_K7_' + this.props.k_exp[6] + '_B7_' + this.props.b_exp[6]
              + '_K8_' + this.props.k_exp[7] + '_B8_' + this.props.b_exp[7]
              + '_K9_' + this.props.k_exp[8] + '_B9_' + this.props.b_exp[8]
              + '_K10_' + this.props.k_exp[9] + '_B10_' + this.props.b_exp[9]

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
    response: state.response,
    game: state.game,
    exp: state.exp,
    k_exp: state.k_exp,
    b_exp: state.b_exp,
    time: state.time,
    trials: state.trials
  }
}

export default connect(
  mapStateToProps,
  { setValue }
)(GenericButtonExp);

//