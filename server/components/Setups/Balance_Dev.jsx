import React from 'react';
import Button from '@material-ui/core/Button';

import InputText from '../Generic/InputText';
import { connect } from "react-redux";

class FollowVel_Dev extends React.Component {
  
  constructor(props) {
    super(props);
  }


  render() {

    return (
      <div> 
        <InputText text="Max Position" textValue={this.props.xmax} paramName='xmax' />
        <InputText text="Contact Stiffness" textValue={this.props.k_contact} paramName='k_contact' />
        <InputText text="Contact Object mass" textValue={this.props.m_contact} paramName='m_contact' />
        <InputText text="Admittance B" textValue={this.props.B} paramName='B' />
        <InputText text="Admittance M" textValue={this.props.M} paramName='M' />
        <InputText text="P Gain" textValue={this.props.P} paramName='P' />
        <InputText text="D Gain" textValue={this.props.D} paramName='D' />
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
    m_contact: state.m_contact,
    k_contact: state.k_contact,
  }
}

export default connect(
  mapStateToProps,
  {}
)(FollowVel_Dev);

//