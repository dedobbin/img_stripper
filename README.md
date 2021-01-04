# img_stripper
Library to convert images files to audio files and other way around.

## Use
`./image_stripper ~/some_input_file ~/some_output_file`  

Supported file types are `wav`, `jpeg`, `tiff` and `png`.  
When the output is an image, a third argument is required, defining the dimensions of the image.

## Build
Just run `make`   
Dependencies are [SDL](https://wiki.libsdl.org/Installation) and [wav_hammer](https://github.com/dedobbin/wavhammer).  
`make check_deps` can be used to check if these are satisfied.

## Examples
![example1](https://nop.koindozer.org/gh/stepper1.png)  
(Partial [Fast Energetic Story Stepper](https://archive.org/details/uncopyrighted-music) converted to png)
##
lena.png [to audio](https://nop.koindozer.org/gh/lena.wav)
## 
A fun thing this tool allows you to do is converting an image, use an audio editor like Audacity to edit the audio file, and convert it back to an image again.
For example, Lena with tweaked EQ.  
![example2](https://nop.koindozer.org/gh/lena_eq1.png)  
##
Vector image with high decay factor echos. You can see them pile up at the bottom of the image.
![example3](https://nop.koindozer.org/gh/sdl2_echo2.png)
