ifconfig wlan0 | grep "inet addr" | cut -d ":" -f2 | cut -d " " -f1