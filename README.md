# nanos-ui

This project is an external version of the Nano S User Interface which can be loaded as a specific application - it can be used to personalize most generic parts of the user experience and the most appreciated experiments can be pushed into the official firmware. 

This application shall be installed in Recovery mode - to enter recovery mode, start your Nano S with the right button pushed until "Recovery" is displayed on screen. To install the application use

```
make load
```

As the UI application is quite large and we don't properly defragment yet, you may have to delete other applications in order to load it. A 6a84 error will be reported if not enough space is available on the Nano S. 

To recover from an application error or just remove the UX, enter Recovery mode again and delete the application with 

```
make delete
``` 


