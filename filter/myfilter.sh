#!/bin/sh
exec sed "s/note:/\nnote:/g" | sed "s/error:/\nerror:/g" | sed 's/\/home\/marius\/cg\/src\///g'
