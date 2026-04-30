## Useful Resources I Used

[https://www.youtube.com/watch?v=2v8r0oQd9Xc&t=339s](Optimizing rendering voxels by Aurailus)
[https://0fps.net/2012/06/30/meshing-in-a-minecraft-game/](Meshing in a Minecraft Game)

https://renderwonk.com/publications/gdm-2002/GDM_August_2002.pdf

LearnOpenGL.com

https://learnopengl.com/Advanced-OpenGL/Cubemaps


## Fog Effect

The fog effect seen in the voxel engine is just a simple fragment shader calculation that determines the color of the block and shifts it to a predefined color based on the distance it is from the camera position.

## Atmosphere

- Three scattering effects, in scattering, out scattering and absorption.
- Blue light scatters more than other lights, when sun is setting, majority of blue light is reflected via outscattering making the colors shift to yellow and red.
- $L_{scattering}=F_{ex}L_0+L_{in}$
- We can divide the scattering phenomena into two groups: those which remove light from a ray (absorption and out-scattering, which we will combine under the term “extinction”), and those which add it (in-scattering). Let’s compare the radiance in a ray before and after it is affected by atmospheric light scattering (L0 and L scattering, respectively). Extinction has a multiplicative effect on L0, which we can express as a dimensionless factor Fex . In-scattering has an additive effect on L0, which we can express as a radiance value Lin. 
- Each particle is approximated to a slab of thickness ds and area A, each particle has an effective area to catch photons of $\sigma_{ab}$.
- Therefore total absorbing area across a volume is $A_{ab}=\sigma_{ab}\rho_{ab}A_{ds}$.
- Probability a photon gets absorbed = (absorbing area) / (total area)
- $P_{ab}=A{ab}/A=\sigma_{ab}\rho_{ab}ds$
- Define $\beta_{ab}=\sigma_{ab}\rho_{ab}$
- Then turn the probability into radiance loss $dL=-\beta_{ab}*L*ds$
- $dL/ds=-\beta_{ab}L$
- Solving gives: $L(s)=L_0e^{-\beta_{ab}s}$
- $β_{ab}$ absorption coefficient (probability per unit length), Light decays exponentially as it travels.
- For out scattering, we have the new scattering coefficient $β_{sc}$
- Mie phase is for large particles, Rayleigh phase is for small particles.
- This calculation is done in the vertex shader to speed up computation.
- For Rayleigh scattering, blue light is scattered more than red.
- Extinction $\beta_{ex}=\beta{ab}+\beta_{sc}$, That is, it is the sum of the absorption and scattering coefficients.

### Render Pipleline
- The render pipeline for the atmosphere starts with the distance between the camera position and the fragment position from the vertex shader