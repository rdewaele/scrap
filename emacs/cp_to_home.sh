#!/bin/sh

# store script location to be able to refer to .emacs(.d) files no matter where
# the script is called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "This script is stupid and will remove your .emacs file and .emacs.d folder!"
read -p "ARE YOU SURE? [y/N] " is_sure
if (! ( [ "$is_sure" = "y" ] || [ "$is_sure" = "Y" ] ) ); then exit 1; fi

rm -rf ~/.emacs ~/.emacs.d

cp $DIR/emacs ~/.emacs
cp -r $DIR/emacs.d ~/.emacs.d
cd ~/.emacs.d/site-lisp && git clone https://github.com/haskell/haskell-mode.git

echo "The Haskell mode can use stylish-haskell to indent code."
echo "Note that this library directly or indirectly depends on the 'happy' package. Please install it prior to continuing with this script."
read -p "Would you like to install this library through cabal? [Y/n] " cabalrun
if ( [ "$cabalrun" = "n" ] || [ "$cabalrun" = "N" ] ); then exit 1; fi
cabal update
cabal install stylish-haskell
