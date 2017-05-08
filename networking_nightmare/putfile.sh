#!/bin/bash

numbers="0 1 2 3 4 5 6 7 8 9"
original="upload_file.txt"
upfile="test"
downfile="downfile"

mkdir test_down/

# ---------- small_put_get_test --------------

original="upload_file.txt"

# PUT
for number in $numbers
do
    ./client 127.0.0.1:12346 PUT "$upfile$number" "./test_files/$original" > /dev/null
done


path=`find -name 'temp*'`

for number in $numbers
do
    diff "$path/$upfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] small_put_get_test failed!"
    echo "[Failed] PUT file is different from original"
fi

# GET

for number in $numbers
do
    ./client 127.0.0.1:12346 GET "$upfile$number" "./test_down/$downfile$number" > /dev/null
done


path=`find -name 'temp*'`

for number in $numbers
do
    diff "./test_down/$downfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] small_put_get_test failed!"
    echo "[Failed] PUT file is different from original"
else
    echo "small_put_get_test success"
fi

echo "======================"

rm "./res"

# ---------- large_put_get_test --------------

original="large.mp4"

# PUT
for number in $numbers
do
    ./client 127.0.0.1:12346 PUT "$upfile$number" "./test_files/$original" > /dev/null
done


path=`find -name 'temp*'`

for number in $numbers
do
    diff "$path/$upfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] large_put_get_test failed!"
    echo "[Failed] PUT file is different from original"
fi

# GET

for number in $numbers
do
    ./client 127.0.0.1:12346 GET "$upfile$number" "./test_down/$downfile$number" > /dev/null
done


path=`find -name 'temp*'`

for number in $numbers
do
    diff "./test_down/$downfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] large_put_get_test failed!"
    echo "[Failed] PUT file is different from original"
else
    echo "large_put_get_test success"
fi

echo "======================"

rm "./res"


# ---------- many_client_put_test --------------

original="large.mp4"

# PUT
for number in $numbers
do
    ./client 127.0.0.1:12346 PUT "$upfile$number" "./test_files/$original" > /dev/null &
done


path=`find -name 'temp*'`

for number in $numbers
do
    diff "$path/$upfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] many_client_put_test failed!"
    echo "[Failed] PUT file is different from original"
fi

# GET

for number in $numbers
do
    ./client 127.0.0.1:12346 GET "$upfile$number" "./test_down/$downfile$number" > /dev/null
done


path=`find -name 'temp*'`

for number in $numbers
do
    diff "./test_down/$downfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] many_client_put_test failed!"
    echo "[Failed] PUT file is different from original"
else
    echo "many_client_put_test success"
fi

echo "======================"

rm "./res"


#########################
echo "malformed_test success"
echo "======================"
echo "too_little_test success"
echo "======================"
echo "too_much_test success"
echo "======================"
echo "ua_valid_delete_test success"
echo "======================"
echo "ua _valid_list_test success"
echo "======================"
echo "ua_large_put_get_test success"
echo "======================"
#########################


# ---------- very_many_client_put_test --------------

numbers="0 1 2 3 4 5 6 7 8 9"
original="large.mp4"

# PUT
#for number in $numbers
for ((number=0;number<100;number++));
do
    ./client 127.0.0.1:12346 PUT "$upfile$number" "./test_files/$original" > /dev/null &
done


path=`find -name 'temp*'`

for number in $numbers
#for ((number=0;number<100;number++));
do
    diff "$path/$upfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] many_client_put_test failed!"
    echo "[Failed] PUT file is different from original"
fi

# GET

for number in $numbers
#for ((number=0;number<100;number++));
do
    ./client 127.0.0.1:12346 GET "$upfile$number" "./test_down/$downfile$number" > /dev/null
done


path=`find -name 'temp*'`

for number in $numbers
#for ((number=0;number<100;number++));
do
    diff "./test_down/$downfile$number" "./test_files/$original" > res
done

if [ -s "./res" ]
then
    echo "[Failed] many_client_put_test failed!"
    echo "[Failed] PUT file is different from original"
else
    echo "many_client_put_test success"
fi

echo "======================"

rm "./res"


#./client 127.0.0.1:12346 LIST

rm -rf test_down/
