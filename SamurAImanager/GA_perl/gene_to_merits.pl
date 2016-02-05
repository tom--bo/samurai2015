#!/bin/perl
use warnings;
use strict;
use List::Util qw(max);
use GA;
$| = 1;

my @merits = ();
my $arg = $ARGV[0];
@merits =  &gene_to_merits($arg);

for my $i(@merits) {
    print $i.", ";
}
print "\n";

1;

sub gene_to_merits{
    my ($gene) = @_;
    my @merits = ();
    for(my $i=0; $i<9; $i++) {
        push @merits, unpack("C", pack("B8", "00".substr($gene, $i*6, 6)));
    }

    return @merits;
}


