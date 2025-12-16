# Temporanim



Temporanim is a real-time 3D graphics demo submitted as a final project for the Fall 2025 edition of CSCI 1230/2230 Computer Graphics at Brown University. Built using OpenGL and the Qt Framework, this program implements the following features:

* Kinematic Skeletons
	* Parses and animates hierarchical bone structures to simulate animations.
* Linear Blend Skinning
	* Applies bone influences on meshes to display smooth deformations during character movement.
* Normal Mapping
	* Bolsters shading effects by incorporating mesh surface normal vectors in lighting calculations. 
* Rigid Body Translation
	* Orchestrates physics-based manipulation of non-deformable bodies and their interactions.
* Rigid Body Rotation
	* Models rotational forces acting on non-deformable bodies.
* Rigid Body Constraints
	* Collision Detection
 		* Uses axis-aligned bounding boxes to detect and resolve reaction forces.
	* Projectile Simulation
 		* Dynamically spawns and manages projectiles in physics-based systems.

It also includes tessellation support for primitives and key-based interactivity for camera movement, animation inspection, and normal map toggling.

## Media Samples

### Demonstrating Normal Maps and Toggling

![temporanim_normal_map](https://github.com/user-attachments/assets/f9974e10-616b-43ba-9257-bea3b6fc83f6)

### Demonstrating Animation Controls

![temporanim_heraklios](https://github.com/user-attachments/assets/3bd38f97-eaf0-4222-93fe-157e9e622272)

### Demonstrating Gravitational Effects

![temporanim_gravity](https://github.com/user-attachments/assets/a86693c0-e7fc-490f-ae51-739f079add67)

### Demonstrating Camera Movement and Projectile Simulation with Physics

![temporanim_projectile_optimized](https://github.com/user-attachments/assets/f1adfa53-1227-4893-a83c-0fc69e4c0853)

## Controls

* Use the WASD keys to move the camera around.
* Use the Space key to move vertically upwards.
* Use the Ctrl/Cmd key to move vertically downwards.
* Use the P key to play and pause animations.
* Use the N key to toggle normal mapped textures on and off.
* Use the left and right arrows to swap to the previous and next animations respectively. If there is only one animation, either arrow will restart the animation.
* Use the F key to throw projectiles.

## Build Instructions

This project uses ASSIMP to load meshes. This repo includes a slimmed-down version of the ASSIMP source code. Here is the preferred way to build that folder in a way that is compatible with this project.

* Cut and paste the ASSIMP folder to your root directory.
	* For Windows users, your path would be `C:\assimp`.
    * For Mac users, your path would be `/Users/{username}/assimp`
* Use your preferred build managing software to build the ASSIMP source code. If you use the CMake GUI, do the following:
    1. Select the source folder to be the path of where you copied your ASSIMP folder to.
    2. Create a folder called build in your ASSIMP directory (e.g. `C:\assimp\build`).
    3. Select said build folder's path as the path you want to build your binaries in.
    4. Click on the `Configure` button at the bottom of the window.
        * You will get an error saying that the tests folder is missing.
        * This is intentional since its size is large and is not necessary for our purposes.
    5. Find the ASSIMP_BUILD_TESTS flag and disable it.
    6. Click on `Configure` again. It should work. You should see something like this:
    
    <img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/574a6874-a0ba-4acc-b8ca-7abbee56be05" />
    
    7. Click on `Generate`. 
    8. Navigate to your build folder, and run the command `cmake --build . --config Release` in your terminal.

## Known Bugs

* Collisions with animated figures have inaccurate bounding boxes and display glitches when projectiles interact.
* Projectiles occasionally slide on floors and phase through walls.

## Attributions

### References

* [LearnOpenGL](https://learnopengl.com)
	* [Skeletal Animation](https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation)
 	* [Normal Mapping](https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
* [OGLDev Tutorial 26 - Normal Mapping](https://ogldev.org/www/tutorial26/tutorial26.html)
* [Baraff, D. 1997. An Introduction to Physically Based Modeling.](https://www.cs.cmu.edu/~baraff/pbm/)
	* [Witkin, A., & Baraff, D. 1997. Differential Equation Basics.](https://www.cs.cmu.edu/~baraff/pbm/diffyq.pdf)
	* [Baraff, D. 1997. Rigid Body Simulation I - Unconstrained Rigid Body Dynamics](https://www.cs.cmu.edu/~baraff/pbm/rigid1.pdf)
 	* [Baraff, D. 1997. Rigid Body Simulation II - Nonpenetration Constraints](https://www.cs.cmu.edu/~baraff/pbm/rigid2.pdf)  

### Libraries

* [CS1230 Scenes](https://scenes.cs1230.graphics) for constructing and parsing custom scenes
* [ASSIMP](https://www.assimp.org) for loading models
* [Qt](https://www.qt.io) for OpenGL and GUI support
* [GLM](https://glm.g-truc.net) for supporting math operations
* [GLEW](https://glew.sourceforge.net) for loading OpenGL extensions

### Assets

* Paladin and Heraklios models and animations from [Mixamo](https://www.mixamo.com)
* Created using assets from [ambientCG.com](https://ambientcg.com), licensed under the Creative Commons CC0 1.0 Universal License.
	* [Bark](https://ambientcg.com/a/Bark001)
 	* [Rock](https://ambientcg.com/a/Rock035)
  	* [Roofing Tiles](https://ambientcg.com/a/RoofingTiles014A)
  	* [Tiles](https://ambientcg.com/a/Tiles129B)
* Marble chessboard texture from [DeviantArt](https://www.deviantart.com/sveinjo/art/Chessboard-Texture-128058895)
* Projectile meshes and textures from [Sketchfab](https://sketchfab.com) by [Meerschaum Digital](https://sketchfab.com/meerschaumdigital)
	* [Apple (Game Ready / 2K PBR)](https://sketchfab.com/3d-models/apple-game-ready-2k-pbr-21a4f0b780494b529272a5e6785537db)
 	* [Cabbage](https://sketchfab.com/3d-models/cabbage-654306fc3ae344c7b9e7a9a40faca45b)
  	* [Carrot](https://sketchfab.com/3d-models/carrot-4cfcef5d26834657a0e1204d2ff32523)
  	* [Red Onion (Game Ready / 2K PBR)](https://sketchfab.com/3d-models/red-onion-game-ready-2k-pbr-963d2cf536db4128b23e6597bc78dd23)
  	* [Sweet Potato (Game Ready / 2K PBR)](https://sketchfab.com/3d-models/sweet-potato-game-ready-2k-pbr-205c0164366c483aaf905cd8a4f5a590) 

## Credits

* [Samson Tsegai](https://github.com/Avtomatovich)
	* Kinematic Skeletons
 	* Linear Blend Skinning
  	* Normal Mapping
  	* Collision Detection
  	* Projectile Simulation
* [Carlos Freire](https://github.com/carlos-freire2)
	* Rigid Body Translation
 	* Rigid Body Rotation
  	* Projectile Simulation
