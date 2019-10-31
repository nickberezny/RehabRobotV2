import React from 'react'
import { connect } from 'react-redux'

import {setValue} from '../../src/actions'

import GenericButton from '../Generic/GenericButton'
import InputText from '../Generic/InputText'
import Dropdown from '../Generic/Dropdown'

import FollowTraj_Dev from '../Setups/FollowTraj_Dev'
import FollowVel_Dev from '../Setups/FollowVel_Dev'
import Balance_Dev from '../Setups/Balance_Dev'
import FollowTraj from '../Setups/FollowTraj'
import FollowVel from '../Setups/FollowVel'
import Balance from '../Setups/Balance'


import Grid from '@material-ui/core/Grid';

import { withStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

class SetupPage extends React.Component {

	constructor(props) {
	    super(props);
	    this.state = { 
	    	contents: null,
	    	color: 'secondary',
	    	button: null,
	    	disabled: false
	    };  
	}

	render() {

		if(this.props.SET>0) this.state.disabled = true
		if(this.props.status == 'READY TO RUN') this.state.color = 'primary' ;
		if(this.props.response == 1) this.props.setValue('status', 'WAITING FOR RESPONSE...');

		if(this.props.traj == 1)
		{
			var temp1 = null
			
		}else if(this.props.traj == 2)
		{ 
			var temp1 = <InputText text="Max Velocity" textValue={this.props.vmax} paramName='vmax' min={1} max={2} />
		}else if(this.props.traj == 3)
		{
			var temp1 = <GenericButton disabled={this.state.disabled} text="Record" message="RECORD" /> 
		}

		this.state.button =  temp1;
		

		if(this.props.user == 1){
			switch(this.props.game) {
				case 1: 
					this.state.contents = <div> <FollowTraj_Dev /> </div> 
					break;
				case 2:
					this.state.contents = <div> <FollowVel_Dev /> </div> 
					break;
				case 3:
					this.state.contents = <div> <Balance_Dev /> </div> 
					break;
				default: 
					this.state.contents = <div> Select a Game </div> 
			}
		} 
		else if(this.props.user == 2){
			switch(this.props.game) {
				case 1: 
					this.state.contents = <div> <FollowTraj /> </div> 
					break;
				case 2:
					this.state.contents = <div> <FollowVel /> </div> 
					break;
				case 3:
					this.state.contents = <div> <Balance /> </div> 
					break;
				default: 
					this.state.contents = <div> Select a Game </div> 
			}
		}



		return (

			<div>
		      	<div>
		      	<Grid container>
			      	<Grid item xs>
			      		<Typography variant="display1" gutterBottom>
			    			Game Type  
		      			</Typography>
			      		<Dropdown disabled={this.state.disabled} text="Game" id="game" value={this.props.game} select1="Follow Trajectory" select2 = "Racing" select3 = "Balance" />
			      		<br/>
			      		<br/>
			      		<Typography variant="display1" gutterBottom>
				    			Trajectory  
			      		</Typography>
			      		<Dropdown disabled={this.state.disabled} text="Trajectory Profile" id="traj" value={this.props.traj} select1="Standard Trajectory" select2 = "Standard Trajectory 2" select3 = "Custom Trajectory" />
			      		{this.state.button}
			      	</Grid>
			      	<Grid item xs>
				      	<Typography variant="display1" gutterBottom>
				    			Parameters 
			      		</Typography>
				      	{this.state.contents}
			      	</Grid>
			      	<Grid item xs>
			      		<Typography variant="display1" gutterBottom>
				    		Send Commands
			      		</Typography>
					    <GenericButton text="Set" message="SET" /> 
					    <GenericButton text="Home" message="HOME" />
					    <GenericButton text="Calibrate" message="CAL" /> 
					    <GenericButton text="Run" message="RUN" />  
					    <GenericButton text="Stop" message="END" disabled={!this.props.RUN} /> 
					    <br/>
					    <br/>
					    <Typography variant="display1" gutterBottom>
				    		Status
			      		</Typography>
			      		<Typography color='secondary'>
			      		{this.props.status}
			      		</Typography>
			      		<Typography>
			      		Total Length (cm): {this.props.force_offset}
			      		</Typography>
			      		<Typography>
			      		Force Offset (lb): {this.props.total_length}
			      		</Typography>

					</Grid>
					<Grid item xs>

					</Grid>
				</Grid>
				</div>
			</div>
	
		);
	}
}


function mapStateToProps(state) {
  return {
  	P: state.P,
  	D: state.D,
  	xdes: state.xdes,
  	K: state.K,
  	B: state.B,
  	M: state.M,
  	vmax: state.vmax,
  	game: state.game,
  	user: state.user,
  	RUN: state.RUN,
  	HOME: state.HOME,
  	CAL: state.CAL,
  	SET: state.SET,
  	traj: state.traj,
  	err: state.param_error,
  	status: state.status,
  	force_offset: state.force_offset,
  	total_length: state.total_length,
  	response: state.response
  }
}

export default connect(
  mapStateToProps,
  {setValue}
)(SetupPage);
