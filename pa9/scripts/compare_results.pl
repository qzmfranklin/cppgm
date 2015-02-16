#!/usr/bin/perl

use strict;
use warnings;

if (scalar(@ARGV) != 2)
{
	die "Usage: compare_results.pl <ref_suffix> <my_suffix>";
}

my $ref_suffix = $ARGV[0];
my $my_suffix = $ARGV[1];

my @tests = split(/\s+/, `find tests -type f`);

my $ntests = 0;
my $npass = 0;

sub getdata
{
	my $file = shift @_;

	my $data = `cat $file`;
	chomp($data);
	return $data;
}

for my $test (sort @tests)
{
	next if $test !~ m/\.t.1$/;

	print "\n$test: ";

	$ntests++;

	my $testbase = $test;

	$testbase =~ s/\.t.1$//;

	my $ref = "$testbase.$ref_suffix";
	my $ref_impl_exit_status = "$ref.impl.exit_status";
	my $ref_program_exit_status = "$ref.program.exit_status";
	my $ref_program_stdout = "$ref.program.stdout";

	my $my = "$testbase.$my_suffix";
	my $my_impl_exit_status = "$my.impl.exit_status";
	my $my_program_exit_status = "$my.program.exit_status";
	my $my_program_stdout = "$my.program.stdout";

	if (getdata($ref_impl_exit_status) ne getdata($my_impl_exit_status))
	{
		print "ERROR: $testbase: implementations exit statuses do not match (.impl.exit_status)\n";
		print "TEST FAIL\n";
		exit(1);
	}
	elsif (getdata($ref_impl_exit_status) ne "0")
	{
		$npass++;
		print "TEST PASS\n";
	}
	elsif ((getdata($ref_program_exit_status) eq getdata($my_program_exit_status)) and
		(getdata($ref_program_stdout) eq getdata($my_program_stdout)))
	{
		$npass++;
		print "TEST PASS\n";
	}
	else
	{
		print "ERROR: $testbase: generated programs do not match in exit status and/or output\n";
		print "TEST FAIL\n";
		exit(1);
	}
}

print "ALL TESTS PASS\n";

