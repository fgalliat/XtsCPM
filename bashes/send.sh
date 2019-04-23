fileN="$1"
if [ "$fileN" = "" ];
then
  echo "need a filepath";
  exit;
fi

fileP="$fileN"
fileN=$(basename $fileN)

echo "Sending $fileP => $fileN"

output="/dev/tty"
#output="/dev/ttyACM0"

echo "a:MBASIC c:RTEXT.BAS" > "$output"
echo "$fileN" > "$output"
cat "$fileP" > "$output"
echo "" > "$output"
echo "-EOF-" > "$output"

# echo "SYSTEM" > "$output"
