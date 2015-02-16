#!/usr/bin/perl

use strict;
use warnings;

if (scalar(@ARGV) != 2)
{
	die "Usage: compare_exit_status.pl <ref_suffix> <my_suffix>";
}

my $ref_suffix = $ARGV[0];
my $my_suffix = $ARGV[1];

my @tests = split(/\s+/, `find tests -type f`);

my $ntests = 0;
my $npass = 0;

for my $test (sort @tests)
{
	next if $test !~ m/\.t.1$/;

	print "\n$test: ";

	$ntests++;

	my $testbase = $test;

	$testbase =~ s/\.t.1$//;

        my $reftest = "$testbase.$ref_suffix";
        my $reftest_exit_status = "$reftest.exit_status";
        my $mytest = "$testbase.$my_suffix";
        my $mytest_exit_status = "$mytest.exit_status";


	my $reftest_exit_status_data = `cat $reftest_exit_status`;
	my $mytest_exit_status_data = `cat $mytest_exit_status`;
	
	chomp($reftest_exit_status_data);
	chomp($mytest_exit_status_data);

	if ($reftest_exit_status_data ne $mytest_exit_status_data)
	{
		print "ERROR: Expected $reftest_exit_status_data, got $mytest_exit_status_data\n\n";
		
		print "TEST FAIL\n";
		exit(1);
	}
	else
	{
		$npass++;
		print "PASS\n\n";
	}
}

print "EXIT STATUS OF REF VS OTHER OK\n";

