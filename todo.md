# To-Do

- [x] fix build issue on mac machine
- [x] verify physics integration
- [ ] gather assets + animations
  - [ ] another knight model
  - [ ] maybe shield mesh? (add to character in Blender)
- [ ] create quick market scene (in Blender?) or just use what we have
  - [ ] maybe skybox?
- [ ] create + edit demo vid

## Samson

- [ ] add collision logic
  - [x] enclose meshes in AABBs for collisions
  - [x] sphere-sphere
  - [ ] box-box
  - [ ] collision loop
- [ ] add logic for character hit animation
  - [ ] find and rig hit animation
  - [ ] react to hit? (move away from projectile impact)
    - [ ] fetch ctm ref to mod it for all meshes in a model
    - [ ] add rigid body instance?

## Carlos

- [ ] compute mass using constant density factor and computed volume
- [ ] add projectile logic
  - [ ] hardcode fruit/veggies into scene
    - [ ] add spawn logic (randomized)
    - [ ] add despawn logic
- [ ] find fruit/veg models (_mesh_)
  - [ ] tomato
  - [ ] cabbage
  - [ ] carrot
  - [ ] apple
  - [ ] onion
