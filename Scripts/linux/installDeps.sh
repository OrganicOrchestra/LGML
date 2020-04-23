#!/usr/bin/env bash


#https
#sudo apt-get -y install libcurl4-gnutls-dev

# rubberband depending on platforms name differ, 2v5 for old platforms

FOUND=""
for RLIB in 'librubberband2v5' 'librubberband2' ; do
	RUB=`apt-cache search -n $RLIB  | grep  "$RLIB -" | xargs `
	echo "rub is $RUB"
	if [ "$RUB" ]; then
				echo found $RUB
				echo installing $RLIB
		 		apt-get -y --assume-yes install $RLIB
		 		FOUND=1
		 		break
	else
		continue
	fi
done

if [ ! $FOUND ]; then
	echo not found rubberband , available are :
	echo `apt-cache search librubberband`
	exit 1
else
	echo successfully installed rubberband
	exit 0
fi



# for dns utility
#apt-get -y --assume-yes install libavahi-compat-libdnssd1

