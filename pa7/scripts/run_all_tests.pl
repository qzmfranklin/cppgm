#!/usr/bin/perl

use strict;
use warnings;

if (scalar(@ARGV) != 2)
{
	die "Usage: test.pl <app> <suffix>";
}

my $app = $ARGV[0];
my $suffix = $ARGV[1];

my @tests = split(/\s+/, `find tests -type f`);

for my $test (sort @tests)
{
	next if $test !~ m/\.t$/;

	print "Running $test...\n";

	my $test_out = $test;
	$test_out =~ s/\.t$/\.$suffix/;
	my $sys_ret = system("scripts/run_one_test.sh $app $test $test_out");
	if ($sys_ret == 0)
	{
		system("echo EXIT_SUCCESS > $test_out.exit_status");
	}
	else
	{
		system("echo EXIT_FAILURE > $test_out.exit_status");
	}
}

