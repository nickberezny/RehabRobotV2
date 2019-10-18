import { combineReducers } from 'redux'
import {
  TOGGLE_DRAWER,
  SET_VALUE,
  PARAM_COUNTER,
} from './actions'

var initState = {user: 1, menuOpen: false, activePage: 1, socket: null, param_error: 0,
                    P: 2, D:1, K:10, B:0.5, M:0.09, game:1, xmax: 100.0, vmax: 0.0,  
                    k_contact: 0.0, m_contact: 0.0, traj: 1, SET: 0, HOME: 0, RUN: 0, user_data: 1
                };

function todoApp(state = initState, action) {
  switch (action.type) {
    case TOGGLE_DRAWER:
      return Object.assign({}, state, {
        menuOpen: !state.menuOpen,
      })
      break;

    case SET_VALUE:
      return Object.assign({}, state, {
        [action.id]: action.value,
      })
      break;

    case PARAM_COUNTER:
      return Object.assign({}, state, {
        param_error: state.param_error + action.value
      })
      break;
      
    default:
      return state
  }
}

export default todoApp


