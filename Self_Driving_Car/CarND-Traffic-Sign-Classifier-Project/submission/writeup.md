# **Classifying Images in a German Traffic Sign Data Set** 

## Writeup on the technical challenges and approach

**Develop a CNN to classify roadsigns**
The goals / steps of this project were the following:
* Develop a CNN that can classify images with an accuracy of atleast 0.93 with the test set
* Run the CNN on randomly downloaded images from the internet and calculate accuracy


[//]: # (Image References)

[image1]: ./traffic-signs-data/SampleImages.png "Sample 43 Images"

[image2]: ./traffic-signs-data/Category_plot.png "Category plot"

[image3]: ./traffic-signs-data/preprocessedImage.png "Pre-processed Image set"

[image4]: ./traffic-signs-data/RightOfWay.png "Right of way"

[image5]: ./traffic-signs-data/Speed30.png "Speed 30"

[image6]: ./traffic-signs-data/Speed60.png "Speed 60"

[image7]: ./traffic-signs-data/Speed70.png "Speed 70"

[image8]: ./traffic-signs-data/WorkAhead.png "Work Ahead"


---

This is the second project of the udacity self driving car nano-degree. It is the first neural network that I have successfully trained
and used to classify objects.

---
# H1 Contents:

- Dataset summary & Exploration of dataset
- Preprocessing techniques used
- Data Augmentation
- Model Architecture
- Model Training
- Performance on new Images
- Model probabilities ( softmax predictions )


---
## H2 
Data Set Summary & Exploration of dataset

1. The data set contains about 50000 images of various road signs in about 43 different categories. A description of the data set can be found at the following location:

http://benchmark.ini.rub.de/?section=gtsrb&subsection=dataset

2. The images for this project were taken from 

https://d17h27t6h515a5.cloudfront.net/topher/2017/February/5898cd6f_traffic-signs-data/traffic-signs-data.zip

3. THe following image shows one image of the 43 different categories

![Sample Images][image1]

4. The image set is split as follows:

- Training set: 34799 images
- Validation set: 4410 images
- Test set: 12630 images

5. Since the training set is used for training the neural network, the set was further explored to find number of images in each category. The following plot shows
the number of images in each category:

![Category plot][image2]

As can be seen from above images, there are two problems:
1. The images aren't the best for recognition ( some seem very dark and devoid of any visible features ) - This requires preprocessing.
2. In some categories the number of images is less - This requires data set augmentation.

---
Preprocessing techniques used

1. First the images were converted to YUV color space. As mentioned in the lessons, RGB color provides no real value during classification.

Formula taken from: https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale

Idea taken from: https://stackoverflow.com/questions/12201577/how-can-i-convert-an-rgb-image-into-grayscale-in-python


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
