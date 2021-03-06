# RPI Game Architecture Homework Repository

  * build - Project files & makefiles as well as the scripts for generating them.
  * personal - Scratch space.
  * src - GA engine framework and homework assignments.

Note: MinGW-w64 build is supported, but not recommended.
Recommended download location is: https://mingw-w64.org/doku.php/download/mingw-builds
The mingw bin folder must be in your PATH variable before running setup-mingw.bat.

Note: Documentation for audio component is located in `src\ga1-core\audio\html\classga__audio__component.html`

# Linking Dependencies

Right-click on the `ga` solution and select Properties

For All Configurations:
  * C/C++ > General > Add `$(SolutionDir)\..\..\src\ga1-core\fmod\inc` to the Additional Include Directories
  * Linker > General > Add `$(SolutionDir)\..\..\src\ga1-core\fmod\lib\x64` to the Additional Library Directories

For Debug:
  * Linker > Input > Add `fmodL_vc.lib` to Additional Dependencies
  * Build Events > Post-Build Event > Add the following three lines to Command Line:<br>
  `xcopy /y "$(SolutionDir)\..\..\src\ga1-core\fmod\lib\x64\fmodL.dll" "$(OutDir)"`<br>
  `xcopy /y "$(SolutionDir)\..\..\src\ga1-core\data\sounds\*" "$(OutDir)\data\sounds\"`<br>
  `xcopy /y "$(SolutionDir)\..\..\src\ga1-core\data\textures\speaker.png" "$(OutDir)\data\textures\"`

For Release:
  * Linker > Input > Add `fmod_vc.lib` to Additional Dependencies
  * Build Events > Post-Build Event > Add the following three lines to Command Line:<br>
  `xcopy /y "$(SolutionDir)\..\..\src\ga1-core\fmod\lib\x64\fmod.dll" "$(OutDir)"`<br>
  `xcopy /y "$(SolutionDir)\..\..\src\ga1-core\data\sounds\*" "$(OutDir)\data\sounds\"`<br>
  `xcopy /y "$(SolutionDir)\..\..\src\ga1-core\data\textures\speaker.png" "$(OutDir)\data\textures\"`
