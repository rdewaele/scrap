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
cd ~/.emacs.d/site-lisp && git clone git://github.com/haskell/haskell-mode.git
