# install juce 
# BASEDIR=$(dirname "$0") 
# cd $BASEDIR
# cd ../../..

pwd
ls
cd ;
if [ ! -d "Dev/JUCE" ]; then
  mkdir -p "Dev"
  curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip
  unzip -q JUCE.zip
  mv JUCE-master/ Dev/JUCE
fi

cd ;
if [ ! -d "Dev/Projucer/linux" ]; then
  cd 
  ls

  cd Dev/JUCE/extras/Projucer/Builds/LinuxMakefile/
  make -j2
  cd 
  pwd
  mkdir -p Dev/Projucer
  mkdir -p Dev/Projucer/linux
  mv Dev/JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer Dev/Projucer/linux/
fi


