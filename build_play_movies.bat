set MicrosDir=modules/uu.micros
set MicrosCLFlags=-I%MicrosDir%/include/ -I%MicrosDir%/libs/glew/include/
set MicrosCLFlags=%MicrosDir%/libs/glew/src/glew.c -DGLEW_STATIC opengl32.lib %MicrosCLFlags%
set MicrosCLFlags=-I%MicrosDir%/libs %MicrosDir%/libs/NT_x64/glfw3.lib gdi32.lib user32.lib shell32.lib %MicrosCLFlags%
set MicrosCLFLags=ole32.lib %MicrosCLFlags%
set MicrosCLFlags=%MicrosDir%/runtime/nt_runtime.cpp %MicrosCLFlags%
set MicrosCLFLags=-MD %MicrosCLFlags%
set CommonCLFlags=-nologo -EHsc -FC -Z7
REM CommonCLFlags=-IH:\3rdparty\ffmpeg-20161108\include\
REM cl -Fe:draw_cubes src\draw-cubes\draw_cubes.cpp %MicrosCLFlags% %CommonCLFlags% src\common.cpp
REM cl -Fe:draw_on_quad src\draw-on-quad-with-shader\main.cpp %MicrosCLFlags% %CommonCLFlags% src\common.cpp
REM cl -Fe:play_house_beat src\play-house-drum-pattern\*.cpp %MicrosCLFlags% %CommonCLFlags% src\common.cpp
cl -Fe:play_movies src\play-movie-loop\*.cpp ^
    %MicrosCLFlags% %CommonCLFlags% -Imodules -DOS=OS_WINDOWS ^
    avformat.lib avcodec.lib avutil.lib swscale.lib ^
    -D_CRT_SECURE_NO_WARNINGS ^
    -DUU_MOVIE_PLAYERS_DEV ^
    -IH:\3rdparty\ffmpeg-20161108\include\ ^
    /link /debug -incremental:no -opt:ref -PDB:play_movies.pdb ^
    -LIBPATH:H:\3rdparty\ffmpeg-20161108\lib\

