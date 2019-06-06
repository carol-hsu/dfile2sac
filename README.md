# Dfile-to-SAC file transformer


## Build the docker container

go to the home directory of this repo, and stay at here :octocat:

```
$ docker build -t <CONTAINER_NAME> .
```

## RUN!

1. prepare a directory, `<OUTPUT_DIRECTORY>`, for your output data, i.e. /home/yoyo/outputs
2. prepare your input dfile and NSTA.DAT, put it at home directory
3. run follow command
```
$ docker run -it -v <DFILE_NAME>:/file_transformer/dfile -v <NSTA.DAT>:/file_transformer/NSTA.DAT -v <OUTPUT_DIRECTORY>:/tmp/ <CONTAINER_NAME> dfile2sac dfile <YEAR> <MONTH> <DAY>
```
