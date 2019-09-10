BINFILE=$1

FMITVERSION=`git describe --tags --always |cut -c2-`-github
echo $FMITVERSION
#DISTRIB=`lsb_release -d |sed 's/Description:\s//g' |sed 's/\s/_/g'`
#echo "Distribution: "$DISTRIB
#if [ "${DISTRIB/Ubuntu}" != "$DISTRIB" ] ; then
#    DISTRIB=`echo $DISTRIB |sed 's/\(Ubuntu_[0-9]\+\.[0-9]\+\).*/\1/g'`
#fi
