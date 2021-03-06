# Script used by the build server the build
# all modules of ALPSCore

# This script expects the following environment variables
# BOOST_ROOT - location for boost distribution
# GTEST_ROOT - location for gtest sources/binaries
# HDF5_ROOT - location for the HDF5 distribution

# Make sure we are in top directory for the repository
SCRIPTDIR=$(dirname $0)
cd $SCRIPTDIR/../..

if [[ -z "$BOOST_ROOT" ]]
then
  BOOST_SYSTEM=NO
else
  BOOST_SYSTEM=YES
fi

echo "Using BOOST at $BOOST_ROOT - no system path $BOOST_SYSTEM"

if [[ -z "$HDF5_ROOT" ]]
then
  echo
else
  export DYLD_LIBRARY_PATH=$BOOST_ROOT/lib:$HDF5_ROOT/lib:$DYLD_LIBRARY_PATH
fi

# Function to build one module
function build {
  MODULE=$1
  MODULEDIR=$PWD/$MODULE
  INSTALLDIR=$PWD/install
  BUILDDIR=$PWD/build.tmp/$MODULE

  echo "*** Checking copyright notice ***"
  common/scripts/check-module-copyright-notice.py $MODULE || exit 1

  mkdir build.tmp
  rm -rf $BUILDDIR
  mkdir $BUILDDIR
  cd $BUILDDIR

  echo "*** Building module in $MODULEDIR to $INSTALLDIR ***"

  cmake \
  -DCMAKE_INSTALL_PREFIX="${INSTALLDIR}" \
  -DTesting=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DBOOST_ROOT="${BOOST_ROOT}" \
  -DGTEST_ROOT="${GTEST_ROOT}" \
  -DBoost_NO_SYSTEM_PATHS="${BOOST_SYSTEM}" \
  -DTestXMLOutput=TRUE \
  -DENABLE_MPI=TRUE \
  ${MODULEDIR}

  make || exit 1 
  make test
  make install || exit 1

  cd ../..
}

build utility
build hdf5
build params
build accumulator
build mc

echo "*** Done ***"
