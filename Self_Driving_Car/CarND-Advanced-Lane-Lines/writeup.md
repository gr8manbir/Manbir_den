# **Advanced lane finding project** 

## Writeup on the technical challenges and approach

**Advanced lane finding project using a polynomial fit**
The goals / steps of this project were the following:
* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.


[//]: # (Image References)

[image1]: ./output_images/Camera_calibration_1.png "Camera Calibration Image"

[image2]: ./output_images/Image_undistortion.png "Undistorting an Image"

[image3]: ./output_images/Image_warping.png "Warped Image"

[image4]: ./output_images/Image_thresholding.png "Sobel Threshold Image"

[image5]: ./output_images/Image_HLS.png "Colour thresholds for Line detection"

[image6]: ./output_images/Image_combinedThresholding.png "Combined thresholds"

[image7]: ./output_images/Image_LaneDetection.png "Lane detection"

[image8]: ./output_images/Image_LaneDetection_slidingwindow.png "Lane detection with sliding window"

[image9]: ./output_images/Image_LaneMapped_straight.png "Final Lane mapped straight lines"

[image10]: ./output_images/Image_LaneMapped_curved_yellow1.png "Final Lane mapped yellow on bright road"

[image11]: ./output_images/Image_LaneMapped_curved_yellow2.png "Final Lane mapped yellow on dark road"

---

This is the fourth project of the first tem of Udacity's self driving car nano-degree. In this I used computer vision to do the following:

1) Use the provided camera chess board images for camera calibration.

2) Undistort the provided images using the camera calibration from step 1.

3) Warp the undistorted images to make it easier to detect lane lines.

4) Try and optimize the various sobel thresholds to detect lanes in an image as much as possible.

5) Try and optimize the various color thresholds to detect lanes in an image as much as possible.

6) Optimize the combination of sobel and color thresholds to best detect lanes in an image.

7) Develop an approach to detect lane pixels and consequently fit them to a left and right lane curvature.

8) From the lane curvature, calculate radius of curvature and distance from center.

9) Create a pipeline to perform all above steps sequentially on an image.

10) Run the pipeline on images streamed from a video file.


---


## Camera Calibration:

An image in a real-world is captured by a camera through a lens. This causes two issues:

1) The light rays tend to bend differently close to the edges of a lens. 

2) The camera might not be in the same 2-D plane as the object being captured. 

The above two issues cause a captured image to appear different from the real world. This can cause issues for us as it is very important to accurately detect car lanes. Else our self driving car might not be able to keep itself in lane accurately. To overcome this the approach followed is as follows:

1) From the given chessboard images, detect chessboard corners. This is a 9 by 6 chessboard.

2) A mesh grid of 9 by 6 is created which acts as the best case 9 by 6 grid.

3) The detected chessboard corners are mapped to the mesh grid corners to obtain the "Camera Matrix" and "Distortion co-effecients" of the camera besides other parameters. 

Two CV2 functions were used:

1) cv2.findChessboardCorners(): Returns a list of corners( image points) from a grayscaled image of a 9 by 6 chessboard.

2) cv2.calibrateCamera(): Returns camera matrix, distortion co-effecients etc. when passed a set of image points and object points( from mesh grid ). 

Result: 20 images were provided in the project. Only 17 could accurately be used to detect chessboard corners. which in hindsight appeared sufficient as the project works now.

Following are the 17 images that worked

![Chessboard corners][image1]


---

## Undistorting a image

A real world 3D image does not map very well to a captured 2D image. This is because there is no way to accurately represent a z-dimension on a 2D plane. As a result, some inaccuracies tend to develop in a captured image. For example:

1) Objects further away i.e. larger z value tend to appear smaller.

2) Objects close to edges tend to get distorted in shape.

This is again a cause of concern as we want an accurate image of the road to be able to detect road lanes properly. Accuracy is not possible if image is distorted. 

The opencv function cv2.undistort() was used to undistort images. This takes the image, camera matrix and distortion co-effecients as input and returns an undistorted images. 

Following was the result of undistort operation applied on sample images. THe difference is not apparent at first. To observe the difference, consider the following examples:

1) The size of the signboard on the bottom right of the first pair of images differs in size.

2) The tree on the left of the road in the third set of images

![Undistorted images][image2]


---

## Warping an image

An image as captured is not the best approach to detect road lanes. The reason being that the lane lines tend to converge to a single point in a distance. In real world the lane lines are actually parallel.

The plane from which the road lane is viewed is known as it's perspective. In this section my goal was to view the image from a "Bird-eye" perspective i.e. from top-down so that the road lanes appear parallel. Since I do not have the image from bird-eye perspective, the only possible approach is to perspective-transform or warp the image to the desired perspective. TO do this I did the following:

1) Identify four points on the road so that they form a polygon on the original image. These are marked as '+' in the left handside image.

2) Map the four points as four corners of a desired shape on the desired image ( bird-eye view ). These are '+'in the right handside image.

3) Use open CV function cv2.getPerspectiveTransform to get a perspective transformation matrix from the points described above.

4) Use open CV function cv2.warpPerspective to warp the image from front view to bird's eye view using the transformation matrix.

![Warped image][image3]


---

## Sobel Thresholds

Sobel thresholds is used to detect edges in an image. This basically works by specifying a minimum and maximum threshold for gradients of each pixel in an image.


The goal of this threshold identification step was to:

1) Identify which of the following sobel thresholds is relevant for lane detection:
	a. Gradient threshold in X axis: Detect lane lines( or portions thereof ) by measuring the rate of change of gradients for pixels along x axis
	b. Gradient threshold in Y axis: Detect lane lines( or portions thereof ) by measuring the rate of change of gradients for pixeks along Y axis
	c. Magnitude of gradient: Detect lane lines( or portions thereof ) by measuring the magnitude of gradient of pixels
	d. Direction of gradient: Detect lane lines( or portions thereof ) by measuring the angle of gradient of pixels

2) Identify best possible threshold values for each of the above 4 sobel parameters.

3) Identify the combination of the abovethat can best detect lanes ( or portions thereof ).

 
All the above are done by:
1) Taking a wapred image
2) Using opencv function cv2.sobel to identify x and y pixel gradients.
3) Performing the mathematical operation necessary. For ex: for magnitude of gradient np.sqrt(sobelx**2 + sobely**2).
4) Identifying pixels within threshold limits and discarding the rest.
5) Hit and trial to find best possible thresholds for each of the four parameters.
6) Hit and trial to find the best possible combination of threshold parameters.


Result:

1) Gradient in Y direction is useless for our project goal.
2) For gradient in X and magnitude threshold limits (30,180) work best.
3) For direction of gradient: thresholds( 0.3, 1.3) work best. Values below 0.3 add too much noise.
4) The best possible combination is (grad_x == 1) | ((mag_binary == 1) & (dir_binary == 1))
5) Even the best possible combination is useless in detecting curved lanes ( Note: we will use colour thresholds for this ).

![Sobel thresholds][image4]


The plane from which the road lane is viewed is known as it's perspective. In this section my goal was to view the image from a "Bird-eye" perspective i.e. from top-down so that the road lanes appear parallel. Since I do not have the image from bird-eye perspective, the only possible approach is to perspective-transform or warp the image to the desired perspective. TO do this I did the following:

1) Identify four points on the road so that they form a polygon on the original image. These are marked as '+' in the left handside image.

2) Map the four points as four corners of a desired shape on the desired image ( bird-eye view ). These are '+'in the right handside image.

3) Use open CV function cv2.getPerspectiveTransform to get a perspective transformation matrix from the points described above.

4) Use open CV function cv2.warpPerspective to warp the image from front view to bird's eye view using the transformation matrix.

![UnWarped image][image3]
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
