#!/bin/bash

### This script allows to switch between plugins
### Usage: ./switch-maliit-plugin.sh plugin.so [subview]

# TODO:
# - properly append new plugins/subviews to active list
# - reset switch (gconftool-2 --recursive-unset /maliit/onscreen)
# - show available plugins (and probe, if need be)

GCONFTOOL=$(which gconftool-2)

${GCONFTOOL} -s --type list --list-type string /maliit/onscreen/enabled "[${1},${2}]"
${GCONFTOOL} -s --type list --list-type string /maliit/onscreen/active "[${1},${2}]"
