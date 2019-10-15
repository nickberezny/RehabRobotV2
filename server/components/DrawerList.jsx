import React from 'react';
import { connect } from 'react-redux'
import { setValue } from "../src/actions";

import { withStyles } from '@material-ui/core/styles';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import Divider from '@material-ui/core/Divider';
import BuildIcon from '@material-ui/icons/Build';
import MoviesIcon from '@material-ui/icons/Movie';
import ChartIcon from '@material-ui/icons/InsertChartOutlined';
import SettingsIcon from '@material-ui/icons/Settings';


const ListStyles = {
    width: 230,
    maxWidth: 360,
}

class DrawerList extends React.Component {

  constructor(props) {
      super(props);
      this.pageOne = this.pageOne.bind(this);
      this.pageTwo = this.pageTwo.bind(this);
      this.pageThree = this.pageThree.bind(this);
  }

  pageOne() {
    this.props.setValue('activePage',1);
  }

  pageTwo() {
    this.props.setValue('activePage',2);
  }

  pageThree() {
    this.props.setValue('activePage',3);
  }


  render() {
    return (
      <div style={ListStyles}>
        <List component="nav">
          <ListItem button onClick={this.pageOne}>
            <ListItemIcon>
              <BuildIcon />
            </ListItemIcon>
            <ListItemText primary="Setup" />
          </ListItem>
          <ListItem button onClick={this.pageTwo}>
            <ListItemIcon>
              <ChartIcon />
            </ListItemIcon>
            <ListItemText primary="Data" />
          </ListItem>
          <ListItem button onClick={this.pageThree}>
            <ListItemIcon>
              <SettingsIcon />
            </ListItemIcon>
            <ListItemText primary="Settings" />
          </ListItem>
        </List>
      </div>
    )
  }
}

function mapStateToProps(state) {
  return {}
}

export default connect(
  mapStateToProps,
  { setValue }
)(DrawerList);




