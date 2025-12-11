# To-Do

- [x] fix build issue on mac machine
- [x] verify physics integration
- [ ] create + edit demo vid

## Samson

- [x] add support for embedded textures
- [x] add collision logic
  - [x] enclose meshes in AABBs for collisions
  - [x] sphere-sphere
  - [x] box-box
  - [x] cube-box (for floors and walls)
  - [x] add rigid body logic to meshes
  - [x] collision loop
  - [x] update character AABBs for animated models
- [x] add logic for character hit animation
  - [x] create animation state machine for model
  - [x] gather assets + animations
    - [x] find and add better brick texture with normal mapping
    - [x] fetch knight model from mixamo with pre-baked shield
    - [x] fetch idle + hit animations
- [x] find, add, test fruit/veg models (_mesh_)
  - [x] sweet potato
  - [x] cabbage
  - [x] carrot
  - [x] apple
  - [x] onion
- [ ] combine animations + export from Blender (RIP)

## Carlos

- [x] compute mass using constant density factor and computed volume
- [x] add projectile logic
  - [x] add spawn logic (randomized)
  - [x] add despawn logic (when new projectile is thrown)
  - [x] add throwing logic
- [ ] test projectile logic
- [ ] impl force updates
