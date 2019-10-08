webpackHotUpdate("static\\development\\pages\\index.js",{

/***/ "./games/Block_game.js":
/*!*****************************!*\
  !*** ./games/Block_game.js ***!
  \*****************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! react */ "./node_modules/react/index.js");
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! react-redux */ "./node_modules/react-redux/es/index.js");
/* harmony import */ var three__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! three */ "./node_modules/three/build/three.module.js");
/* harmony import */ var _src_actions__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ../src/actions */ "./src/actions.js");
var _jsxFileName = "C:\\Users\\nicho_000\\Documents\\RehabRobot\\server\\games\\Block_game.js";

function _typeof(obj) { if (typeof Symbol === "function" && typeof Symbol.iterator === "symbol") { _typeof = function _typeof(obj) { return typeof obj; }; } else { _typeof = function _typeof(obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj; }; } return _typeof(obj); }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

function _possibleConstructorReturn(self, call) { if (call && (_typeof(call) === "object" || typeof call === "function")) { return call; } return _assertThisInitialized(self); }

function _getPrototypeOf(o) { _getPrototypeOf = Object.setPrototypeOf ? Object.getPrototypeOf : function _getPrototypeOf(o) { return o.__proto__ || Object.getPrototypeOf(o); }; return _getPrototypeOf(o); }

function _assertThisInitialized(self) { if (self === void 0) { throw new ReferenceError("this hasn't been initialised - super() hasn't been called"); } return self; }

function _inherits(subClass, superClass) { if (typeof superClass !== "function" && superClass !== null) { throw new TypeError("Super expression must either be null or a function"); } subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, writable: true, configurable: true } }); if (superClass) _setPrototypeOf(subClass, superClass); }

function _setPrototypeOf(o, p) { _setPrototypeOf = Object.setPrototypeOf || function _setPrototypeOf(o, p) { o.__proto__ = p; return o; }; return _setPrototypeOf(o, p); }



 //import * as OBJLoader from 'three-obj-loader';

 //OBJLoader(THREE);
//require("/home/rehab/Documents/RehabRobot/server/games/sprite/crosshair.png")
//require("/home/rehab/Documents/RehabRobot/server/games/sprite/crosshair_yellow.png" );

var Follow_game =
/*#__PURE__*/
function (_Component) {
  _inherits(Follow_game, _Component);

  function Follow_game(props) {
    var _this;

    _classCallCheck(this, Follow_game);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(Follow_game).call(this, props));
    _this.start = _this.start.bind(_assertThisInitialized(_this));
    _this.stop = _this.stop.bind(_assertThisInitialized(_this));
    _this.animate = _this.animate.bind(_assertThisInitialized(_this));
    return _this;
  }

  _createClass(Follow_game, [{
    key: "componentDidMount",
    value: function componentDidMount() {
      var points = 0;
      var maxStroke = 200;
      var text;
      var group = new three__WEBPACK_IMPORTED_MODULE_2__["Group"]();
      var width = this.mount.clientWidth;
      var height = this.mount.clientHeight;
      var scene = new three__WEBPACK_IMPORTED_MODULE_2__["Scene"]();
      scene.background = new three__WEBPACK_IMPORTED_MODULE_2__["Color"](0xf7fdff); //var camera = new THREE.OrthographicCamera( - width / 2, width / 2, height / 2, - height / 2, -100, 500000);

      var camera = new three__WEBPACK_IMPORTED_MODULE_2__["PerspectiveCamera"](70, this.mount.clientWidth / this.mount.clientHeight, 0.1, 2000);
      camera.position.set(500, 500, 200);
      scene.add(camera);
      var renderer = new three__WEBPACK_IMPORTED_MODULE_2__["WebGLRenderer"]({
        antialias: true
      });
      renderer.setSize(width, height);
      var geometry = new three__WEBPACK_IMPORTED_MODULE_2__["BoxGeometry"](100, 100, 100);
      var cubeMaterial = new three__WEBPACK_IMPORTED_MODULE_2__["MeshLambertMaterial"]({
        color: 0xE86D00
      });
      var cube = new three__WEBPACK_IMPORTED_MODULE_2__["Mesh"](geometry, cubeMaterial);
      var footMaterial = new three__WEBPACK_IMPORTED_MODULE_2__["MeshLambertMaterial"]({
        color: 0x50A2FF
      });
      var foot = new three__WEBPACK_IMPORTED_MODULE_2__["Mesh"](geometry, footMaterial);
      cube.position.set(0.0, 0.0, -100);
      foot.position.set(0.0, -200, -100);
      foot.scale.set(1, 0.25, 1);
      var spotLight1 = new three__WEBPACK_IMPORTED_MODULE_2__["SpotLight"](0xffffff, 1);
      spotLight1.position.set(-width / 3, 200, 200);
      var light = new three__WEBPACK_IMPORTED_MODULE_2__["HemisphereLight"](0xffffff, 0xffffff, 1);
      scene.add(light);
      scene.add(spotLight1);
      scene.add(cube);
      scene.add(foot);
      this.scene = scene;
      this.camera = camera;
      this.renderer = renderer;
      this.clock = new three__WEBPACK_IMPORTED_MODULE_2__["Clock"]();
      this.timer = 0;
      this.start_game = 0;
      this.cube = cube;
      this.foot = foot; //this.group = group;

      this.points = points;
      this.mount.appendChild(this.renderer.domElement);
      this.start();
    }
  }, {
    key: "componentWillUnmount",
    value: function componentWillUnmount() {
      this.stop();
      this.mount.removeChild(this.renderer.domElement);
    }
  }, {
    key: "start",
    value: function start() {
      if (!this.frameId) {
        this.frameId = requestAnimationFrame(this.animate);
      }
    }
  }, {
    key: "stop",
    value: function stop() {
      cancelAnimationFrame(this.frameId);
    }
  }, {
    key: "animate",
    value: function animate() {
      if (!this.start_game) {
        this.timer += this.clock.getDelta();
        var current_time = Math.round(5.8 - this.timer).toString();
        var textMaterial = this.textMaterial; //this.camera.position.x = 500*Math.cos(this.timer/100)
        //this.camera.position.y = 50*Math.cos(this.timer/100)

        this.camera.up = new three__WEBPACK_IMPORTED_MODULE_2__["Vector3"](0, 0, 1);
        this.camera.lookAt(this.cube.position);

        if (this.timer > 4.9) {
          this.start_game = 1;
        }
      } else {
        this.cube.position.y = 137.5 - this.props.x_ball;
        this.foot.position.y = 200.0 - this.props.x; //console.log('x_end' + this.props.x_end)
        //console.log('x cube' + this.factor*this.props.x)
        //console.log('xdes cube' + this.factor*this.props.xdes )
      }

      this.renderScene();
      this.frameId = window.requestAnimationFrame(this.animate);
    }
  }, {
    key: "renderScene",
    value: function renderScene() {
      this.renderer.render(this.scene, this.camera);
    }
  }, {
    key: "render",
    value: function render() {
      var _this2 = this;

      return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
        style: {
          width: '800px',
          height: '800px'
        },
        ref: function ref(mount) {
          _this2.mount = mount;
        },
        __source: {
          fileName: _jsxFileName,
          lineNumber: 138
        },
        __self: this
      });
    }
  }]);

  return Follow_game;
}(react__WEBPACK_IMPORTED_MODULE_0__["Component"]);

function mapStateToProps(state) {
  //map state variables to the component's state 
  return {
    x: state.x,
    xdes: state.xdes,
    x_end: state.x_end,
    range: state.range
  };
}

/* harmony default export */ __webpack_exports__["default"] = (Object(react_redux__WEBPACK_IMPORTED_MODULE_1__["connect"])(mapStateToProps, {} //add importing action functions here
)(Follow_game));

/***/ })

})
//# sourceMappingURL=index.js.2b0cf0fb6d562ee2b683.hot-update.js.map