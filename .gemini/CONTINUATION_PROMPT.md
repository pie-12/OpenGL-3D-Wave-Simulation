## Project Continuation Notes

### Latest Update:
- **Date:** Sunday, November 16, 2025
- **Commit:** `3fcc464` - "feat: Replace cube with a bobbing boat model"
- **Description:**
  - Replaced the simple cube with a composite boat model made of two blocks (hull and cabin).
  - Created a flexible `drawBlock()` function to draw cuboids of any dimension.
  - Implemented a `drawBoat()` function to assemble the parts.
  - The boat's vertical position is now dynamically calculated based on the wave's height at the center of the scene, creating a realistic bobbing effect.
  - The boat rotates around its Y-axis.

### Next Steps:
- The user can run the `wave_sim.exe` executable to see the boat bobbing on the waves.
- Further development could involve:
    - Making the boat also **tilt** according to the wave's normal vector for even more realism.
    - Adding keyboard controls to switch between different shading models (Smooth vs. Flat) or projection types (Perspective vs. Orthographic).
    - Refactoring the code into separate .h and .cpp files for better organization.
