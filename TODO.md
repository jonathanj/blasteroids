# TODO

## Features

- [x] Text rendering
  - [x] sprintf
  - [ ] normalize character image storage (to avoid 1bpp code in drawtext)
  - [ ] character map to use other font bitmap layouts
- [ ] Asteroids
- [ ] Entity manager (this is what will check collissions between every object and tell them each what they collided with)
  - [ ] Consider broadcasting events for things (instead of directly calling a function), so that score/sound/etc can react
- [ ] Collisions
- [ ] Bullets
- [ ] Score
- [ ] Multiple players
- [ ] Starfield background
- [ ] Animation

## Bugs

- Coasting/thrusting near the bottom right corner causes a crash

## Documentation

- `A_`: Asset
- `R_`: Render
- `P_`: Player/Entity
- `M_`: Misc
- `I_`: Interface? (input, etc.)
- `G_`: Game
