The following setup are modified in Solution Properties - Configuration Properties

### GLFW and GLEW Setup ###

C/C++ - General - Additional Include Directories
```
$(ProjectDir)..\Dependencies\glew-2.1.0\include;$(ProjectDir)..\Dependencies\GLFW\include
```

C/C++ - Preprocessor - Preprocessor Definitions
```
GLEW_STATIC
```

Linker - General - Additional Library Directories
```
$(ProjectDir)..\Dependencies\glew-2.1.0\lib\Release\x64;$(ProjectDir)..\Dependencies\GLFW\lib-vc2022;%(AdditionalLibraryDirectories)
```

Linker - Input - Additional Dependencies
```
glew32s.lib;glfw3.lib;opengl32.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)
```
