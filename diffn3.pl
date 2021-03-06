#! /usr/local/bin/perl

# diffn3
# C. DeTar 26 Mar 2005

# Compares two files line-by-line and reports lines with differences
# in numeric fields when they exceed a tolerance and lines in
# nonnumeric fields when they are not exactly the same.  Tolerances
# are specified by a third, error tolerance file.
# If the error tolerance line begins with "XXXX" the entire line
# is ignored.
# If the error tolerance field is XXX, that field is ignored.

# Usage...

#    diffn file1 file2 errfile

# where file1 and file2 are to be compared
# and errfile is the error tolerance file
# A discrepancy is reported when abs(field1 - field2) > tol

sub is_integer {
    defined $_[0] && $_[0] =~ /^[+-]?\d+$/;
}

sub is_float {
    defined $_[0] && $_[0] =~ /^[+-]?\d+(\.\d*)?$/;
}

sub is_scientific {
    defined $_[0] && $_[0] =~ /^[+-]?\d+(\.\d*)?[eEdDg][+-]?(\d+)$/;
}

sub is_number {
    # Allow comma or right paren after number
    my $a = $_[0];
    defined $a && $a =~ s/[,\)]$//;
    is_integer($a) || is_float($a) || is_scientific($a);
}

($file1,$file2,$errfile) = @ARGV;

(defined($errfile) && defined($file2) && defined($file1)) || 
    die "Usage $0 <file1> <file2> <errfile>\n";

print "$0 $file1 $file2 $errfile\n";

open(FILE1,$file1) || die "Couldn't open $file1: $!";
open(FILE2,$file2) || die "Couldn't open $file2: $!";
open(ERR,$errfile) || die "Couldn't open $errfile: $!";

($n1, $rest) = split(' ',`wc -l $file1`);
($n2, $rest) = split(' ',`wc -l $file2`);
($ne, $rest) = split(' ',`wc -l $errfile`);

$n1 == $n2 && $n1 == $ne or die "ERROR: File line counts must match";

$lines = 0;

$difflines = 0;
while($line1 = <FILE1>){
    chop($line1);
    if(!($line2 = <FILE2>))
    {
	die "Premature end of file on $file2\n";
    }
    chop($line2);
    if(!($errline = <ERR>))
    {
	die "Premature end of file on $errfile\n";
    }
    chop($errline);
    @fields1 = split(/[ \t\n]+/,$line1);
    @fields2 = split(/[ \t\n]+/,$line2);
    @errs = split(/[ \t\n]+/,$errline);
    # Ignore the entire line if errline begins with XXXX
    if($errs[0] eq "XXXX"){next;}
    $i = 0;
    $same = 1;
    @list1 = @fields1;
    @list2 = @fields2;
    if($#fields2 > $#fields1){
	@list1 = @fields2;
	@list2 = @fields1;
    }
    for(@list1)
    {
	$tol = $errs[$i];
	$diff = abs($_ - $list2[$i]);
	# Allow any difference if errline field is XXX
	# Otherwise nonumeric or zero fields should match exactly
	# And nonzero numeric fields should differ by less than the tolerance
	if( $tol ne "XXX" &&
	    (((!is_number($list2[$i]) || $list2[$i] eq "nan" || $_ eq "nan") &&
	     ($_ ne $list2[$i])) ||
	     $diff > $tol ))
	{
	    $same = 0;
	    $field = $i+1; $line = $lines+1;
	    $diffround = sprintf("%.2g",$diff);
	    print "Field $field Line $line diff $diffround >= tol $tol\n";
	}
	$i++;
    }
    if(!$same)
    {
	print "< ",$line1,"\n";
	print "> ",$line2,"\n";
	$difflines++;
    }
    $lines++;
}

if(<FILE2>)
{
    die "$file2 is longer than $file1\n";
}

if(<ERRBACK>)
{
    die "$errfile is longer than $file1\n";
}

if($difflines == 0){
    printf STDERR "$file1 OK\n";
}
else{
    printf STDERR "$file1 NOT OK $difflines lines differ\n";
}

