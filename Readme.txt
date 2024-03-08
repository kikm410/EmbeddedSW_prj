Embedded System Software [CSE4116] Project

Student ID : 20181598
Name : Kwangmin Kim

Driver name : numbaseball
Driver major number : 242

How to 'make' :
    i) type 'make' in module directory to compile
    ii) type 'make push' in module directory to compile and push to
        /data/local/tmp (make clean will be automatically done after push)

In target board :
    1. type 'echo "7    6   1   7" > /proc/sys/kernel/printk' to see printk messages
    2. type 'insmod numbaseball.ko'
    3. type 'chmod 777 /dev/numbaseball'
    4. run android application program

Check Document.pdf for program description