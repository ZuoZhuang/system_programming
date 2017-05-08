echo "Remove previous temp folder"

rm -rf temp*

make

./server 12346
