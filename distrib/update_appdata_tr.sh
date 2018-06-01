#!/usr/bin/sh

itstool -i as-metainfo.its -o appdata_tr/appdata.pot fmit.appdata.xml.in

for f in appdata_tr/*.po; do
	msgmerge -o $f $f appdata_tr/appdata.pot
done
