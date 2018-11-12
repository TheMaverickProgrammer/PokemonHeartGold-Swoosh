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

[TmxLite](https://github.com/fallahn/tmxlite) - you may need to build and provide your own DLLs when building the project from source

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
* Hold Spacebar to speed up battle in real-time by 2.5x!

# The Engine

The battle system is lightweight and boasts the hardest part about pokemon battles: turn order, decision making, and proper timing. This engine, while small, does solve those problems. You can extend the engine to include other pre-battle steps like choosing items, switching pokemon, etc. with just a few custom action items.

The battle system uses an ActionList with custom [action items](https://github.com/TheMaverickProgrammer/PokemonHeartGold-Swoosh/blob/master/ExampleDemo/Swoosh/BattleActions.h) that can be used as individual pieces to make more complex moves. 

Adding pokemon is as easy as defining them in the [data header file](https://github.com/TheMaverickProgrammer/PokemonHeartGold-Swoosh/blob/master/ExampleDemo/Swoosh/Pokemon.h#L51) and loading their art and sounds in the [load step](https://github.com/TheMaverickProgrammer/PokemonHeartGold-Swoosh/blob/master/ExampleDemo/Swoosh/DemoActivities/BattleScene.h#L297).
Adding movesets is the same way. Once you add a move in the data header file you queue up all the action items in the [`decideBattleOrder`](https://github.com/TheMaverickProgrammer/PokemonHeartGold-Swoosh/blob/master/ExampleDemo/Swoosh/DemoActivities/BattleScene.h#L196) step and watch the battle unfold!

As small as it is, there are lots of areas for refactoring that could reduce the amount of work to register new moves, pokemon, and their corresponding action items. For instance, if you add scripting you can associate the actions from the script file and have a truly flyweight engine. Who doesn't like to write less code?

# Origins

This demo began because a few SFML community members challenged the capabilities of Swoosh. Could it make segues as complicated as pokemon?
Two flashes and then a unique effect happens before the battle shows up. 

In less than a weekend, I had a demo working with the screen transitions just like pokemons. Then I realized ActionLists are an amazing
building block for turn based games like pokemon.

Two weekends later I had polished the battle engine using primarily ActionLists that are now a part of Swoosh. 

**That is less than 5 days** - I had a near perfect replica of the battle engine for the GBA pokemon games. The polish and speed that Swoosh
brings is incredible. This demo is testament to that.
