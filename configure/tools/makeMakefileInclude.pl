eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
	if $running_under_some_shell; # makeMakefileInclude.pl

# $Id$
#
#  Author: Janet Anderson
# 

sub Usage
{
    my ($txt) = @_;

    print "Usage:\n";
    print "\makeMakefileInclude name outfile\n";
    print "\tcp name [ name2 name3 ...] outfile\n";
    print "\nError: $txt\n" if $txt;

    exit 2;
}

# need at least two args: ARGV[0] and ARGV[1]
Usage("need more args") if $#ARGV < 1;

$outfile=$ARGV[$#ARGV];
@nameList=@ARGV[0..$#ARGV-1];

unlink("${outfile}");
open(OUT,">${outfile}") or die "$! opening ${outfile}";
print OUT "#Do not modify this file.\n";
print OUT "#This file is created during the build.\n";

foreach $name ( @nameList ) {
	print OUT "\n";
	print OUT "ifneq (\$(strip \$(${name}_SRCS_\$(OS_CLASS))),)\n";
	print OUT "${name}_SRCS+=\$(subst -nil-,,\$(${name}_SRCS_\$(OS_CLASS)))\n";
	print OUT "else\n";
	print OUT "ifdef ${name}_SRCS_DEFAULT\n";
	print OUT "${name}_SRCS+=\$(${name}_SRCS_DEFAULT)\n";
	print OUT "endif\n";
	print OUT "endif\n";
	print OUT "ifneq (\$(strip \$(${name}_OBJS_\$(OS_CLASS))),)\n";
	print OUT "${name}_OBJS+=\$(subst -nil-,,\$(${name}_OBJS_\$(OS_CLASS)))\n";
	print OUT "else\n";
	print OUT "ifdef ${name}_OBJS_DEFAULT\n";
	print OUT "${name}_OBJS+=\$(${name}_OBJS_DEFAULT)\n";
	print OUT "endif\n";
	print OUT "endif\n";
	print OUT "ifneq (\$(strip \$(${name}_LIBS_\$(OS_CLASS))),)\n";
	print OUT "${name}_LIBS+=\$(subst -nil-,,\$(${name}_LIBS_\$(OS_CLASS)))\n";
	print OUT "else\n";
	print OUT "ifdef ${name}_LIBS_DEFAULT\n";
	print OUT "${name}_LIBS+=\$(${name}_LIBS_DEFAULT)\n";
	print OUT "endif\n";
	print OUT "endif\n";
	print OUT "ifneq (\$(strip \$(${name}_SYS_LIBS_\$(OS_CLASS))),)\n";
	print OUT "${name}_SYS_LIBS+=\$(subst -nil-,,\$(${name}_SYS_LIBS_\$(OS_CLASS)))\n";
	print OUT "else\n";
	print OUT "ifdef ${name}_SYS_LIBS_DEFAULT\n";
	print OUT "${name}_SYS_LIBS+=\$(${name}_SYS_LIBS_DEFAULT)\n";
	print OUT "endif\n";
	print OUT "endif\n";
	print OUT "${name}_OBJS+=\$(addsuffix \$(OBJ),\$(basename \$(${name}_SRCS)))\n";
	print OUT "\n";
	print OUT "depends: \$(${name}_SRCS)\n";
	print OUT "\n";
	print OUT "ifeq (\$(filter ${name},\$(PROD)),${name})\n";
	print OUT "ifeq (,\$(strip \$(${name}_OBJS) \$(PROD_OBJS)))\n";
	print OUT "${name}_OBJS+=${name}\$(OBJ)\n";
	print OUT "endif\n";
	print OUT "${name}_RESS+=\$(addsuffix \$(RES),\$(basename \$(${name}_RCS)))\n";
	print OUT "${name}_DEPLIBS=\$(foreach lib, \$(${name}_LIBS),\$(firstword \$(wildcard \\\n";
	print OUT " \$(\$(lib)_DIR)/\$(LIB_PREFIX)\$(lib)\*)))\n";
	print OUT "${name}\$(EXE): \$(${name}_OBJS) \$(${name}_RESS) \$(${name}_DEPLIBS)\n";
	print OUT "endif\n";
	print OUT "\n";
	print OUT "ifeq (\$(filter ${name},\$(LIBRARY_HOST) \$(LIBRARY_IOC)),${name})\n";
	print OUT "ifneq (,\$(strip \$(${name}_OBJS) \$(LIBRARY_OBJS)))\n";
	print OUT "BUILD_LIBRARY += ${name}\n";
	print OUT "endif\n";
	print OUT "\$(LIB_PREFIX)${name}\$(LIB_SUFFIX):\$(${name}_OBJS)\n";
	print OUT "\$(LIB_PREFIX)${name}\$(SHRLIB_SUFFIX):\$(${name}_OBJS)\n";
	print OUT "endif\n";
	print OUT "\n";
}
close OUT or die "Cannot close $outfile: $!";

