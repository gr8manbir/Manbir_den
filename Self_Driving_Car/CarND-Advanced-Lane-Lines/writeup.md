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

[image12]: ./test_images/test1.jpg "Yellow lane on a bright road"

[image13]: ./test_images/test2.jpg "Yellow lane on a dark road"


---

This is the fourth project of the first term of Udacity's self driving car nano-degree. In this I used computer vision to do the following:

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
1) Taking a warped image
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

---

## Color Thresholds

A normal image is usually in RGB space. This though sufficient for the human eye is not sufficient for mathematically detecting lanes. The biggest drawbacks of using RGB
image space are:

1) It doesn't work very well for lanes in a shaded area

2) It detects yellow lines differently depending on road brightness. This was the biggest hurdle I faced in the project. As an example, refer the below images
for example of yellow lanes on bright and dark road. This is almost impossible to reliably detect in RGB space.

![Yellow Light][image12]

![Yellow Dark][image13]


Color threshold step was used to detect edges in an image. This basically works as follows:

1) Breaking an RGB image down into its R, G and B components
2) Converting the image to HLS color space.
3) Breaking the HLS image down into its H, L and S components
4) Identifying which component works best to detect lanes across "all" sample images
5) Identifying what threshold values and combination of images work best

Result:

1) All colors in RGB space are useless for project goals. They work for some images but not all.
2) S, L color space work best for all images.
3) The best combination is: (S>120 & L>40) | (L>205)

The above result can be easily understood as seen from below sample image:

![Color thresholds][image5]


## Combined Thresholds

This is a relatively simple step as we have already separately identified sobel thresholds and color thresholds. So in this step we only combined the both and
verified it against images to see if combined thresholds can decently identify all components of a road lane.


Not much modification was necessary from that already done in sobel and color threshold sections. The below image shows the lane detection combined. The green pixels 
on the road lane have been detected by sobel thresholds and the blue pixels by L,S colors from HLS color space.

Result:

1) Straight lines are best detected by L,S colors
2) Curved and short lanes are best detected by Sobel thresholds


![Combined thresholds][image6]


## Lane detection and curve fitting

This step consists of following mathematical steps:

1) Separating the image into left and right halfs on x axis. This is done to detect left lane in left half and right lane in right half of the image.
2) Fitting a histogram to identify peaks on bottom left and bottom right portions of an image. These peaks serve as the base for left and right lanes.
3) Create an empty list of left lane pixels and right lane pixels.
4) Starting with the base for each lane create a set of windows ( image is split into 9 windows totally ) for each lane. 
5) Taking left lane as an example: If a non-zero image pixel falls within the left lane window and is within a margin( 100 pixels )from the current left base: add it to the left lane pixel list.
6) Update left base as the mean of the left lane pixel list.
7) Repeat the steps 5 and 6 for all 9 vertical windows in the image, dynamically re-centering the window based on pixel mean. ( Do the same for right lane also ). 


After the pixels have been found for left and right lanes, the next steps are as follows:

1) Fit a polynomial on the left and right image pixel list. This is done via the following function call:

left_fit = np.polyfit(lefty, leftx, 2)

lefty, leftx are the non-zero pixels on the left lane from the left lane pixel list.

Similarly do the same for right lane.


2) From the fitted function, obtain a list of values that lie on the lane curve. For this, we will use the curve co-effecients detected from the np.polyfit call.
This can be seen as below:

    left_fitx = left_fit[0]*ploty**2 + left_fit[1]*ploty + left_fit[2]
    right_fitx = right_fit[0]*ploty**2 + right_fit[1]*ploty + right_fit[2]


The above is hard to understand textually. The following image shows the above in a pictorial form. In the top right hand side image:

1) The red pixels identify the left lane pixels ( left lane pixel list ).

2) The blue pixels identify the right lane pixels.

3) The green rectangles represent the 9 windows the image was divided into.

4) The yellow lines represent the result of np.polyfit drawn using left_fitx and right_fitx.

![Sliding window][image8]


##calculating radius curvature and distance of car from center

This step in volved calculating the radius of the lane(s) if any. As a car turns left or right, the car lanes form a cicular curve. The goal of this step was to identify how
much was the radius of the curve. 

An  article describing this mathematically can be found at:

http://mathworld.wolfram.com/RadiusofCurvature.html

The mathematical equation relevant for us is 
R​curve = ​((1+(2Ay+B)^2)^​3/2)/ |2A|

Here A and B are the co-effecients of curve fit. Previously we had fit a curve in Lane detection step and the pixel values were stored in
left_fitx and right_fitx. In this step, we do the same fitting but not on pixel values but real distance values.

The mapping of pixel values is as follows:

ym_per_pix = 30/720 # meters per pixel in y dimension

xm_per_pix = 3.7/700 # meters per pixel in x dimension

Now we can simply multiply left_fitx and right_fitx with xm_per_pix to get actual x co-ordinate values in meters. 
The y-value(s) are obtained by multiplying random y values with ym_per_pix.

Then these newly calculated values are fit onto a curve by using np.polyfit() function as follows

left_fit_cr = np.polyfit(ploty*ym_per_pix, left_fitx*xm_per_pix, 2)
right_fit_cr = np.polyfit(ploty*ym_per_pix, right_fitx*xm_per_pix, 2)

These left_fit_cr and right_fit_cr can now be used to calculate RCurve. This is "Radius of Curvature".

Now calculating distance of car from center is easy. We know left_fitx and right_fitx contain pixel values for left and right lane. We simply subtract the last value(s) in this array
and divide by two to get width of road. Since the camera is in center of car, we can divide image size in x direction by 2 to get position pixel of car in X axis.

Sutracting lane width from car position gives how far is the car from center of the lane.
​​

## Pipeline creation

The image pipeline simply consists of calling the above steps in sequential order i.e.:

1) Calibrate the camera( done only once )
2) Undistort the image
3) Process the image for thresholds( sobel and color ) to identify lane pixels
4) Transform image perspective to bird-eye
5) From the bird's eye perspective identify lane curvature using sliding window approach.
6) Use open CV functions to colour the detected lanes
7) Calculate radius of curvature and distance from center

The result of this pipeline can be seen in below processed sample images:

![Straight line][image9]

![Yellow lane bright road][image10]

![Yellow lane dark road][image11]


Next step was to apply the above pipeline to a video stream. A video stream is nothing but a stream of images.

Below is the result of the above exercise being applied to detect lanes in a video. Clicking on below will open a new window.
Click on "View Raw" after that to download and play video.

![Video](./test_videos_output/project_video.mp4)

Due to shortage of time, I was unable to work on the challenge videos unfortunately. Some of the problems, I wish to further work on:

1) Challenge videos
2) I had a hard time detecting lanes from HSV and RGB images. Probably need to check with more threshold combinations
3) What if the parallel lanes are intersected by another set of lanes( let's say a 4 way crossing/ stop line ) etc.

I am not sure if this lane detection in x axis will always be guaranteed. Also there could be a possibility of having very weak or no lanes at all.
