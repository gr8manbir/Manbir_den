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

![HSV image][image3]

My pipeline consists of the following steps using CV2.

	1. Taking an image file and converting it from RGB to grayscale and HSV. 
	   The grayscale helped in discovering white lanes and HSV is better( not necessary ) for yellow lanes
	   Following is the output of HSV image
 	   ![HSV image][image3]

	2. Merging the grayscale image and HSV image so that we have one combined image with both white and yellow lanes.
	   After this I applied Gaussian blur. This helps in removing noise( ultimately less wobbly lanes )

	3. The output image of gaussian blur is subjected to canny edge detection to identify edges.

	4. Since we are only interested in the region of the image which describes lanes, the output of canny edge detection is
	   masked over a rectangular polygon. The resultant image is shown below:
	   ![Masked][image4]



### 2. Identify potential shortcomings with your current pipeline


One potential shortcoming would be what would happen when ... 

Another shortcoming could be ...


### 3. Suggest possible improvements to your pipeline

A possible improvement would be to ...

Another potential improvement could be to ...
