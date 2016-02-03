#!/bin/perl
use warnings;
use strict;
use List::Util qw(max);
use GA;
$| = 1;

my @parents;
my @result_children;
my @result_points;
my $player = $ARGV[0];
if(!defined($player)) {
    die "arg0 is not set\n";
}

for(my $i=0; $i<10; $i++) {
    push @parents, &generate_gene();
}
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
my $log_file = "logs/p"i.$player."_".($mon+1)."m".$mday."d_".$hour."h".$min."m".$sec.".log";
open(OUT, ">> $log_file") or die "$!";

for(my $i=0; $i<500; $i++) {
    my @children;
    my @merits;
    @result_points = ();
    @result_children = ();
    @children = &GA::cross_parents(@parents);
    for (my $j=0; $j<$#children+1; $j++) {
        @merits = &gene_to_merits($children[$j]);
        `echo $merits[0] > evolution/load.gen`;
        for(my $k=1; $k<9; $k++) { 
            `echo $merits[$k] >> evolution/load.gen`;
        }

        if($player == 0) {
            `manager/gameManager -a players/filePlayer -p "" -u "" -n "greedy0" -r 1 -s 100 -a players/greedyPlayer1 -p "" -u "" -n "greedy1" -r 2 -s 98 -a players/greedyPlayer2 -p "" -u "" -n "greedy2" -r 3 -s 70 -a players/greedyPlayer3 -p "" -u "" -n "greedy3" -r 3 -s 60 -a players/greedyPlayer4 -p "" -u "" -n "greedy4" -r 3 -s 50 -a players/greedyPlayer5 -p "" -u "" -n "greedy5" -r 3 -s 40 > result.txt`;
        } elsif ($player == 1) {
            `manager/gameManager -a players/greedyPlayer0 -p "" -u "" -n "greedy0" -r 1 -s 100 -a players/filePlayer -p "" -u "" -n "greedy1" -r 2 -s 98 -a players/greedyPlayer2 -p "" -u "" -n "greedy2" -r 3 -s 70 -a players/greedyPlayer3 -p "" -u "" -n "greedy3" -r 3 -s 60 -a players/greedyPlayer4 -p "" -u "" -n "greedy4" -r 3 -s 50 -a players/greedyPlayer5 -p "" -u "" -n "greedy5" -r 3 -s 40 > result.txt`;
        } else {
            `manager/gameManager -a players/greedyPlayer0 -p "" -u "" -n "greedy0" -r 1 -s 100 -a players/greedyPlayer1 -p "" -u "" -n "greedy1" -r 2 -s 98 -a players/filePlayer -p "" -u "" -n "greedy2" -r 3 -s 70 -a players/greedyPlayer3 -p "" -u "" -n "greedy3" -r 3 -s 60 -a players/greedyPlayer4 -p "" -u "" -n "greedy4" -r 3 -s 50 -a players/greedyPlayer5 -p "" -u "" -n "greedy5" -r 3 -s 40 > result.txt`;
        }

        my $point;
        open(IN, "result.txt");
        while(<IN>) {
            if($_ =~ /\[(.*)\]/) {
                my @scores = split(/,/, $1);
                $point = $scores[0]; 
                last;
            }
        }
        close(IN);
        unlink("result.txt");

        push @result_children, $children[$j];
        push @result_points, $point;
    }
    @parents = ();
    for(my $mi=0; $mi<5; $mi++) {
        my $ma = max(@result_points);
        for(my $mj=0; $mj<$#result_points+1; $mj++) {
            if($result_points[$mj] == $ma) {
                push @parents, $result_children[$mj];
                print OUT $result_points[$mj].", ".$result_children[$mj]."\n";
                $result_points[$mj] = 0;
                last;
            }
        }
    }
    # @children = GA::cross_parents(@parents);

}

sub generate_gene(){
    my $code  = "";
    for(my $i=0; $i<9; $i++) {
        my $num = int(rand(64));
        $code .= sprintf "%06b", $num;
    }
    return $code;
}

sub gene_to_merits{
    my ($gene) = @_;
    my @merits = ();
    for(my $i=0; $i<9; $i++) {
        push @merits, unpack("C", pack("B8", "00".substr($gene, $i*6, 6)));
    }

    return @merits;
}

sub trim_space {
    my $val = shift;
    $val =~ s/^ *(.*?) *$/$1/;
    return $val;
}

