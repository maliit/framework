#!/bin/sh
set -e

cd $1

SHA1="unknown"
# check if we are working with a git checkout
if [ -e .git/logs/HEAD ]; then
  # if git is not installed extract sha1 manually
  if $(type -P git > /dev/null); then
    SHA1=$(git log --pretty=format:%H -n 1)
  else
    SHA1=$(awk 'END { print $2 }' .git/logs/HEAD)
  fi
fi

# extract debian version number
DEBIAN_VERSION="unknown"
if $(type -P dpkg-parsechangelog > /dev/null); then \
  DEBIAN_VERSION=$(dpkg-parsechangelog | grep '^Version: ' | sed 's/^Version: \(.*\)/\1/')
fi

echo "Git revision: ${SHA1} Debian Version: ${DEBIAN_VERSION}"
