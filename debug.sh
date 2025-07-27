if [ ! -d "./out" ]; then
    mkdir "./out"
fi
gcc -shared -fPIC -o ./out/moniterLog.so moniterLog.c -ldl
gcc main.c -o ./out/main -lxcb
LD_PRELOAD=./out/moniterLog.so ./out/main


