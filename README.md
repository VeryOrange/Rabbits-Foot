# The Rabbits Foot
Ransomware Program to encrypt Linux devices inspired by the BadRabbit ransomware malware.

This program will encrypt all files in a users /home folder. It uses 256 bit AES in CBC mode. 

At the time of writing, this program has only been tested on Linux.

*For test purposes only*

To compile:
- Ensure you have libssl-dev installed on your system
- gcc rabbitsfoot.c -o [output] -lcrypto
- (sudo) chmod a+x [output]
- ./[output]

To decrypt:
- gcc decryptor.c -o [output] -lcrypto
- ./[output]
