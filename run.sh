if [ ! -d "./out" ]; then
    mkdir "./out"
fi
gcc main.c -o ./out/main -lxcb
./out/main
