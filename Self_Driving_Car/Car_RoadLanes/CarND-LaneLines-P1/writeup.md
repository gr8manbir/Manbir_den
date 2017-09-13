# **Finding Lane Lines on the Road** 

## Writeup on the technical challenges and approach

**Finding Lane Lines on the Road**
The goals / steps of this project were the following:
* Make a pipeline that finds lane lines on the road
* Use that pipeline to mark lanes on a video of a car being driven


[//]: # (Image References)

[image1]: ./test_images/solidWhiteCurve.jpg "First Image"

[image2]: ./test_images/whiteCarLaneSwitch.jpg "yellow and white lanes"

[image3]: ./test_images_output/HSVLane "HSV Image"

[image4]: ./test_images_output/Masked "Masked(Region of Interest)"

[image5]: ./test_images_output/img_houghLines "Hough Lines"

[image6]: ./test_images_output/solidWhiteCurve_WithLanes "Lane Detected"

[image7]: ./test_images_output/whiteCarLaneSwitch_WithLanes "Mixed lane detected"

---

This is the first project of the udacity self driving car nano-degree. Hence there were two challenges involved in this project:

1) Setting up the environment for development: Thankfully with past experience in anaconda and jupyter notebooks, this was OK for me.

2) Delivering the project including the challenge video. 


---


The below section describes how I went about detecting lanes in a car driving video. I divided it into two sections:

1. First step is to build a pipeline that takes an image of a vehicle being driven on the road and detect lanes from that image.
Following are two examples out of the five images. The first one is very simple
![Simple car on road][image1]

The second one is a bit more complex
![Bit more complex][image2]

My pipeline consists of the following steps using CV2.

	a. Taking an image file and converting it from RGB to grayscale and HSV. 
	The grayscale helped in discovering white lanes and HSV is better( not necessary ) for yellow lanes
	Following is the output of HSV image

![HSV image][image3]

	b. Merging the grayscale image and HSV image so that we have one combined image with both white and yellow lanes.
	After this I applied Gaussian blur. This helps in removing noise( ultimately less wobbly lanes )

	c. The output image of gaussian blur is subjected to canny edge detection to identify edges.

	d. Since we are only interested in the region of the image which describes lanes, the output of canny edge detection is
	   masked over a rectangular polygon. The resultant image is shown below:

![Masked][image4]

	e. Next step is to take the masked image and find hough lines from it. This step requires a lot of parameter tuning. The
	final result( example ) is shown below:

![Hough Lines][image5]

	f. The next step is to draw the lines over the original image. This required a lot of fine tuning to draw smooth lines over
	the real image. Some of the steps are described below:
	Parse through the list of detected hough lines and do the below for each line
		i. Check if the slope is -ve(left lane) or +ve(right lane).
		ii. If the slope meets a certain minimum threshold, add it to a list( either left lane list or right lane list )
		iii. If at the end of parsing through hough lines list, either left or right lane list is empty, return error ( no detectable lanes exist )
		iv. Subject the detected left lane list and right lane list to a normal distribution ( i.e. value of slope < abs( mean slope - 2 * std. deviation )
		Any slope not a part of the normal distribution is rejected.
		v. Calculate left and right mean slopes again after rejecting outliers
		vi. Find mean (x,y)left and (x,y)right from respective lanes
		vii. From the mean slope and corresponding (x,y), find the mean left and right intercepts
		viii. Find end points of left and right lane from the slope, intercepts.
		ix. Using the detected co-ordinates, plot lines over the detected road lanes 
	
	g. After repetitive improvement of parameters, following were some of the processed images ( highly satisfying :) )

![Final Result][image6]

![Another one][image7]

2. Next step was to apply the above pipeline to a video stream. A video stream is nothing but a stream of images. In the above pipeline, the following 
modification was needed

	a. After the first image has been displayed with the lanes detected, save it globally.
	b. In subsequent images, do a smoothening by taking 80% of the old image and 20% of the new image
	c. Repeat the above till the entire stream has been processed.

Below is the result of the above exercise being applied to detect lanes in a video. Clicking on below will open a new window. Click on "View Raw" after that
to download and play video.

![Straight Lanes](./test_videos_output/solidYellowLeft.mp4)

![Curved Lanes](./test_videos_output/challenge.mp4)


The project successfully detects lanes in a video stream even with the challenge video where the road curves a bit. 


One potential shortcoming possibly is that the above algorithm might not be able to detect a lane merge from an on-ramp. Also this hasn't 
been tested on adverse weather/night conditions.


A possible improvement would be to fit the lane detection better over less detectable lanes/break in lanes. Also testing on night time driving
or under rain/snow would be interesting.
