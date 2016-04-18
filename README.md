## video_robot_wifi_car
### function:  
    get video from IP camera or webcam  
>control wifi car to move  
>find direction when move under vehicles by edge detection  
>stitch images to generate a big picture of vehicle bottom(have not achieved yet)  
  
### based on:   
  opencv3 and qt5  
    
### class mythread, myobject, TCPclient:  
  run a TCPsocket in a thread to communicate with remote control IP(format IP:port)  
  
### class imageformat:   
  convert image format of opencv to Qimage  
  
### class settingdialog:  
  set IP address of IP camera or webcam and control address  
  
### class findLines:  
  find direction by edge detection
