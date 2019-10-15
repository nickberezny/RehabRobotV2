webpackHotUpdate("static\\development\\pages\\index.js",{

/***/ "./src/reducers.js":
/*!*************************!*\
  !*** ./src/reducers.js ***!
  \*************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var redux__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! redux */ "./node_modules/redux/es/redux.js");
/* harmony import */ var _actions__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./actions */ "./src/actions.js");
function _defineProperty(obj, key, value) { if (key in obj) { Object.defineProperty(obj, key, { value: value, enumerable: true, configurable: true, writable: true }); } else { obj[key] = value; } return obj; }



var initState = {
  user: 1,
  menuOpen: false,
  activePage: 1,
  socket: null,
  run: 0,
  P: 2,
  D: 1,
  xdes: 1000,
  K: 10,
  B: 0.5,
  M: 0.09,
  x: 0.5,
  v: 0,
  vdes: 0,
  game: 1,
  xmax: 100.0,
  vmax: 0.0,
  k_contact: 0.0,
  m_contact: 0.0,
  k_assist: 0.0,
  k_floor: 0.0,
  k_gravity: 0.0,
  exp: 1,
  stage: 1,
  home: 0,
  x_ball: -60,
  x_end: 400.0,
  range: 30
};

function todoApp() {
  var state = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : initState;
  var action = arguments.length > 1 ? arguments[1] : undefined;

  switch (action.type) {
    case _actions__WEBPACK_IMPORTED_MODULE_1__["TOGGLE_DRAWER"]:
      return Object.assign({}, state, {
        menuOpen: !state.menuOpen
      });
      break;

    case _actions__WEBPACK_IMPORTED_MODULE_1__["OPEN_PAGE"]:
      return Object.assign({}, state, {
        activePage: action.id
      });
      break;

    case _actions__WEBPACK_IMPORTED_MODULE_1__["SELECT_CONTROLLER"]:
      console.log(action.controller);
      return Object.assign({}, state, {
        controller: action.controller
      });
      break;

    case _actions__WEBPACK_IMPORTED_MODULE_1__["SET_VALUE"]:
      return Object.assign({}, state, _defineProperty({}, action.id, action.value));
      break;

    default:
      return state;
  }
}

/* harmony default export */ __webpack_exports__["default"] = (todoApp);

/***/ })

})
//# sourceMappingURL=index.js.dbe0b9d79e31a3357c58.hot-update.js.map