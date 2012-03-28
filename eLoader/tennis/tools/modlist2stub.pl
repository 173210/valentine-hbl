# Required input file :
#
# "modlist.txt", which is the the output of the "modlist"
# command in PSPLink


use feature ':5.10';
use strict 'vars';


 
open(my $fh, "<", "modlist.txt") or die "ERROR: Cannot open the file modlist.txt\n";

say STDERR "Working...";


# Parse the entire file
while (!eof($fh))
{
	my $line = readline $fh;
	say '"', substr($line,35, length($line)-35-1), '",';
}



close($fh);

say STDERR "Done!";
