import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as THREE from 'three'
//import * as OBJLoader from 'three-obj-loader';
import { setParam } from "../src/actions";
//OBJLoader(THREE);

//require("/home/rehab/Documents/RehabRobot/server/games/sprite/crosshair.png")
//require("/home/rehab/Documents/RehabRobot/server/games/sprite/crosshair_yellow.png" );

class Race_game extends Component {
  constructor(props) {
    super(props)

    this.start = this.start.bind(this)
    this.stop = this.stop.bind(this)
    this.animate = this.animate.bind(this)
    this.move_object = this.move_object.bind(this)
  }

  componentDidMount() {

    var theta1 = 0.0
    var theta2 = 0.0
    var theta3 = 0.0

    var race_speed1 = 0.5
    var race_speed2 = 0.55
    var race_speed3 = 0.6

    var prev_x;
    var prev_time;
    var curr_time;  

    var group = new THREE.Group();

    const width = this.mount.clientWidth
    const height = this.mount.clientHeight

    var scene = new THREE.Scene()
    var fogColor = new THREE.Color(0x99ccff);
    scene.background = fogColor;
    scene.background = new THREE.Color( fogColor );
    //scene.fog = new THREE.Fog(fogColor, 0.0025, 500);

    
    var camera = new THREE.PerspectiveCamera(90, window.innerWidth/window.innerHeight, 0.1, 800 );
    camera.position.set( -95,-50,30);
    //camera.rotation.set(1.5,0.0,0.0);
    scene.add( camera );

    const renderer = new THREE.WebGLRenderer({ antialias: true })
    renderer.setSize(width, height)

    var plateMaterial = new THREE.MeshBasicMaterial( { color: 0xcc6600} );
    var plateMaterial2 = new THREE.MeshBasicMaterial( { color: 0x009933} );
    var other1Material = new THREE.MeshLambertMaterial( { color: 0xff7272 } );
    var mainMaterial = new THREE.MeshLambertMaterial( { color: 0xFFA500 } );
    var other2Material = new THREE.MeshLambertMaterial({ color: 0xD4D1C8 });
    var boxMaterial = new THREE.MeshLambertMaterial({ color: 0x99ccff });

    //var textureLoader = new THREE.TextureLoader();
    //var texture_grass = textureLoader.load( "static/textures/grass.jpg" );
    //var material_grass = new THREE.MeshBasicMaterial( { color: 0xffffff, map: texture_grass } );
    var material_grass = new THREE.MeshBasicMaterial( { color: 0x009933 } );
    
    //var texture_track = textureLoader.load( "static/textures/track.jpg" );
    //var material_track = new THREE.MeshBasicMaterial( { color: 0xffffff, map: texture_track } );
    var material_track = new THREE.MeshBasicMaterial( { color: 0xcc6600} );

   // texture_grass.wrapS = texture_grass.wrapT = THREE.RepeatWrapping;
    //texture_grass.repeat.set( 64, 64 );

    //var texture_grass2 = textureLoader.load( "static/textures/grass.jpg" );
    //texture_grass2.wrapS = texture_grass2.wrapT = THREE.RepeatWrapping;
    //texture_grass2.repeat.set( 4, 4);

    //var material_grass2 = new THREE.MeshBasicMaterial( { color: 0xffffff, map: texture_grass2 } );
    var material_grass2 = new THREE.MeshBasicMaterial( { color: 0x009933 } );

    //texture_track.wrapS = texture_track.wrapT = THREE.RepeatWrapping;
   // texture_track.repeat.set( 8, 8 );

    var circle = new THREE.Mesh( new THREE.CircleBufferGeometry( 100, 20, 0, Math.PI * 2 ), material_track );
    circle.position.set(0,-100,0)
    scene.add(circle)

    var circle2 = new THREE.Mesh( new THREE.CircleBufferGeometry( 100, 20, 0, Math.PI * 2 ), material_track );
    circle2.position.set(0,100,0)
    scene.add(circle2)

    var plate = new THREE.Mesh( new THREE.PlaneBufferGeometry( 200, 200, 4, 4 ), material_track );
    plate.position.set(0,0,0)
    scene.add(plate)

    var circle3 = new THREE.Mesh( new THREE.CircleBufferGeometry( 60, 20, 0, Math.PI * 2 ), material_grass2 );
    circle3.position.set(0,-100,0.11)
    scene.add(circle3)

    var circle4 = new THREE.Mesh( new THREE.CircleBufferGeometry( 60, 20, 0, Math.PI * 2 ), material_grass2 );
    circle4.position.set(0,100,0.11)
    scene.add(circle4)

    var plate2 = new THREE.Mesh( new THREE.PlaneBufferGeometry( 120, 200, 4, 4 ), material_grass2 );
    plate2.position.set(0,0,0.10)
    scene.add(plate2)

  
    var ground = new THREE.Mesh( new THREE.PlaneBufferGeometry( 5000, 5000, 4, 4 ), material_grass );
    ground.position.set(-500,-500,-0.1)
    scene.add(ground)

    var character = new THREE.Mesh( new THREE.TetrahedronBufferGeometry( 5, 0 ), mainMaterial );
    character.position.set(-80,0,5)
    scene.add(character)

    var other = new THREE.Mesh( new THREE.BoxBufferGeometry( 5, 5, 5, 4, 4, 4 ), other1Material );
    other.position.set(-90,0,5)
    scene.add(other)

    var other2 = new THREE.Mesh( new THREE.BoxBufferGeometry( 5, 5, 5, 4, 4, 4 ), other2Material );
    other2.position.set(-70,0,5)
    scene.add(other2)

    var box1 = new THREE.Mesh( new THREE.BoxBufferGeometry( 5000, 5, 1000 ), boxMaterial );
    box1.position.set(0,350,0)
    scene.add(box1)

    var spotLight1 = new THREE.SpotLight( 0xffffff, 1 );
    spotLight1.position.set( -width/3, 200, 200 );

    var light = new THREE.HemisphereLight( 0xffffff, 0xffffff, 1 );
    scene.add( light );



    scene.add(spotLight1)
    scene.add( camera );

    this.character = character;
    this.other = other;
    this.other2 = other2 

    this.scene = scene
    this.camera = camera
    this.renderer = renderer

    this.theta1 = theta1
    this.theta2 = theta2
    this.theta3 = theta3

    this.race_speed1 = race_speed1
    this.race_speed2 = race_speed2
    this.race_speed3 = race_speed3

    this.prev_x = prev_x; 
    this.prev_time = prev_time;
    this.curr_time = curr_time; 
    this.timer = 0.0

    this.clock = new THREE.Clock()

    this.begin = 0;

    this.mount.appendChild(this.renderer.domElement)
    this.start()
  }

  componentWillUnmount() {
    this.stop()
    this.mount.removeChild(this.renderer.domElement)
  }

  start() {
    if (!this.frameId) {
      this.frameId = requestAnimationFrame(this.animate)
    }
  }

  stop() {
    cancelAnimationFrame(this.frameId)
  }

  move_object(object, theta, radius, race_speed) {

    if(object.position.x > 0){
      var race_dir = 1.0;
    }else{
      var race_dir = -1.0;
    }

    if(object.position.y > 0){
      var turn_dir = 1.0;
    }else{
      var turn_dir = -1.0;
    }
    
    if(Math.abs(object.position.y) < 100 ) 
    {
      object.position.y -= race_speed*race_dir
    }

    else
    {
      object.position.x = -turn_dir*radius*Math.cos(theta);
      object.position.y = turn_dir*(100.0 + radius*Math.sin(theta));
      object.rotation.z = theta;
    }
  }

  animate() {

  //wait for 10 sec before beginning 
  this.timer += this.clock.getDelta()
  if(this.timer >= 5) this.begin = 1 
  console.log(this.timer)


  if(this.begin){
    
    this.race_speed1 = (150.0 - 5.0 - Math.abs(this.props.x - this.props.xdes)) / 250.0
    //this.race_speed2 = this.race_speed2
    if(this.timer > 6) {
      if(this.race_speed1>this.race_speed2) { this.race_speed2 = this.race_speed1; }
      else { this.race_speed2 -= 0.03*Math.random(); }
      this.race_speed3 = 0.57 - 5*Math.random()/25 + 5*Math.random()/25
      this.timer = 0;
    }
    //else this.race_speed3 = 0.57

    if(this.character.position.y >= 100 )
    {
      this.theta1 += this.race_speed1/90.0;
      this.camera.position.x = this.character.position.x - 40.0*Math.sin(this.theta1);
      this.camera.position.y = this.character.position.y - 40.0*Math.cos(this.theta1);
    }

    else if(this.character.position.y <= -100 )
    {
      this.theta1 += this.race_speed1/90.0;
      this.camera.position.x = this.character.position.x + 40.0*Math.sin(this.theta1);
      this.camera.position.y = this.character.position.y + 40.0*Math.cos(this.theta1);
    }else if(this.character.position.x > 10 ){
      this.theta1 = 0.0;
      this.camera.position.x = this.character.position.x + 40.0*Math.sin(this.theta1);
      this.camera.position.y = this.character.position.y + 40.0*Math.cos(this.theta1);
    }else{
      this.theta1 = 0.0;
      this.camera.position.x = this.character.position.x - 40.0*Math.sin(this.theta1);
      this.camera.position.y = this.character.position.y - 40.0*Math.cos(this.theta1);
    }

    if(Math.abs(this.other.position.y) >= 100 )
    {
      this.theta2 += this.race_speed2/90.0;
    }else{
      this.theta2 = 0.0;
    }
    if(Math.abs(this.other2.position.y) >= 100 )
    {
      this.theta3 += this.race_speed3/90.0;
    }else{
      this.theta3 = 0.0;
    }
        this.move_object(this.character, this.theta1, 80.0, this.race_speed1);
        this.move_object(this.other, this.theta2, 90.0, this.race_speed2);
        this.move_object(this.other2, this.theta3, 70.0, this.race_speed3);

        console.log(this.character.rotation.z)
  
        this.character.rotation.x += 0.1;
        this.character.rotation.y += 0.05; 

        this.other.rotation.x += 0.1;
        this.other.rotation.y += 0.05;

        this.other2.rotation.x += 0.1;
        this.other2.rotation.y += 0.05;

    }

    this.camera.up = new THREE.Vector3(0,0,1);
    this.camera.lookAt( this.character.position );

    this.renderScene()
    this.frameId = window.requestAnimationFrame(this.animate)
   
  }

  renderScene() {
    this.renderer.render(this.scene, this.camera)
  }

  render() {

    return (
      
      <div
        style={{ width: '800px', height: '400px' }}
        ref={(mount) => { this.mount = mount }}
      />
    )
  }
}

function mapStateToProps(state) {
  
  //map state variables to the component's state 
  return {
    x: state.x,
    xdes: state.xdes,
    v: state.v,
    vdes: state.vdes
  }
}


export default connect(
  mapStateToProps,
  {} //add importing action functions here
)(Race_game);