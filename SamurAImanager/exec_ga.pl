#!/bin/perl
use warnings;
use strict;
use GA;

my @parents;
my @result_parents;
my @result_points;

for(my $i=0; $i<3; $i++) {
    push @parents, &generate_gene();
}

exit;
#??????????????? exit 

for(my $i=0; $i<1000; $i++) {
    my @children;
    my @merits;
    @children = GA::generate_children(@parents);
    for (my $j=0; $j<$#children+1; $j++) {
        @merits = &gene_to_merits();
        `echo $merits[0] > load.gen`;
        for(my $k=1; $k<8; $k++) { 
            `echo $merits[$k] >> load.gen`;
        }

        `manager/gameManager -a players/filePlayer -p "" -u "" -n "greedy0" -r 1 -s 100 -a players/greedyPlayer1 -p "" -u "" -n "greedy1" -r 2 -s 98 -a players/greedyPlayer2 -p "" -u "" -n "greedy2" -r 3 -s 70 -a players/greedyPlayer3 -p "" -u "" -n "greedy3" -r 3 -s 60 -a players/greedyPlayer4 -p "" -u "" -n "greedy4" -r 3 -s 50 -a players/greedyPlayer5 -p "" -u "" -n "greedy5" -r 3 -s 40 -t`;

        my $point;
        open(IN, "file.gen");
        while(<IN>) {
            $point = chomp($_);
        }
        close(IN);

        push @result_parents, $parents[$j];
        push @result_points, $point;
    }
    @parents = ();
    for(my $mi=0; $mi<3; $mi++) {
        my $ma = max(@result_points);
        for(my $mj=0; $mj<$#result_points+1; $mj++) {
            if($result_points[$mj] == $ma) {
                push @parents, $result_parents[$mj];
                $result_points[$mj] = 0;
            }

        }
    }
    @children = GA::cross_parents(@parents);

}

sub generate_gene(){
    my $code  = "";
    for(my $i=0; $i<8; $i++) {
        my $num = int(rand(64));
        $code .= sprintf "%06b", $num;
    }
    return $code;
}

sub gene_to_merits{
    my ($gene) = @_;
    my @merits;
    for(my $i=0; $i<8; $i++) {
        push @merits, unpack("C", pack("B8", "00".substr($gene, $i*6, 6)));
    }

    return @merits;
}

