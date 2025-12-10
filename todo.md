# To-Do

- [x] fix build issue on mac machine
- [x] verify physics integration
- [ ] create + edit demo vid

## Samson

- [ ] add collision logic
  - [x] enclose meshes in AABBs for collisions
  - [x] sphere-sphere
  - [x] box-box
  - [x] cube-box (for floors and walls)
  - [x] add rigid body logic to meshes
  - [x] collision loop
  - [ ] update character AABBs for animated models
- [ ] add logic for character hit animation
  - [x] create animation state machine for model
  - [ ] gather assets + animations
    - [x] find and add better brick texture with normal mapping
    - [x] fetch knight model from mixamo with pre-baked shield
    - [x] fetch idle + hit animations
    - [ ] combine animations + export from Blender
- [x] find, add, test fruit/veg models (_mesh_)
  - [x] sweet potato
  - [x] cabbage
  - [x] carrot
  - [x] apple
  - [x] onion

## Carlos

- [x] compute mass using constant density factor and computed volume
- [ ] add projectile logic
  - [ ] add spawn logic (randomized)
  - [ ] add despawn logic (when new projectile is thrown)
  - [ ] add throwing logic
- [ ] maybe skybox?
