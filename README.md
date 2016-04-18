## video_robot_wifi_car
### function:  
* get video from IP camera or webcam  
* control wifi car to move  
* find direction when move under vehicles by edge detection  
* stitch images to generate a big picture of vehicle bottom(not achieved yet)  

### based on:   
* opencv3 and qt5  
    
### class mythread, myobject, TCPclient:  
* run a TCPsocket in a thread to communicate with remote control IP(format IP:port)  
  
### class imageformat:   
* convert image format of opencv to Qimage  
  
### class settingdialog:  
* set IP address of IP camera or webcam and control address  

### class testParamset:  
* set some param for test
  
### class findLines:  
* find direction by edge detection
    * mainly depend on two features: large gradient and large brightness
    * steps:
        * find gradient threshold using otsu method
        * find edge by canny using the gradient threshold
        * find brightness threshold around **edge** using otsu method
        * find edge by binary image using the threshold of brightness
        * combine both edge
        * remove unwanted edge
            * gradient angle greater than 30 degree(because we believe direction will not change more than 30)
            * gradient direction greater than 0 in the Top half, smaller than 0 in the bottom half(because we know background is white and vehicle bottom is black)
        * repeat above steps for one time, just change first step a little bit: find gradient threshold using otsu method around **edge**(this step will enhance the detected edge because we use more accurate edges)
        * find lines, using hough metnod
        * find two clusters of lines, one is the top half, the other is the bottom half.
        * predict lines by one line in top half and one line in bottom half.
              
