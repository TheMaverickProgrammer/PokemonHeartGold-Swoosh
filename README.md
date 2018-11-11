# PokemonHeartGold-Swoosh
Using the Swoosh segue and easing library, recreate a pokemon game with ease as a proof of concept

# Preface

THIS PROJECT IS NON PROFIT AND OPEN SOURCE.

THIS PROJECT IS FOR EDUCATIONAL PURPOSES ONLY. 

I DO NOT CLAIM TO OWN ANY OF THE POKEMON ART OR MUSIC INCLUDED. 

ALL RIGHTS BELONG TO NINTENDO. 

# Video
Watch most of the playthrough [here](https://streamable.com/vyfhq)!

Or click the preview

[![clip](https://media.giphy.com/media/1WbJank711TIIMmVr4/giphy.gif)](https://streamable.com/vyfhq)

You'll see 3 pokemon-style segues just like from the games! 

You'll see 8+ different pokemon!

You'll see 6+ different moves!

You can level up your pokemon!

You can whiteout!

# Technology
Uses [Swoosh](https://github.com/TheMaverickProgrammer/Swoosh) the mini segue and activity library

SFML 2.5

C++14

_Optional_ - includes visual studio project

# Controls

Main Screen 

* Press Enter

Overworld 

* Arrow keys to move
* Enter to take a snapshot and go to the Main Screen
* Walk in grass for random battle encounters

Battle

* Arrow keys to select move
* Enter to confirm move / proceed textbox
* Back key to leave the battle at any time

# Origins

This demo began because a few SFML community members challenged the capabilities of Swoosh. Could it make segues as complicated as pokemon?
Two flashes and then a unique effect happens before the battle shows up. 

In less than a weekend, I had a demo working with the screen transitions just like pokemons. Then I realized ActionLists are an amazing
building block for turn based games like pokemon.

Two weekends later I had polished the battle engine using primarily ActionLists that are now a part of Swoosh. 

**That is less than 5 days** - I had a near perfect replica of the battle engine for the GBA pokemon games. The polish and speed that Swoosh
brings is incredible. This demo is testament to that.
