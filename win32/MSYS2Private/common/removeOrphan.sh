#!/bin/bash

#使われていないパッケージを削除
pacman -Rns $(pacman -Qtdq)
