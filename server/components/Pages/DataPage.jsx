import React from 'react'
import { connect } from 'react-redux'

class DataPage extends React.Component {

	constructor(props) {
	    super(props);
	}

	render() {
		return (
			<div> Data Page</div>
		)
	}
}


function mapStateToProps(state) {
  return {}
}

export default connect(
  mapStateToProps,
  {}
)(DataPage);
