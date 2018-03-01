# this script install deps needed to compile LGML from source :
# ** /!\ /!\ /!\ /!\ gcc-5 will be installed for c++14 support /!\ /!\ /!\ /!\  (on ubuntu <16 for example)
# ** JUCE is downloaded in a sibling directory of LGML
# ** JUCE deps also
# this script support cross-compiling setting the CROSS_ARCH flag to armhf (see circleci config.yml)

set +e # ignore apt update error
NATIVE_ARCH=`dpkg --print-architecture`
if [ -z ${CROSS_ARCH+x} ]; then CROSS_ARCH="$NATIVE_ARCH"; fi
if [ "$CROSS_ARCH" != "$NATIVE_ARCH" ]; then echo "adding foreing arch $CROSS_ARCH"; dpkg --add-architecture $CROSS_ARCH;apt-get -qq update; fi
echo "arch is set to '$CROSS_ARCH'"

set -e # un-ignore apt update error

## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
# from Makefile alsa freetype2 libcurl x11 xext xinerama
apt-get -y --assume-yes install libfreetype6-dev:$CROSS_ARCH \
libx11-dev:$CROSS_ARCH        \
libxinerama-dev:$CROSS_ARCH    \
libxrandr-dev:$CROSS_ARCH      \
libxcursor-dev:$CROSS_ARCH     \
mesa-common-dev:$CROSS_ARCH     \
libasound2-dev:$CROSS_ARCH      \
freeglut3-dev:$CROSS_ARCH      \
libxcomposite-dev:$CROSS_ARCH   \
libjack-dev:$CROSS_ARCH          \
libcurl4-openssl-dev:$CROSS_ARCH \
ladspa-sdk:$CROSS_ARCH

# libavahi-compat-libdnssd libfreetype6 libx11 libxinerama libxrandr libxcursor mesa-common libasound2 freeglut3 libxcomposite libjack libcurl4-openssl

apt-get -y -q --assume-yes install curl


echo "checking if gcc5"
# Compare multipoint versions
check_min_version () {
  local BASE=$1
  local MIN=$2

  # Break apart the versions into an array of semantic parts
  local BASEPARTS=(${BASE//./ })
  local MINPARTS=(${MIN//./ })

  # Ensure there are 3 parts (semantic versioning)
  if [[ ${#BASEPARTS[@]} -lt 3 ]] || [[ ${#MINPARTS[@]} -lt 3 ]]; then
    VERSION_CHECK_ERROR="Please provide version numbers in semantic format MAJOR.MINOR.PATCH."
    return
  fi

  # Compare the parts
  if [[ ${BASEPARTS[0]} -lt ${MINPARTS[0]} ]] || [[ ${BASEPARTS[0]} -eq ${MINPARTS[0]} && ${BASEPARTS[1]} -lt ${MINPARTS[1]} ]] || [[ ${BASEPARTS[0]} -eq ${MINPARTS[0]} && ${BASEPARTS[1]} -eq ${MINPARTS[1]} && ${BASEPARTS[2]} -lt ${MINPARTS[2]} ]]; then
    VERSION_CHECK_ERROR="Minimum version required is $MIN.  Your's is $BASE."
  fi
}

set +e 
# we are using c++14 features now
check_min_version `gcc -dumpversion` "4.9.0"

if [[ -n "$VERSION_CHECK_ERROR" ]]; then
  echo -e "$VERSION_CHECK_ERROR"
  echo "gcc version is too low, JUCE for c++14 support"
  # modifying gcc is likely to break things
  echo `gcc -dumpversion`
  exit 1
  #add-apt-repository ppa:ubuntu-toolchain-r/test
  echo "deb http://ftp.us.debian.org/debian unstable main contrib non-free" >> /etc/apt/sources.list.d/unstable.list
  apt-get update
  apt-get install -y --force-yes -t unstable gcc-5 g++-5
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
fi

set -e


SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

if [ ! -d "JUCE" ]; then
  curl -L https://github.com/julianstorer/JUCE/archive/master.tar.gz > JUCE.tar.gz
  tar -xf JUCE.tar.gz
  mv JUCE-master/ JUCE
  echo "downloading JUCE"
else
  echo "using system JUCE"
fi

apt-get -y -q --assume-yes install python3
apt-get -y -q --assume-yes install python3-pip

# rubberband
apt-get -y -q --assume-yes install librubberband-dev:$CROSS_ARCH
# for dns utility
apt-get -y -q --assume-yes install libavahi-compat-libdnssd-dev:$CROSS_ARCH


# cd ;
# if [ ! -d "Dev/Projucer/linux" ]; then
#   cd 
#   ls

#   cd Dev/JUCE/extras/Projucer/Builds/LinuxMakefile/
#   make -j2
#   cd 
#   pwd
#   mkdir -p Dev/Projucer
#   mkdir -p Dev/Projucer/linux
#   mv Dev/JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer Dev/Projucer/linux/
# fi


