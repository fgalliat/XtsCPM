iw wlan0 info | grep ssid | sed "s/[\t]//g" | cut -d " " -f2