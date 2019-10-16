import React from 'react'
import {curveCatmullRom} from 'd3-shape';
import { connect } from 'react-redux'

//import "react-vis/dist/style.css";
import Head from 'next/head';

import data_test from '../../data/data_test'


import {XYPlot, XAxis, YAxis, ChartLabel, HorizontalGridLines, VerticalGridLines, LineSeries, LineSeriesCanvas} from 'react-vis';
/* import XAxis from 'react-vis/dist/index.js';
import YAxis from 'react-vis/dist/index.js';
import ChartLabel from 'react-vis/dist/index.js';
import HorizontalGridLines from 'react-vis/dist/index.js';
import VerticalGridLines from 'react-vis/dist/index.js';
import LineSeries from 'react-vis/dist/index.js';
import LineSeriesCanvas from 'react-vis/dist/index.js'; */


class DataPage extends React.Component {

	constructor(props) {
	    super(props);
	    console.log(data_test.data)
	}

	render() {
		
    	const Line = LineSeriesCanvas;

		return (
			
			<div> 
			<Head>
				<link
                    href="https://unpkg.com/react-vis/dist/style.css"
                    rel="stylesheet"
                />
            </Head>
			  <XYPlot width={300} height={300}>
		          <HorizontalGridLines />
		          <VerticalGridLines />
		          <XAxis />
		          <YAxis />
		          <Line
		            className="first-series"
		            data = {data_test.data}  //{[{x: 1, y: 3}, {x: 2, y: 5}, {x: 3, y: 15}, {x: 4, y: 12}]}
		          />
		           <LineSeries
			        className="third-series"
			        curve={'curveMonotoneX'}
			        data={[{x: 1, y: 10}, {x: 2, y: 4}, {x: 3, y: 2}, {x: 4, y: 15}]}
			        strokeDasharray="7, 3"
			      />
		          <ChartLabel 
		            text="X Axis"
		            className="alt-x-label"
		            includeMargin={false}
		            xPercent={0.025}
		            yPercent={1.01}
		            />

		          <ChartLabel 
		            text="Y Axis"
		            className="alt-y-label"
		            includeMargin={false}
		            xPercent={0.06}
		            yPercent={0.06}
		            style={{
		              transform: 'rotate(-90)',
		              textAnchor: 'end'
		            }}
		            />
	        </XYPlot>
			</div>
		);
	}
}


function mapStateToProps(state) {
  return {}
}

export default connect(
  mapStateToProps,
  {}
)(DataPage);
