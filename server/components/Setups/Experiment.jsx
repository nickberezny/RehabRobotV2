import React from 'react';
import Button from '@material-ui/core/Button';

import Dropdown from '../Generic/Dropdown'
import InputTextExp from '../Generic/InputTextExp';
import { connect } from "react-redux";
import Grid from '@material-ui/core/Grid';
import InputAdornment from '@material-ui/core/InputAdornment';

class Experiment extends React.Component {
  
  constructor(props) {
    super(props);
  }


  render() {

    return (
      <div>
        <Grid container>
          
          <Grid item xs={4} style={{padding:10}}>
            <InputTextExp text="K" textValue={this.props.k_exp[0]} paramName='k_exp' stage={0} startAdornment="1" />
            <InputTextExp text="" textValue={this.props.k_exp[1]} paramName='k_exp' stage={1} startAdornment="2"/>
            <InputTextExp text="" textValue={this.props.k_exp[2]} paramName='k_exp' stage={2} startAdornment="3"/>
            <InputTextExp text="" textValue={this.props.k_exp[3]} paramName='k_exp' stage={3} startAdornment="4"/>
            <InputTextExp text="" textValue={this.props.k_exp[4]} paramName='k_exp' stage={4} startAdornment="5"/>
            <InputTextExp text="" textValue={this.props.k_exp[5]} paramName='k_exp' stage={5} startAdornment="6"/>
            <InputTextExp text="" textValue={this.props.k_exp[6]} paramName='k_exp' stage={6}  startAdornment="7"/>
            <InputTextExp text="" textValue={this.props.k_exp[7]} paramName='k_exp' stage={7} startAdornment="8"/>
            <InputTextExp text="" textValue={this.props.k_exp[8]} paramName='k_exp' stage={8} startAdornment="9"/>
            <InputTextExp text="" textValue={this.props.k_exp[9]} paramName='k_exp' stage={9} startAdornment="10"/>
          </Grid>
          <Grid item xs={4} style={{padding:10}}>
            <InputTextExp text="B" textValue={this.props.b_exp[0]} paramName='b_exp' stage={0} />
            <InputTextExp text="" textValue={this.props.b_exp[1]} paramName='b_exp' stage={1} />
            <InputTextExp text="" textValue={this.props.b_exp[2]} paramName='b_exp' stage={2} />
            <InputTextExp text="" textValue={this.props.b_exp[3]} paramName='b_exp' stage={3} />
            <InputTextExp text="" textValue={this.props.b_exp[4]} paramName='b_exp' stage={4} />
            <InputTextExp text="" textValue={this.props.b_exp[5]} paramName='b_exp' stage={5} />
            <InputTextExp text="" textValue={this.props.b_exp[6]} paramName='b_exp' stage={6} />
            <InputTextExp text="" textValue={this.props.b_exp[7]} paramName='b_exp' stage={7} />
            <InputTextExp text="" textValue={this.props.b_exp[8]} paramName='b_exp' stage={8} />
            <InputTextExp text="" textValue={this.props.b_exp[9]} paramName='b_exp' stage={9} />
          </Grid>
        </Grid>

      </div>
    )
  }
}

function mapStateToProps(state) {
  return {
    P: state.P,
    D: state.D,
    xmax: state.xmax,
    vmax: state.vmax,
    K: state.K,
    B: state.B,
    M: state.M,
    k_exp: state.k_exp,
    b_exp: state.b_exp
  }
}

export default connect(
  mapStateToProps,
  {}
)(Experiment);

//