/*
 * action types
 */
export const CHANGE_TEXT = 'CHANGE_TEXT'
export const TOGGLE_DRAWER = 'TOGGLE_DRAWER'
export const SET_VALUE = 'SET_VALUE'


/*
 * action creators
 */

export function changeText() {
  return { type: CHANGE_TEXT, text: "Click!"}
}

export function toggleDrawer() {
  return { type: TOGGLE_DRAWER}
}


export function setValue(id, value) {
  return {
  type: SET_VALUE,
  id,
  value }
}