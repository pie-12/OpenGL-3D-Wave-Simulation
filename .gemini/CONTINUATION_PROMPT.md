## Project Continuation Notes

### Latest Update:
- **Date:** Sunday, November 16, 2025
- **Commit:** `1693b71` - "feat: Add rotating cube to demonstrate arbitrary-axis rotation"
- **Description:**
  - Added a new `drawCube()` function to model and render a simple cube with correct normals for lighting.
  - Updated the main `display()` loop to render the cube.
  - Used `glPushMatrix()` and `glPopMatrix()` to isolate the cube's transformations.
  - Positioned the cube using `glTranslatef()` and applied a continuous rotation around the arbitrary axis (1,1,1) using `glRotatef()`.
  - The cube's rotation angle is animated in the `update()` function.
  - This successfully demonstrates the "Rotation about an arbitrary axis" concept from the user's theoretical requirements.

### Next Steps:
- The user can run the `wave_sim.exe` executable to see the wave and the rotating cube.
- Further development could involve:
    - Making the cube bob up and down with the waves.
    - Adding keyboard controls to switch between different shading models (Smooth vs. Flat) or projection types (Perspective vs. Orthographic).
    - Exploring other surface types (e.g., Ruled Surfaces) for different 3D objects.
    - Refactoring the code into separate .h and .cpp files for better organization.
