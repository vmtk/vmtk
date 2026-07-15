#!/usr/bin/env perl
# One-time migration: convert legacy VTK-style header comments
# (// .NAME / // .SECTION Description / // Description:) into native
# Doxygen syntax (/** @class @brief ... */ and ///@{ ... ///@}), matching
# the convention used by upstream VTK. Rewrites files in place.

use Getopt::Long;
use Fcntl;
use File::Find;
use Text::Wrap;
use strict;

my %args;
GetOptions(\%args, "dry-run");

my @dirs = @ARGV;
die "usage: $0 dir [dir...]\n" unless @dirs;

my @files;
foreach my $dir (@dirs) {
    find(sub { push @files, $File::Find::name if /vtk[^\\\/]*\.h\z/; }, $dir);
}

my $nb_converted = 0;

foreach my $source (@files) {
    open(my $fh, '<', $source) or die "unable to open $source: $!\n";
    my @headerfile = <$fh>;
    close($fh);

    next unless grep { /\/\/ \.NAME/ } @headerfile;

    my @converted = ();
    my $line;

    while ($line = shift @headerfile) {
        last if $line =~ /\/\/ \.NAME/;
        next if $line =~ /^\s*Date:\s*/;
        next if $line =~ /^\s*Version:\s*/;
        if ($line =~ /^\s*Thanks:\s*/) {
            while ($line = shift @headerfile) { last if $line =~ /^\s*$/; }
            next;
        }
        push @converted, $line;
    }

    die "$source: malformed .NAME line\n"
      unless $line =~ /\/\/ \.NAME (\w*)( \- (.*))?/;
    my ($class_name, $brief) = ($1, $3);

    push @converted, "/**\n";
    push @converted, " * \@class   $class_name\n";
    push @converted, " * \@brief   $brief\n" if $brief;

    my ($tag, $inblock) = ("", 0);
    while ($line = shift @headerfile) {
        last if $line =~ /^\#/;

        if ($line =~ /^\/\/\s+\.SECTION\s+(.+)\s*$/i) {
            my $type = $1;
            if ($type =~ /Bugs?/i) { $tag = "\@bug"; }
            elsif ($type =~ /(Caveats|Warnings?)/i) { $tag = "\@warning"; }
            elsif ($type =~ /Description/i) { $tag = ""; }
            elsif ($type =~ /Note/i) { $tag = "\@attention"; }
            elsif ($type =~ /See Also/i) { $tag = "\@sa"; }
            elsif ($type =~ /Todo/i) { $tag = "\@todo"; }
            else { $tag = "\@par " . $type . ":"; }
            $inblock = 0;
            push @converted, " *\n";
            next;
        }

        if ($line =~ /^\/\/(.*)/) {
            my $remaining = $1;
            $remaining =~ s/\s+$//;
            if ($remaining =~ /\S/) {
                if ($tag ne "" && !$inblock) {
                    push @converted, " * $tag\n";
                }
                $remaining =~ s/^\s//;
                push @converted, " *" . ($remaining eq "" ? "" : " $remaining") . "\n";
                $inblock = 1;
            } else {
                push @converted, " *\n";
                $inblock = 0;
            }
        }
    }

    push @converted, " */\n\n";
    push @converted, $line;

    while ($line = shift @headerfile) {

        if ($line =~ /^(\s*)\/\/\s*De(s|c)(s|c)?ription:?\s*$/) {
            my $indent = $1;
            $Text::Wrap::columns = 100;

            my @description = ();
            while ($line = shift @headerfile) {
                last if $line !~ /^\s*\/\//;
                chomp $line;
                $line =~ s/^\s*\/\/\s*//;
                $line =~ s/\s*$//;
                push @description, $line;
            }

            my @declarations = ();
            while (defined($line) && $line =~ /\S/) {
                push @declarations, $line;
                $line = shift @headerfile;
            }

            my $enclose = (scalar(@declarations) > 1
                            || (@declarations && $declarations[0] =~ /vtk\w*Macro/));

            push @converted, "$indent///\@{\n" if $enclose;
            if (@description) {
                my $wrapped = wrap("", "", join(" ", @description));
                push @converted, "$indent/**\n";
                foreach my $wline (split /\n/, $wrapped) {
                    push @converted, "$indent * $wline\n";
                }
                push @converted, "$indent */\n";
            }
            push @converted, @declarations;
            push @converted, "$indent///\@}\n" if $enclose;
        }

        push @converted, $line if defined $line;
    }

    if ($args{"dry-run"}) {
        print "would convert: $source\n";
    } else {
        open(my $out, '>', $source) or die "unable to write $source: $!\n";
        print $out @converted;
        close($out);
    }
    ++$nb_converted;
}

print "$nb_converted file(s) converted.\n";
