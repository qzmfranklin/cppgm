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
	next if $test !~ m/\.t\.1$/;

	print "Running $test...\n";

	my $test_out = $test;
	$test_out =~ s/\.t\.1$/\.$suffix/;
	my $test_base = $test;
	$test_base =~ s/\.t\.1$/\.t/;

	my $command = "scripts/run_one_test.sh $app $test_out $test_base";

	my $sys_ret = system($command);
	if ($sys_ret == 0)
	{
		system("echo EXIT_SUCCESS > $test_out.exit_status");
	}
	else
	{
		system("echo EXIT_FAILURE > $test_out.exit_status");
	}
}

