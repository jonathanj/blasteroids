# TODO

## Features

- [x] Text rendering
  - [x] sprintf
  - [x] normalize character image storage (to avoid 1bpp code in drawtext)
  - [x] character map to use other font bitmap layouts
- [x] Asteroids
  - [x] Render asteroids instead of circles
- [x] Entity manager (this is what will check collisions between every object and tell them each what they collided with)
  - [ ] Consider broadcasting events for things (instead of directly calling a function), so that score/sound/etc can react
- [x] Collisions
  - [ ] Make collision detection more efficient than O(n**2)
- [x] Bullets
- [ ] Score
- [x] Multiple players
- [ ] Starfield background
- [ ] Animation
- [x] Particle system
  - [ ] Hitting an asteroid
    - [x] Bullet
    - [ ] Ship
    - [ ] Asteroid
  - [x] Destroying an asteroid
  - [ ] Sparks when ships collide
- [ ] Fixed timestep updates
      Accumulate time between frames and perform a fixed timestep update, to
      prevent skipping interactions
- [ ] Lua scripting?

## Bugs

- [x] Coasting/thrusting near the bottom right corner causes a crash
- [x] Bullets can sometimes kill multiple asteroids
- [ ] Collisions near the edges of the screen don't check wrapped coordinates properly

## Documentation

- `A_`: Asset
- `R_`: Render
- `P_`: Player/Entity
- `M_`: Misc
- `I_`: Interface? (input, etc.)
- `G_`: Game
