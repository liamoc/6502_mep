#!/bin/bash
orig_filename=$1
bbc_filename=$2
load=0x$3
exec=0x$4
attr=$5
type=$6
actual_size="$(gstat -c "%s" "$orig_filename")"
printf "\$.$bbc_filename %.6X %.6X %.6X ATTR=$attr TYPE=$type\n" $load $exec $actual_size > $7
