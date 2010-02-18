#!/usr/bin/perl

sub xml($) {
	$buffer = $_[0];
	$buffer =~ s/^File: (.*) \(row: (\d+)\) (.*)/\t\<Message file=\"$1\" row=\"$2\"\>\n\t\t$3\n\t\<\/Message\>\n/;
	return $buffer;
}

open(FIN,"doxygen.log");
open(FOUT,">doxygen.log.xml");

print FOUT "<Doxygen>\n";

$buffer = "";
while (<FIN>) {
	chop;
	$row = $_;

	$row =~ s/&/&#38;#38;/g;
	$row =~ s/</&#38;#60;/g;
	$row =~ s/>/&#x003E;/g;

	if ( $row =~ /^File: / ) {
		if (length($buffer)>0) {
			print FOUT xml($buffer);
			$buffer = "";
		} 
		$buffer .= $row;
	} else {
		$buffer .= $row;
	}

}
print FOUT "</Doxygen>\n";

close(FIN);
close(FOUT);



