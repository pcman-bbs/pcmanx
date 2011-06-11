#! /usr/bin/env perl
# -*- coding: utf-8; indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*-
# vim:fileencodings=utf-8:expandtab:tabstop=4:shiftwidth=4:softtabstop=4

use 5.010;
use diagnostics;
use strict;
use warnings;

my @big5 = ();
my @utf16 = ();

while (<>) {
    next if /^#/;
    s/0x//g;
    my ($big5, $utf16) = split;
    push @big5, $big5;
    push @utf16, $utf16;
}

print <<"ENDLINE";
#include <glib.h>

ENDLINE

print "static const gint uao_table_size = ", scalar @big5, ";\n\n";

print "static const gchar big5[] = {\n";

foreach (@big5) {
    my @hex = split //;
    printf "\t0x%s%s, 0x%s%s,\n", @hex;
}
print "};\n\n";

print "static const gunichar2 utf16[] = {\n";

foreach (@utf16) {
    my @hex = split //;
    printf "\t0x%s%s%s%s, 0x0,\n", @hex;
}
print "};\n\n";

print <<"ENDLINE";
gchar* uao(const gchar* input)
{
    return NULL;
}
ENDLINE

1;
