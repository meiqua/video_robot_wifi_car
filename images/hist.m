img=imread('up.png'); 
 imageGray=rgb2gray(img); 
 histGray=imhist(imageGray) ;
 
filterAve = fspecial('average');
filterLap = fspecial('laplacian');
filterSob = fspecial('sobel');

grayAve = imfilter(imageGray,filterAve,'replicate');
grayLap = imfilter(grayAve,filterLap,'replicate');
grayLap = abs(grayLap);

graySob = imfilter(grayAve,filterSob,'replicate');
graySobv = imfilter(grayAve,filterSob','replicate');

graySobsqrt = uint8(sqrt(double(graySob.^2+graySobv.^2)));
%ming = min(min(grayLap));
%maxg = max(max(grayLap));
%grayLap = uint8(((grayLap*1.0 - ming))/(maxg-ming)*255);

grayAverOtsu1 = graythresh(graySobsqrt);


figure(1),imshow(grayAve);
figure(2),imhist(grayAve);
grayAverOtsu = graythresh(img);
figure(3),im2bw(grayAve,grayAverOtsu);

figure(4),imshow(grayLap*4);
figure(5),imshow(graySob);
figure(6),imshow(graySobsqrt*10);
figure(7),imhist(graySobsqrt*10);

