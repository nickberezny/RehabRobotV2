webpackHotUpdate("static\\development\\pages\\index.js",{

/***/ "./components/Pages/SetupPage.jsx":
/*!****************************************!*\
  !*** ./components/Pages/SetupPage.jsx ***!
  \****************************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! react */ "./node_modules/react/index.js");
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! react-redux */ "./node_modules/react-redux/es/index.js");
/* harmony import */ var _Generic_GenericButton__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ../Generic/GenericButton */ "./components/Generic/GenericButton.jsx");
/* harmony import */ var _Generic_InputText__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ../Generic/InputText */ "./components/Generic/InputText.jsx");
/* harmony import */ var _Generic_Dropdown__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ../Generic/Dropdown */ "./components/Generic/Dropdown.jsx");
/* harmony import */ var _Setups_FollowTraj_Dev__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! ../Setups/FollowTraj_Dev */ "./components/Setups/FollowTraj_Dev.jsx");
/* harmony import */ var _Setups_FollowVel_Dev__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! ../Setups/FollowVel_Dev */ "./components/Setups/FollowVel_Dev.jsx");
/* harmony import */ var _Setups_Balance_Dev__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! ../Setups/Balance_Dev */ "./components/Setups/Balance_Dev.jsx");
/* harmony import */ var _Setups_FollowTraj__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(/*! ../Setups/FollowTraj */ "./components/Setups/FollowTraj.jsx");
/* harmony import */ var _Setups_FollowVel__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(/*! ../Setups/FollowVel */ "./components/Setups/FollowVel.jsx");
/* harmony import */ var _Setups_Balance__WEBPACK_IMPORTED_MODULE_10__ = __webpack_require__(/*! ../Setups/Balance */ "./components/Setups/Balance.jsx");
/* harmony import */ var _material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11__ = __webpack_require__(/*! @material-ui/core/Grid */ "./node_modules/@material-ui/core/Grid/index.js");
/* harmony import */ var _material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11___default = /*#__PURE__*/__webpack_require__.n(_material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11__);
/* harmony import */ var _material_ui_core_styles__WEBPACK_IMPORTED_MODULE_12__ = __webpack_require__(/*! @material-ui/core/styles */ "./node_modules/@material-ui/core/styles/index.js");
/* harmony import */ var _material_ui_core_styles__WEBPACK_IMPORTED_MODULE_12___default = /*#__PURE__*/__webpack_require__.n(_material_ui_core_styles__WEBPACK_IMPORTED_MODULE_12__);
/* harmony import */ var _material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13__ = __webpack_require__(/*! @material-ui/core/Typography */ "./node_modules/@material-ui/core/Typography/index.js");
/* harmony import */ var _material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13___default = /*#__PURE__*/__webpack_require__.n(_material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13__);
var _jsxFileName = "C:\\Users\\nicho_000\\Documents\\RehabRobotV2\\server\\components\\Pages\\SetupPage.jsx";

function _typeof(obj) { if (typeof Symbol === "function" && typeof Symbol.iterator === "symbol") { _typeof = function _typeof(obj) { return typeof obj; }; } else { _typeof = function _typeof(obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj; }; } return _typeof(obj); }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

function _possibleConstructorReturn(self, call) { if (call && (_typeof(call) === "object" || typeof call === "function")) { return call; } return _assertThisInitialized(self); }

function _assertThisInitialized(self) { if (self === void 0) { throw new ReferenceError("this hasn't been initialised - super() hasn't been called"); } return self; }

function _getPrototypeOf(o) { _getPrototypeOf = Object.setPrototypeOf ? Object.getPrototypeOf : function _getPrototypeOf(o) { return o.__proto__ || Object.getPrototypeOf(o); }; return _getPrototypeOf(o); }

function _inherits(subClass, superClass) { if (typeof superClass !== "function" && superClass !== null) { throw new TypeError("Super expression must either be null or a function"); } subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, writable: true, configurable: true } }); if (superClass) _setPrototypeOf(subClass, superClass); }

function _setPrototypeOf(o, p) { _setPrototypeOf = Object.setPrototypeOf || function _setPrototypeOf(o, p) { o.__proto__ = p; return o; }; return _setPrototypeOf(o, p); }
















var SetupPage =
/*#__PURE__*/
function (_React$Component) {
  _inherits(SetupPage, _React$Component);

  function SetupPage(props) {
    var _this;

    _classCallCheck(this, SetupPage);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(SetupPage).call(this, props));
    _this.state = {
      contents: null
    };
    return _this;
  }

  _createClass(SetupPage, [{
    key: "render",
    value: function render() {
      if (this.props.user == 1) {
        switch (this.props.game) {
          case 1:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 37
              },
              __self: this
            }, " ", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Setups_FollowTraj_Dev__WEBPACK_IMPORTED_MODULE_5__["default"], {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 37
              },
              __self: this
            }), " ");
            break;

          case 2:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 40
              },
              __self: this
            }, " ", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Setups_FollowVel_Dev__WEBPACK_IMPORTED_MODULE_6__["default"], {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 40
              },
              __self: this
            }), " ");
            break;

          case 3:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 43
              },
              __self: this
            }, " ", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Setups_Balance_Dev__WEBPACK_IMPORTED_MODULE_7__["default"], {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 43
              },
              __self: this
            }), " ");
            break;

          default:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 46
              },
              __self: this
            }, " Select a Game ");
        }
      } else if (this.props.user == 2) {
        switch (this.props.game) {
          case 1:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 52
              },
              __self: this
            }, " ", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Setups_FollowTraj__WEBPACK_IMPORTED_MODULE_8__["default"], {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 52
              },
              __self: this
            }), " ");
            break;

          case 2:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 55
              },
              __self: this
            }, " ", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Setups_FollowVel__WEBPACK_IMPORTED_MODULE_9__["default"], {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 55
              },
              __self: this
            }), " ");
            break;

          case 3:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 58
              },
              __self: this
            }, " ", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Setups_Balance__WEBPACK_IMPORTED_MODULE_10__["default"], {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 58
              },
              __self: this
            }), " ");
            break;

          default:
            this.state.contents = react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
              __source: {
                fileName: _jsxFileName,
                lineNumber: 61
              },
              __self: this
            }, " Select a Game ");
        }
      }

      return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 69
        },
        __self: this
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 70
        },
        __self: this
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11___default.a, {
        container: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 71
        },
        __self: this
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11___default.a, {
        item: true,
        xs: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 72
        },
        __self: this
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13___default.a, {
        variant: "display1",
        gutterBottom: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 73
        },
        __self: this
      }, "Game Type"), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Generic_Dropdown__WEBPACK_IMPORTED_MODULE_4__["default"], {
        text: "Game",
        id: "game",
        value: this.props.game,
        select1: "Follow Trajectory",
        select2: "Racing",
        select3: "Balance",
        __source: {
          fileName: _jsxFileName,
          lineNumber: 76
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("br", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 77
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("br", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 78
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13___default.a, {
        variant: "display1",
        gutterBottom: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 79
        },
        __self: this
      }, "Trajectory"), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Generic_Dropdown__WEBPACK_IMPORTED_MODULE_4__["default"], {
        text: "Trajectory Profile",
        id: "game",
        value: this.props.game,
        select1: "Standard Trajectory",
        select2: "Custom Trajectory 1",
        select3: "Custom Trajectory 2",
        __source: {
          fileName: _jsxFileName,
          lineNumber: 82
        },
        __self: this
      })), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11___default.a, {
        item: true,
        xs: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 85
        },
        __self: this
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13___default.a, {
        variant: "display1",
        gutterBottom: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 86
        },
        __self: this
      }, "Parameters"), this.state.contents), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11___default.a, {
        item: true,
        xs: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 91
        },
        __self: this
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13___default.a, {
        variant: "display1",
        gutterBottom: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 92
        },
        __self: this
      }, "Send Commands"), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Generic_GenericButton__WEBPACK_IMPORTED_MODULE_2__["default"], {
        text: "Set",
        message: "SET",
        __source: {
          fileName: _jsxFileName,
          lineNumber: 95
        },
        __self: this
      }), this.props.SET, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Generic_GenericButton__WEBPACK_IMPORTED_MODULE_2__["default"], {
        text: "Home",
        message: "HOME",
        __source: {
          fileName: _jsxFileName,
          lineNumber: 97
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_Generic_GenericButton__WEBPACK_IMPORTED_MODULE_2__["default"], {
        text: "Run",
        message: "RUN",
        __source: {
          fileName: _jsxFileName,
          lineNumber: 98
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("br", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 99
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("br", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 100
        },
        __self: this
      }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Typography__WEBPACK_IMPORTED_MODULE_13___default.a, {
        variant: "display1",
        gutterBottom: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 101
        },
        __self: this
      }, "Robot Status"), "Status:", react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("br", {
        __source: {
          fileName: _jsxFileName,
          lineNumber: 105
        },
        __self: this
      }), "Total Length:"), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(_material_ui_core_Grid__WEBPACK_IMPORTED_MODULE_11___default.a, {
        item: true,
        xs: true,
        __source: {
          fileName: _jsxFileName,
          lineNumber: 108
        },
        __self: this
      }))));
    }
  }]);

  return SetupPage;
}(react__WEBPACK_IMPORTED_MODULE_0___default.a.Component);

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
  };
}

/* harmony default export */ __webpack_exports__["default"] = (Object(react_redux__WEBPACK_IMPORTED_MODULE_1__["connect"])(mapStateToProps, {})(SetupPage));

/***/ })

})
//# sourceMappingURL=index.js.224650836dfdd0744fef.hot-update.js.map