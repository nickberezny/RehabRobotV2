import React from 'react'
import { connect } from 'react-redux'
import { withStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

import SetupPage from './Pages/SetupPage'
import DataPage from './Pages/DataPage'
import SettingsPage from './Pages/SettingsPage'

import io from "socket.io-client";
import { setValue } from "../src/actions";

let WIRELESS = 1

class WindowContent extends React.Component {

	constructor(props) {

	    super(props);
	    this.handleInfo = this.handleInfo.bind(this);
	    this.handleConfirm = this.handleConfirm.bind(this);

	    this.state = { 
	    	content: null,
	    	style: null,
	    	info: null, 
	    	home: null,
	    };
	}

	componentDidMount() {
	    var socketio = io();
	    socketio.on('message', this.handleMessage) 
	    socketio.on('CONFIRM', this.handleConfirm) 
	    socketio.on('INFO', this.handleInfo) 
	    console.log(socketio);
	    this.props.setValue('socket', socketio)	 

	}

	componentWillUnmount () {
	    console.log('unmount')
	    this.props.socket.close()
	}

	handleInfo = (message) => {
		var res = message.toString().split(',')
		var test= <div> Force Offset: {res[1]}, Total Length: {res[2]} </div>
		this.setState({ info: test });
		
		//this.forceUpdate();
	}

	handleConfirm = (message) => {
		//var test = <div>Back Home Complete</div> 
		this.setValue(message.toString(), 2 );
	}


	render() {

		if(this.props.menuOpen) {
	    	this.state.style = {
				transition: 'marginLeft 6',
				marginLeft: 255 ,
				padding: 20
			}
		}else{
			this.state.style = {
				transition: 'marginLeft 6',
				marginLeft: 0,
				padding: 20
			}
		}

		switch(this.props.activePage) {
			case 1:
				  if(this.props.user == 1) {this.state.content = <div style={this.state.style}> <SetupPage /> </div>}
				  if(this.props.user == 2) {this.state.content = <div style={this.state.style}> <SetupPage /> </div>}
				  break;
			case 2:
				  this.state.content = <div style={this.state.style}> <DataPage /> {this.state.home} {this.state.info}  </div>
				  break;
			case 3:
				this.state.content = <div style={this.state.style}> <SettingsPage /> </div>
				break;
			default:
				  this.state.content = <div style={this.state.style}> Page Load Failed </div>		
		}

		return (
			<div>
				{this.state.content}
			</div>
		)
	}
}


function mapStateToProps(state) {
	
	//map state variables to the component's state 
 	return {
 		activePage: state.activePage,
 		menuOpen: state.menuOpen,
 		socket: state.socket,
 		user: state.user ,
 		stage: state.stage,
 		exp: state.exp,
 		game: state.game,
 	}
}


export default connect(
  mapStateToProps,
  { setValue } //add importing action functions here
)(WindowContent);


