#!/usr/bin/perl

use strict;
use warnings;

if (scalar(@ARGV) != 2)
{
	die "Usage: run_all_tests.pl <app> <suffix>";
}

my $app = $ARGV[0];
my $suffix = $ARGV[1];

my @tests = split(/\s+/, `find tests -type f`);

for my $test (sort @tests)
{
	next if $test !~ m/\.t\.1$/;

	print "Running $test...\n";

	my $test_base = $test;
	$test_base =~ s/\.t\.1$//;

	my $command = "scripts/run_one_test.sh $app $suffix $test_base";

	print "$command\n";
	my $sys_ret = system($command);
}

