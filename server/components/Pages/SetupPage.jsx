import React from 'react'
import { connect } from 'react-redux'

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
	    };  
	}

	render() {

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
			      		<Dropdown text="Game" id="game" value={this.props.game} select1="Follow Trajectory" select2 = "Racing" select3 = "Balance" />
			      		<br/>
			      		<br/>
			      		<Typography variant="display1" gutterBottom>
				    			Trajectory  
			      		</Typography>
			      		<Dropdown text="Trajectory Profile" id="game" value={this.props.game} select1="Standard Trajectory" select2 = "Custom Trajectory 1" select3 = "Custom Trajectory 2" />

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
					    {this.props.SET}
					    <GenericButton text="Home" message="HOME" />
					    {this.props.HOME} 
					    <GenericButton text="Run" message="RUN" /> 
					    {this.props.RUN}
					    <br/>
			      		<br/>
					    <Typography variant="display1" gutterBottom>
				    		Robot Status
			      		</Typography>
			      		Status:
			      		<br/> 
			      		Total Length: 
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
  	game: state.game,
  	user: state.user,
  	RUN: state.RUN,
  	HOME: state.HOME,
  	SET: state.SET

  }
}

export default connect(
  mapStateToProps,
  {}
)(SetupPage);
