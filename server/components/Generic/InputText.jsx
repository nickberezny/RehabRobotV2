import React, { Component } from 'react'
import { connect } from 'react-redux'

import Grid from '@material-ui/core/Grid';
import TextField from '@material-ui/core/TextField';
import { setValue } from "../../src/actions";
import { paramCounter } from "../../src/actions";

class InputText extends React.Component {

	constructor(props) {
	    super(props);
	    this.state = {
	    	err: false
	    }
	}

	onInputChange = (event) => {
        console.log("New parameter")
        let name = this.props.paramName
        let val = event.target.value
        if(val>this.props.max || val<this.props.min)
        {
        	if(this.state.err == false) this.props.paramCounter(1)
        	this.setState({ err: true });
        	
        }else
        {
        	if(this.state.err == true) this.props.paramCounter(-1)
        	this.setState({ err: false });
        }
        this.props.setValue(name,event.target.value);
        
    }

	render() {
		return (
			<div>
	            <TextField
					id="TextInput"
					label={this.props.text}
					value={this.props.textValue}
					onChange={this.onInputChange}
					margin="normal"
					error={this.state.err}
		        />
		    </div>

		)
	}
}


function mapStateToProps(state) {
  return {
  }
}

export default connect(
  mapStateToProps,
  { setValue,
    paramCounter }
)(InputText);