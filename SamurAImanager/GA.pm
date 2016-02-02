#!/bin/perl
package GA;
use warnings;
use strict;

sub cross_parents {
    my @parents = @_;
    my @children;
    for(my $i=0; $i<$#parents+1; $i++) {
        for(my $j=$i+1; $j<$#parents+1; $j++) {
            push @children, &one_point_crossover($parents[$i], $parents[$j]);
            push @children, &two_point_crossover($parents[$i], $parents[$j]);
            push @children, &uniform_crossover($parents[$i], $parents[$j]);
            push @children, &uniform_crossover($parents[$i], $parents[$j]);
        }
    }
    
    return @children;
}

sub uniform_crossover {
    my ($p1, $p2) = @_;
    my $child = "";
    for(my $i=0; $i<length($p1); $i++) {
        if(rand(10) > 5) {
            $child .= substr($p1, $i, 1);
        }else{
            $child .= substr($p2, $i, 1);
        }
    }

    my $ret = &mutation($child);
    return $ret;
}

sub one_point_crossover {
    my ($p1, $p2) = @_;
    my $rnd = int(rand(54));
    my $child = "";
    $child .= substr($p1, 0, $rnd);
    $child .= substr($p2, $rnd);

    my $ret = &mutation($child);
    return $ret;
}

sub two_point_crossover {
    my ($p1, $p2) = @_;
    my $child = "";
    my $rnd1 = int(rand(53));
    my $rnd2 = int(rand(54-$rnd1));

    $child .= substr($p1, 0, $rnd1);
    $child .= substr($p2, $rnd1, $rnd2);
    $child .= substr($p2, $rnd1+$rnd2);

    my $ret = &mutation($child);
    return $ret;
}

sub mutation {
    my ($gene) = @_;
    my $mutant;
    for(my $i=0; $i<length($gene); $i++) {
        if(rand(10) < 1) {
            my $tmp = substr($gene, $i, 1);
            if($tmp eq "0") {
                $gene = substr($gene, 0, $i)."1".substr($gene, ($i+1), (length($gene)-$i));
            }else{
                $gene = substr($gene, 0, $i)."0".substr($gene, ($i+1), (length($gene)-$i));
            }
        }
    }
    $mutant = $gene;

    return $mutant;
}

1;
