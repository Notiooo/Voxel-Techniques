<div align="center">
<img src="readme/logo.png" width="300px" alt="Voxino Logo"/>
<h1>Voxel Techniques</h1>
<div style="display: inline">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/Notiooo/Voxel-Techniques/.github%2Fworkflows%2Fcmake_windows_unit_tests.yml?label=Windows%20Unit%20Tests">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/Notiooo/Voxel-Techniques/.github%2Fworkflows%2Fcmake_windows_build.yml?label=Windows%20Build">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/Notiooo/Voxel-Techniques/.github%2Fworkflows%2Fclang-format-check.yml?label=Code%20Formatting">
</div>
</div>
<br>
<p align="justify">This project provides a testing environment for analyzing and comparing various techniques for optimizing voxel-based world representation. The goal of the project is to evaluate different methods for rendering voxel data, focusing on techniques like triangulation and ray tracing, which are commonly used in 3D interactive environments.</p>

<div align="center">
<a href="https://youtu.be/OVThWmk9Qlg"><img src="readme/demo.gif" alt="Gif presenting the game"/></a>
<p>A <a href="https://youtu.be/OVThWmk9Qlg">video</a> showing how the app works</p>
</div>

## Important note:
> This repository was created solely for the purpose of my Master's thesis titled "Techniques for Optimizing Voxel-Based Representation of the Game World", and it was used during the research phase. The repository has been archived and is no longer open for review or further development. However, I encourage you to read my thesis for detailed insights into the research, methods, and results derived from this testing environment.

## Tested voxel techniques
- Triangulation methods: naive method, face culling, greedy meshing, binary greedy meshing.
- Ray tracing methods: voxel ray marching, brick maps, sparse voxel octrees.

### Triangulation

| ![Base](readme/examples/traingulation/base.png) | ![Naive method](readme/examples/traingulation/naive_wireframe.png) |
|:--:|:--:|
| Base | Naive method |

| ![Culling Method](readme/examples/traingulation/culling_wireframe.png) | ![Culling Method with GPU support](readme/examples/traingulation/culling_gpu_wireframe.png) |
|:--:|:--:|
| Culling Method | Culling Method with GPU support |

| ![Greedy Meshing](readme/examples/traingulation/greedy_wireframe.png) | ![Binary Greedy Meshing](readme/examples/traingulation/binary_greedy_wireframe.png) |
|:--:|:--:|
| Greedy Meshing | Binary Greedy Meshing |


### Raycast

| ![Base](readme/examples/raycast/base.png) | ![Flat](readme/examples/raycast/flat.png) |
|:--:|:--:|
| Base | Flat |

| ![Brickmap](readme/examples/raycast/brickmap.png) | ![Octree](readme/examples/raycast/octree.png) |
|:--:|:--:|
| Brickmap | Octree |


### Fast Voxel Traversal Algorithm

| ![Fast Voxel Traversal Algorithm](readme/examples/fvta/FVTA_normals.png) | ![Fixed ray of length 0.2](readme/examples/fvta/FIXED_normals_0.2.png) |
|:--:|:--:|
| Fast Voxel Traversal Algorithm <br>~10'476'236 ray iterations | Fixed ray of length 0.2 <br>~33'831'991 ray iterations |

| ![Fixed ray of length 0.1](readme/examples/fvta/FIXED_normals_0.1.png) | ![Fixed ray of length 0.05](readme/examples/fvta/FIXED_normals_0.05.png) |
|:--:|:--:|
| Fixed ray of length 0.1 <br>~66'209'105 ray iterations | Fixed ray of length 0.05 <br>~130'390'387 ray iterations |


## Some of the results
<div align="center">
    <img src="readme/results/cpu_time_comparison.jpg" alt="CPU time comparison" width="500px" />
    <img src="readme/results/real_time_comparison.jpg" alt="Real time comparison" width="500px" />
    <img src="readme/results/memory_occupation.png" alt="Memory occupation" width="500px" />
</div>

## Tools and Libraries
- **C++20**: The core programming language used to develop the environment.
- **OpenGL**: The primary API for rendering.
- **CMake**: Used as the build system for managing dependencies and compiling the project.
- **FastNoiseLite**: Procedural noise generation.
- **GLEW**: OpenGL Extension Wrangler for managing OpenGL extensions.
- **GLM**: OpenGL Mathematics, a header-only C++ math library.
- **imgui**: Immediate Mode Graphical User Interface for rendering interactive GUI.
- **imgui-sfml**: Integration of imgui with SFML.
- **minitrace**: Lightweight tracing library.
- **SFML**: Simple and Fast Multimedia Library for handling windows, and inputs.
- **spdlog**: Fast, header-only C++ logging library.
- **stb**: Single-file public domain libraries for image loading and other utilities.
- **Tracy**: Real-time, frame-based profiling library.
- **yaml-cpp**: A YAML parser and emitter for C++.

<div align="center">
<h2>The thesis</h2>
<a href="readme/final_report.pdf"><img src="readme/final_report.png" width="80%"alt="Report image"/></a>
<p>You may find the thesis <a href="readme/final_report.pdf">here</a></p>
</div>