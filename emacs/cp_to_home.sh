#!/bin/sh

# store script location to be able to refer to .emacs(.d) files no matter where
# the script is called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "This script is stupid and will remove your .emacs file and .emacs.d folder!"
read -p "ARE YOU SURE? [y/N] " promptanswer
if (! ( [ "$promptanswer" = "y" ] || [ "$promptanswer" = "Y" ] ) ); then exit 1; fi

rm -rf ~/.emacs ~/.emacs.d

cp $DIR/emacs ~/.emacs
cp -r $DIR/emacs.d ~/.emacs.d
cd ~/.emacs.d/site-lisp && git clone https://github.com/haskell/haskell-mode.git

read -p "Would you like to run 'cabal update'? (The next few prompts will ask about installing software from hackage.) [Y/n] " promptanswer
if (! [ "$promptanswer" = "n" ] || [ "$promptanswer" = "N" ] ); then cabal update; fi

echo "The Haskell mode can use stylish-haskell to indent code."
echo "Note that this library directly or indirectly depends on the 'happy' package. Please install it prior to continuing with this script."
read -p "Would you like to install this library through cabal? [Y/n] " promptanswer
if (! [ "$promptanswer" = "n" ] || [ "$promptanswer" = "N" ] ); then cabal install stylish-haskell; fi

echo "The Haskell mode can run 'cabal-dev' instead of just ghci as REPL environment."
read -p "Would you like to install cabal-dev? [Y/n] "
if (! [ "$promptanswer" = "n" ] || [ "$promptanswer" = "N" ] ); then for i in {HTTP,network,tar,zlib}; do cabal install $i; done; cd /tmp && rm -rf cabal-dev && git clone https://github.com/creswick/cabal-dev && cd cabal-dev && cabal configure && cabal build && cabal install; fi
