# About
This project started as a way for me to get some practice working with CMake (this is why the repo is a mess). Currently I don't have much time to spend working on this project due to working on my final assessment for school, but it does have some cool features implemented. Right now you can successfully connect up to 2 people to a game and create villagers.

# SDL, RakNet and Tiled

## SDL
I'm using the SDL library to handle all the graphics for my game. So far I'm enjoying to use SDL and it makes things a bit easier and more straight forward compared to writing my own OpenGL code.

## RakNet
RakNet is a great networking library that makes making multiplayer games like this really fun. Although RakNet itself hasn't been updated for a while, I still chose to go with it as I have some prior experience with it.

## Tiled
I'm using the Tiled software to create my world map. To parse Tiled's information into my game, I'm using a library called TMXParser.

# Dev Log
I'll be making videos to share the progress of the game every so often. Right now you can check out the first video at: https://youtu.be/qTe8Ex3hluk

# CMake
Right now the project isn't really a CMake project due to some version control issues... But! I do plan on making CMake work. I started off building this with CMake but when I built a Visual Studio Project with it early on, I started adding libraries and headers into the Visual Studio Project directly. This made me have to retreat from CMake until I can write my CMakeLists.txt to match the Visual Studio Project properties.

# Screenshots
![Imgur](https://i.imgur.com/4GVMz9O.png)
![Imgur](https://i.imgur.com/nLfaYvH.png)
![Imgur](https://i.imgur.com/RBBce7L.png)
