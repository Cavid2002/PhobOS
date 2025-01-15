## PHOBOS ##

# How to build: # 

Remote Access application written in C++ with Windows Socket Library. To compile and run, execute `compile.bat` that would create server and client executables. Activate server and then client, server would control client device via standard **CMD** commands.

# Intorduction: #

Server supports multi-user controll multithreading. Server listens to port and creates new thread if new client connects. Each Client is given separate ID number that will be used in combination with command to control that exact client.

```
3mkdir test # would create a new folder in client 3 computer!
```

Server allows control over up to 10(0-9) clients. Additionally server allows download and upload of files, meaning server can transfer any data from clients computer or send additional data.


