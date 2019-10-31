import React from 'react';
import Button from '@material-ui/core/Button';

import InputText from '../Generic/InputText';
import { connect } from "react-redux";

class FollowTraj_Dev extends React.Component {
  
  constructor(props) {
    super(props);
  }


  render() {

    return (
      <div> 
        <InputText text="Admittance K" textValue={this.props.K} paramName='K' min={1} max={2} />
        <InputText text="Admittance B" textValue={this.props.B} paramName='B' min={1} max={2} />
        <InputText text="Admittance M" textValue={this.props.M} paramName='M' min={1} max={2} />
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
  }
}

export default connect(
  mapStateToProps,
  {}
)(FollowTraj_Dev);

//