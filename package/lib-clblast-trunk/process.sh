#! /bin/bash

#
# Installation script for CLBlast.
#
# See CK LICENSE for licensing details.
# See CK COPYRIGHT for copyright details.
#
# Developer(s):
# - Grigori Fursin, 2015;
# - Anton Lokhmotov, 2016.
#

# PACKAGE_DIR
# INSTALL_DIR

export CLBLAST_SRC_DIR=${INSTALL_DIR}/src
export CLBLAST_BUILD_DIR=${INSTALL_DIR}/build/release

echo ""
echo "Getting CLBlast from GitHub ..."
rm -rf ${CLBLAST_SRC_DIR}
git clone ${CLBLAST_URL} --no-checkout ${CLBLAST_SRC_DIR}
if [ "${?}" != "0" ] ; then
  echo "Error: Getting CLBLast failed!"
  exit 1
fi

echo ""
echo "Checking out the '${CLBLAST_BRANCH}' branch of CLBlast ..."
cd ${CLBLAST_SRC_DIR}
git checkout ${CLBLAST_BRANCH}
if [ "${?}" != "0" ] ; then
  echo "Error: Checking out the '${CLBLAST_BRANCH}' branch of CLBLast failed!"
  exit 1
fi

echo ""
echo "Building the '${CLBLAST_BRANCH}' branch of CLBlast ..."
mkdir -p ${CLBLAST_BUILD_DIR}
cd ${CLBLAST_BUILD_DIR}
cmake ${CLBLAST_SRC_DIR} -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=${CK_CC} -DCMAKE_CXX_COMPILER=${CK_CXX} \
  -DOPENCL_LIBRARIES:FILEPATH=${CK_ENV_LIB_OPENCL_LIB}/${CK_ENV_LIB_OPENCL_DYNAMIC_NAME} \
  -DOPENCL_INCLUDE_DIRS:PATH=${CK_ENV_LIB_OPENCL_INCLUDE} \
  -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
make install
if [ "${?}" != "0" ] ; then
  echo "Error: Building the '${CLBLAST_BRANCH}' branch of CLBlast failed!"
  exit 1
fi

#export LIB_NAME=libclblast
