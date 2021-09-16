# imSquared
Puzzle game engine idealized in 2010.   
Revived now for the kids.

![imSquared](https://github.com/RuiVarela/imSquared/raw/master/sample.gif)   

[Web Demo](https://imsquared.demanda.pt/)


# credits
This game was developed by
* [Rui Varela](https://github.com/RuiVarela)
* [Pedro Bastos](https://github.com/theaGit)

Free music used
+ [escalpe - xtd](https://modarchive.org/index.php?request=view_profile&query=69008)
+ [blind justice - axis of eremation](https://modarchive.org/index.php?request=view_by_moduleid&query=87501)
+ [65mix - Scream](https://modarchive.org/index.php?request=view_by_moduleid&query=192351)


## Building
This should be very portable.   
The only dependency needed is [raylib](https://www.raylib.com/). 
Build raylib and place the `raylib.h` and `libraylib.a` under `vendor/raylib/{env}` and update the makefile accordingly.   

```
make
./imSquared
```
And that is it..

## Building for web   

use a specific version of emsdk
```
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

```
make WEB=1 DEBUG=0
python wasm-server.py
```
open chrome on http://localhost:8080/imSquared.html
