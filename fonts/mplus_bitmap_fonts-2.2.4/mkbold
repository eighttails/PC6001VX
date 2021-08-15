#!/usr/bin/perl
#
#  -- makes BDF font bold
#	programmed by NAGAO, Sadakazu <snagao@cs.titech.ac.jp>	
#		* public domain *
#

$bdir = 1;			# left
$pile = 0;			# right

for $opt (@ARGV) {
    if ($opt =~ /^-/) {	# option
	$bdir = 0 if $opt eq '-r';	# right
	$bdir = 1 if $opt eq '-l';	# left
	$pile = 0 if $opt eq '-R';	# right
	$pile = 1 if $opt eq '-L';	# left
	next;
    } else {
	$file = $opt;
	last;
    }
}
die unless $file;
open F, $file || die;

while (<F>) {

    if (/^BITMAP/) {
	$bitmap = 1;
	print;
	next;
    }

    if (/^ENDCHAR/) {
	$bitmap = 0;
	print;
	next;
    }

    if ($bitmap) {
	chop;
	$l = (length($_) / 2) - 1;

	if ($bdir) {		# left
	    for $i (0..$l) {
		$d[$i] = hex(substr($_, $i*2, 2));
	    }
	    shl(\@bold, \@d, $l);
	} else {
	    for $i (0..$l) {	# right
		$bold[$i] = hex(substr($_, $i*2, 2));
	    }
	    shr(\@d, \@bold, $l);

	}

	if ($pile) {		# left
	    shl(\@tmp, \@bold, $l);
	    bitcalc(\@d, \@tmp, \@d, \@bold, $l);
	} else {		# right
	    shr(\@tmp, \@d, $l);
	    bitcalc(\@d, \@tmp, \@bold, \@d, $l);
	}

	# print
	for $i (0..$l) {
	    printf "%02x", $d[$i];
	}

	print "\n";
	next;
    }

    print;

}
close F;
exit 0;

sub shl {
    my ($dout, $din, $size) = @_;
    my $c = 0, $d, $i;
    for ($i = $size; $i >= 0; $i--) {
	$d = $c;
	$c = ($din->[$i] & 0x80) >> 7;
	$dout->[$i] = (($din->[$i] << 1) & 0xff) | $d;
    }
}

sub shr {
    my ($dout, $din, $size) = @_;
    my $c = 0, $d, $i;
    for $i (0..$size) {
	$d = $c;
	$c = ($din->[$i] & 0x01) << 7;
	$dout->[$i] = ($din->[$i] >> 1) | $d;
    }
}

sub bitcalc {
    my ($out, $d1, $d2, $d3, $size) = @_;
    for $i (0..$size) {
	$out->[$i] = ~$d1->[$i] & $d2->[$i] | $d3->[$i];
    }
}
