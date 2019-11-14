Description
-----------

This is an experiment in compile-time eval on c++-14 and above. Test constitutes the computation of an image via raycasting over a minimalistic scene of voxels. The practical value of this project is nil, aside for torturing the compiler.

Test passes successfully on:

| compiler, RAM amount                                        |
|-------------------------------------------------------------|
| g++-6.3.0, 8GB RAM
| g++-7.4.0, 6GB RAM, 14GB VM                                 |
| g++-8.2.0, 4GB ZRAM                                         |
| g++-8.3.0, 4GB ZRAM                                         |
| g++-9.1.0, unknown (godbolt.org); output reduced to 128x128 |
| g++-9.2.0, unknown (godbolt.org); output reduced to 128x128 |

Test fails for various reasons on other compilers. Such as:

* not-constexpr pure functions in cmath
* not-fully-c++14-compliant constexpr support
* compiler runs out of (virtual) memory
