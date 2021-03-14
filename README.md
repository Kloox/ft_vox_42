<h1>FT_VOX</h1><br />
<br />

<h2>Description</h2>
Welcome to the beautiful world of voxels, where you will use all the benefits of the<br/>
abstraction "the whole world is a 3 dimensions grid" in order to display a daunting number<br/>
of things on a screen, travel in a gigantic procedural universe and a completely malleable<br/>
playground. What are voxels?<br/>
<br/>
This project aims to confront you to a graphic project that will be extremely demanding<br/>
in terms of optimization. You will have to study the characteristics of the voxel worlds,<br/>
and use them along your infographics knowledge to display a lot of elements on screen.<br/>
Thus, you will have to study different algo/opti to obtain a SMOOTH render (there are<br/>
many of them). You will also have to manage your memory and data structures properly<br/>
to be able to travel in a very, very large universe. <br/>
<br/>

<h2>Installation</h2><br />
<br />

Requirements: `OpenGL version >= 4.0`.<br/>

You must install the following dependencies : `freeglut 2.8.1`, `freetype-2.10.0`, `glew 2.2.0`, `glm 0.9.9.8` and `irrKlang-64bit-1.6.0`.</br>

</br>

On Linux you can use:<br/>
`make install_libs`<br />
`. ./set_libs_paths.sh`<br />
`make` or `make fast` to compile with multiple threads.<br />
<br />

If you have the error "fatal error: GL/glu.h: No such file or directory" while installing the libs, you must install the package `libglu1-mesa-dev`.<br/>

You can now run ft_vox. :)
</br>

<h2>Parameters</h2><br />

`-f or --fullscreen :` Fullscreen mode.<br/>
`-w or --width <int> :` Specify the window width. (min: 320, max: 15360) <br/>
`-h or --height <int> :` Specify the window height. (min: 240, max: 8460)<br/>
`-ws or --world-size <int> :` Set the data structure world size, must be a multiple of 8. (min: 8, max: 128, default 64)<br/>
`-s or --seed <int> :` Set the seed world. (default 42)<br/>
`-d :` Debug mode<br/>
`--sync :` Disable vsync<br/>
`-v or --version <int> :` Specify shaders version. The right version to give is at your discretion. (min 100, max: 999, default: 460)<br/>

<h2>Controls</h2><br />

`ZQSD/WASD` to move around, `A` to go up, `E` to go down and `shift` to go fast.<br/>
`home` and `end` to go to two predetermined positions.<br/>
`PageUp` and `PageDown` to up/down the sound volume.<br/>
`left mouse button` to build a block.<br/>
`right mouse button` to remove a block.<br/>
`scrollwheel` to select different types of blocks.<br/>
`KP_+` and `KP_-` to change player speed.<br/>
`insert` to toggle anchor camera.<br/>
`F1` to change sampling value.<br/>
`F2` to toggle anti-aliasing.<br/>
`F3` to toggle transparency.<br/>
`F4` to toggle collision.<br/>
`F5` to toggle focussing on transparent blocks.<br/>
`F6` to change the framerate limit.<br/>
`F7` to change the render distance limit.<br/>
`F8` to swap the both worlds.<br/>
`F9` to show debug info.<br/>
`F10` to desactivate infinite world.<br/>
`F11` to activate autowalk.<br/>

<h2>Screenshots</h2>
<h3>World:</h3>

<img src="screenshots/world.png"
     alt="World screenshot"
     style="float: left; margin-right: 10px;" />
<br/>

<h3>Cave:</h3>

<img src="screenshots/cave.png"
     alt="World screenshot"
     style="float: left; margin-right: 10px;" />

<h3>Blocks:</h3>

<img src="screenshots/blocks.png"
     alt="World screenshot"
     style="float: left; margin-right: 10px;" />


<h2>Grade</h2>

`125/100`