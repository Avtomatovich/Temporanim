# To-Do

- [x] fix build issue on mac machine
- [x] verify physics integration
- [ ] improve what we have
  - [ ] gather assets + animations
    - [ ] find better brick texture with normal mapping
    - [ ] find another knight model with pre-baked shield
- [ ] create + edit demo vid

## Samson

- [ ] add collision logic
  - [x] enclose meshes in AABBs for collisions
  - [x] sphere-sphere
  - [x] box-box
  - [x] cube-box (for floors and walls)
  - [ ] add rigid body logic to meshes
  - [ ] collision loop
- [ ] add logic for character hit animation
  - [ ] find and rig hit animation
  - [ ] react to hit? (move away from projectile impact)
    - [ ] fetch ctm ref to mod it for all meshes in a model
    - [ ] add rigid body instance?
- [x] find and add fruit/veg models (_mesh_)
  - [x] tomato
  - [x] cabbage
  - [x] carrot
  - [x] apple
  - [x] onion
- [x] test fruit/veg models (_mesh_)
  - [x] tomato
  - [x] cabbage
  - [x] carrot
  - [x] apple
  - [x] onion

## Carlos

- [ ] compute mass using constant density factor and computed volume
- [ ] add projectile logic
  - [ ] add spawn logic (randomized)
  - [ ] add despawn logic (when new projectile is thrown)
  - [ ] add throwing logic
- [ ] maybe skybox?
