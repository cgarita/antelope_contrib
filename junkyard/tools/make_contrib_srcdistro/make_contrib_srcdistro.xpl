
# make_contrib_srcdistro

use Datascope;

require "getopts.pl" ;

if( ! &Getopts('r:c:') ) {
	die( "Usage: $0 [-r README-file] [-c ftpdir]\n" );
}

$cvsroot = pfget( "make_contrib_srcdistro", "cvsroot" );
$package = pfget( "make_contrib_srcdistro", "package" );
$tarfile = pfget( "make_contrib_srcdistro", "tarfile" );

$day = strdate( str2epoch( "now" ) );

$tarfile_name = epoch2str( str2epoch( "now" ), "$tarfile" );

if( $opt_r ) {
	$readme = abspath( $opt_r );
} else {
	$readme = "$ENV{ANTELOPE}/data/misc/README.contrib";
}

if( $opt_c ) {
	$ftpdir = abspath( $opt_c );
}

$pwd = `pwd`;
chomp( $pwd );

if( $opt_c ) {

	$target_dir = $ftpdir;

} else {

	$target_dir = $pwd;
}

chdir( "/tmp" );
mkdir( "src", 0755 );
chdir( "/tmp/src" );

printf STDERR "Building CVS export distribution in /tmp:\n";
$cmd = "cvs -d $cvsroot export -D $day $package";
system( $cmd );

chdir( "/tmp" );

printf STDERR "Copying readme file from $readme:\n";
$cmd = "/bin/cp $readme README.contrib";
system( $cmd );

printf STDERR "Building tar-file $tarfile_name:\n";
$cmd = "tar cvf $target_dir/$tarfile_name README.contrib src/contrib";
system( $cmd );

printf STDERR "Compressing tar-file $tarfile_name:\n";
$cmd = "/bin/cp $target_dir/$tarfile_name $target_dir/$tarfile_name.orig";
system( $cmd );
$cmd = "compress $target_dir/$tarfile_name";
system( $cmd );
$cmd = "/bin/mv $target_dir/$tarfile_name.orig $target_dir/$tarfile_name";
system( $cmd );
$cmd = "gzip $target_dir/$tarfile_name";
system( $cmd );

system( "/bin/rm -rf /tmp/src" );
