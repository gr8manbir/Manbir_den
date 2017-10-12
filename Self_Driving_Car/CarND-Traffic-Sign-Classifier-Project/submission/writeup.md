# **Classifying Images in a German Traffic Sign Data Set** 

## Writeup on the technical challenges and approach

**Develop a CNN to classify roadsigns**
The goals / steps of this project were the following:
* Develop a CNN that can classify images with an accuracy of atleast 0.93 with the test set
* Run the CNN on randomly downloaded images from the internet and calculate accuracy


[//]: # (Image References)

[image1]: ./traffic-signs-data/RightOfWay.png "Right of Way"

[image2]: ./traffic-signs-data/Speed30.png "Speed 30"

[image3]: ./traffic-signs-data/Speed60.png "Speed 60"

[image4]: ./traffic-signs-data/Speed70.png "Speed 70"

[image5]: ./traffic-signs-data/WorkAhead.png "Work Ahead"

[image6]: ./traffic-signs-data/SampleImages.png "Sample Images"


---

This is the second project of the udacity self driving car nano-degree. It is the first neural network that I have successfully trained
and used to classify objects.

---
Contents:

- Dataset summary
- Exploration of dataset
- Preprocessing techniques used
- Data Augmentation
- Model Architecture
- Model Training
- Performance on new Images
- Model probabilities ( softmax predictions )

The below section describes how I went about detecting lanes in a car driving video. I divided it into two sections:

1. First step is to build a pipeline that takes an image of a vehicle being driven on the road and detect lanes from that image.
Following are two examples out of the five images. The first one is very simple
![Simple car on road][image1]

The second one is a bit more complex
![Bit more complex][image2]

My pipeline consists of the following steps using CV2.

	a. Taking an image file and converting it from RGB to grayscale and HSV. 
	The grayscale helped in discovering white lanes and HSV is better( not necessary ) for discovering yellow lanes
	Following is the output of HSV image

![HSV image][image3]

	b. Merging the grayscale image and HSV image so that we have one combined image with both white and yellow lanes.
	After this I applied Gaussian blur. This helps in removing noise( ultimately less wobbly lanes )

	c. The output image of gaussian blur is subjected to canny edge detection to identify edges.

	d. Since we are only interested in the region of the image which describes lanes, the output of canny edge 
	detection is masked over a rectangular polygon. The resultant image is shown below:

![Masked][image4]

	e. Next step is to take the masked image and find hough lines from it. This step requires a lot of
	parameter tuning. Final result( not the same example as point d. ) is shown below:

![Hough Lines][image5]

	f. The next step is to draw the lines over the original image. This required a lot of fine tuning to
	draw smooth lines over the real image. Some of the steps are described below:
	Parse through the list of detected hough lines and do the below for each line
		i. Check if the slope is -ve(left lane) or +ve(right lane).
		ii. If the slope meets a certain minimum threshold, add it to a list
		i.e. either left lane list or right lane list
		iii. If at the end of parsing through hough lines list, either left or right lane list is empty,
		return error i.e.no detectable lanes exist
		iv. Subject the detected left lane list and right lane list to a normal distribution
		i.e. value of slope < abs( mean slope - 2 * std. deviation )
		Any slope not a part of the normal distribution is rejected.
		v. Calculate left and right mean slopes again after rejecting outliers
		vi. Find mean (x,y)left and (x,y)right from respective lanes
		vii. From the mean slope and corresponding (x,y), find the mean left and right intercepts
		viii. Find end points of left and right lane from the slope, intercepts.
		ix. Using the detected co-ordinates, plot lines over the detected road lanes 
	
	g. After repetitive improvement of parameters, following were some of the
	processed images ( highly satisfying :) )

![Final Result][image6]

![Another one][image7]

2. Next step was to apply the above pipeline to a video stream. A video stream is nothing but a stream of images.
In the above pipeline, the following modification was needed

	a. After the first image has been displayed with the lanes detected, save it globally.
	b. In subsequent images, do a smoothening by taking 80% of the old image and 20% of the new image
	c. Repeat the above till the entire stream has been processed.

Below is the result of the above exercise being applied to detect lanes in a video. Clicking on below will open a new window.
Click on "View Raw" after thatto download and play video.

![Straight Lanes](./test_videos_output/solidYellowLeft.mp4)

![Curved Lanes](./test_videos_output/challenge.mp4)


The project successfully detects lanes in a video stream even with the challenge video where the road curves a bit. 


One potential shortcoming possibly is that the above algorithm might not be able to detect a lane merge from an on-ramp. Also this hasn't 
been tested on adverse weather/night conditions.


A possible improvement would be to fit the lane detection better over less detectable lanes/break in lanes. Also testing on night time driving
or under rain/snow would be interesting.
