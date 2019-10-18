import React from 'react'
import { connect } from 'react-redux'

import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import MenuIcon from '@material-ui/icons/Menu';
import PowerSettingsNew from '@material-ui/icons/PowerSettingsNew';


import { toggleDrawer } from "../src/actions";



class Topbar extends React.Component {

  constructor(props) {
      super(props);
      this.state = { 
          menuIconStyle: {marginLeft: 6, marginRight: 2},
          appbarStyle: {display: 'flex', flexGrow: 0} 
        };
  }


  render() {
    return (
      <div>
      <AppBar position="fixed">
          <Toolbar>
            <IconButton color="inherit" aria-label="Menu" onClick={this.props.toggleDrawer}>
                <MenuIcon />
            </IconButton>
            <Typography variant="title" color="inherit" style={{flex:1}}>
                  {this.props.title}
            </Typography>
            <div>
            <IconButton color="inherit">
              <PowerSettingsNew />
            </IconButton>
            </div>
          </Toolbar>
      </AppBar>
      </div>
    )

  }
}


function mapStateToProps(state) {
  
  //map state variables to the component's state 
  return {}
}


export default connect(
  mapStateToProps,
  {toggleDrawer} //add importing action functions here
)(Topbar);


