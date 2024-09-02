# gravitx
Enhanced version of my previous Gravity, writen in c++.



## Usage
To compile the code:
```
git submodule update --init --recursive 
make
```

### Skybox

Hdr skyboxes are big files you can download from here: https://svs.gsfc.nasa.gov/4851/

Download .exr in celestial coordinates.

You can convert it to hdr with imagemagick: https://imagemagick.org/
and finally:
```
./gravitx --skybox ./starmap_2020_8k.hdr --sky-factor 13
```

To support skyboxes, raylib must be configured with HDR support when built:
```
cmake -DCUSTOMIZE_BUILD=ON -DBUILD_SHARED_LIBS=OFF -DINCLUDE_EVERYTHING=ON -DSUPPORT_FILEFORMAT_HDR=ON ..
```


## Credits 

### Libs:
- https://www.raylib.com/
- https://github.com/leethomason/tinyxml2

### Data
- https://nssdc.gsfc.nasa.gov/planetary/factsheet/moonfact.html
- https://ssp.imcce.fr/forms/ephemeris
- https://astronomy.stackexchange.com/questions/14032/color-of-planets

### Images
- https://svs.gsfc.nasa.gov/4851/
- https://svs.gsfc.nasa.gov/cgi-bin/details.cgi?aid=4720
- https://en.wikipedia.org/wiki/File:Map_of_the_full_sun.jpg
- https://nssdc.gsfc.nasa.gov/planetary/image/earth_day.jpg
- https://en.wikipedia.org/wiki/File:FullMoon2010.jpg
- https://en.m.wikipedia.org/wiki/File:Earth_Western_Hemisphere_transparent_background.png