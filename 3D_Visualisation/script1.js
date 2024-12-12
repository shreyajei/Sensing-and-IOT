import * as THREE from "https://cdn.skypack.dev/three@0.129.0/build/three.module.js";
import { GLTFLoader } from "https://cdn.skypack.dev/three@0.129.0/examples/jsm/loaders/GLTFLoader.js";

let scene, camera, renderer, model, model1;
let warningDisplayed = false; // Flag to prevent message spamming
window.resetPosition = resetPosition;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

function init3D() {
  // Initialize scene and camera
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  camera = new THREE.PerspectiveCamera(
    75,
    parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")),
    0.1,
    1000
  );
  camera.position.set(0, 1, 10); // Start the camera farther back

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));
  document.getElementById("3Dcube").appendChild(renderer.domElement);

  // Add directional light
  const light = new THREE.DirectionalLight(0xffffff, 1);
  light.position.set(5, 5, 5).normalize();
  scene.add(light);

  // Load the 3D model
  const loader = new GLTFLoader();
  loader.load('scaledbreast.gltf', (gltf) => {
    model1 = gltf.scene;
    model1.scale.set(0.1, 0.1, 0.1);
    model1.position.set(-20, 0, 0);
    model1.traverse((node) => {
      if (node.isMesh) {
        node.material.color.set(0xffdbac); 
      }
    });

    scene.add(model1);
  }, undefined, console.error);

  animate();

  loader.load('mamilla.gltf', (gltf) => {
    model = gltf.scene;
    model.scale.set(0.06, 0.06, 0.06);
    model.position.set(12, 0, -4);
    model.rotation.y = Math.PI; // 90 degrees in radians
    model.rotation.z = Math.PI; // 90 degrees in radians
    model.rotation.x = Math.PI / 2; // 90 degrees in radians
    model.traverse((node) => {
      if (node.isMesh) {
        node.material.color.set(0xffdbac); 
        node.material.transparent = true;
        node.material.opacity = 0.5; // transparency
      }
    });

    scene.add(model);
  }, undefined, console.error);
}

// Render loop
function animate() {
  requestAnimationFrame(animate);
  renderer.render(scene, camera);
}

// Update model rotation based on gyroscope readings
if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function (e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function (e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('gyro_readings', function (e) {
    var obj = JSON.parse(e.data);
    document.getElementById("gyroX").innerHTML = obj.gyroX;
    document.getElementById("gyroY").innerHTML = obj.gyroY;
    document.getElementById("gyroZ").innerHTML = obj.gyroZ;


    // Update model rotation
    model.rotation.x = obj.gyroY;
    model.rotation.z = obj.gyroX;
    model.rotation.y = obj.gyroZ;
    renderer.render(scene, camera);
  }, false);


  source.addEventListener('accelerometer_readings', function (e) {
    console.log("accelerometer_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("accX").innerHTML = obj.accX;
    document.getElementById("accY").innerHTML = obj.accY;
    document.getElementById("accZ").innerHTML = obj.accZ;
  }, false);
}

function resetPosition(element) {
  if (element.id === "reset") {
    model.position.set(12, 0, -4);
  }
  renderer.render(scene, camera); // Re-render the scene
}

init3D();
