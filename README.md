# Temporanim

## Build Instructions

This project uses ASSIMP to load meshes. This repo includes a slimmed-down version of the ASSIMP source code. Here is the preferred way to build that folder in a way that is compatible with this project.

* Cut and paste the ASSIMP folder to your root directory.
    * For Windows users, your path would be `C:\assimp`.
    * For Mac users, your path would be `/Users/{username}/assimp`
* Use your preferred build managing software to build the ASSIMP source code. If you use the CMake GUI, do the following:
    1. Select the source folder to be the path of where you copied your ASSIMP folder to.
    2. Create a folder called build in your ASSIMP directory (e.g. `C:\assimp\build`).
    3. Select said build folder's path as the path you want to build your binaries in.
    4. Click on the `Configure` button at the bottom of the window.
        * You will get an error saying that the tests folder is missing.
        * This is intentional since its size is large and is not necessary for our purposes.
    5. Find the ASSIMP_BUILD_TESTS flag and disable it.
    6. Click on `Configure` again. It should work. You should see something like this:
    
    <img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/574a6874-a0ba-4acc-b8ca-7abbee56be05" />
    
    7. Click on `Generate`. 
    8. Navigate to your build folder, and run the command `cmake --build . --config Release` in your terminal.
