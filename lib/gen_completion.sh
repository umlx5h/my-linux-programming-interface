#!/bin/bash

make clean
bear -- make
compdb list | sponge compile_commands.json
cp -f ./compile_commands.json ../
