# **Vehicle detection project** 

## Writeup on the technical challenges and approach

**Detecting cars in a video stream**

The goals / steps of this project were the following:
* Extract features( HOG, spatial, histogram ) from a set of images
* Train a classifier for classifying images as car/non-car.
* Decide on a combination of features which gives the best classification.
* Given a window region in an image, detect cars using the trained classifier.
* Reduce the number of false positives using a heatmap.
* Detect lane pixels and fit to find the lane boundary.
* Apply the above to a video stream to detect cars.


[//]: # (Image References)

[image1]: ./output_images/Cars_visualization.png "Data set examples"

[image2]: ./output_images/Feature_visualization.png "HOG features"

[image3]: ./output_images/rectangles_visualization.png "Sliding window rectangles"

[image4]: ./output_images/cars_noheatmap.png "Cars detection"

[image5]: ./output_images/heatmap_nothreshold.png "heat map cars"

[image6]: ./output_images/heatmap_threshold.png "heat map cars threshold"

[image7]: ./output_images/labelled_image.png "labelled images"

[image8]: ./output_images/final_image.png "Final result"


---

This is the fifth project of the first term of Udacity's self driving car nano-degree. In this I used computer vision to do the following:

1) Load an image set and visualize it.

2) Extract useful features from the images.

3) Train an SVM classifier using the features to classify car/non car.

4) Optimize the feature parameters to improve classification score.

5) Try and optimize the various color thresholds to detect lanes in an image as much as possible.

6) Use sliding window approach to break an image down into regions most likely to contain cars.

7) Detect cars using the above approach.

8) Reduce number of false positives.

9) Implement the above on a video stream.


---


## Load an image set and visualize it:

This project requires two set of images

1) A suitably sized collection of images of cars
2) A collection of images of road features most commonly seen.

The above images are taken from a GTI vehicle database and KITTI vision benchmark suite. This was provided by udacity as following two zip files:

cars: https://s3.amazonaws.com/udacity-sdc/Vehicle_Tracking/vehicles.zip

non-cars: https://s3.amazonaws.com/udacity-sdc/Vehicle_Tracking/non-vehicles.zip

This contains 8792 images of cars and 8968 non-car images. This can be seen in the following example visualization.


![Dataset visualization][image1]


---

## Extract useful features from an image

The approach used for this project was to use a technique called Histogram of gradients. This implies:

1) Dividing an image into cells. For eg: 8*8 pixels form one cell in a 64*64 image
2) Calculating the gradients of each pixel in a cell and storing them in a 2D array
3) Unravelling the 2D array into a normalized 1D array
4) Generating a histogram of the gradients array( called HOG ). This is done as higher the gradient, more will be its contribution to its histogram bin.
5) Normalize the HOG of a 2*2 group of cells called a block.

Since the pixel gradients of a car are very different from pixel gradients of a non-car road feature, this can act as a input feature to a 
classifier for machine learning. For this project, to generate HOG, we use a library function provided by scikit in skimage.

More details on this step are provided in the section on optimizing the parameters used for generating features. The following visualization shows HOG features extracted from one car and one non-car images. 

![HOG features][image2]


---

## Training an SVM classifier

This step is fairly straightforward. The goal of this step is to generate a Linear SVM machine learning classifier based on the image features extracted in the previous step. We use the package sklearn.svm from scikit library.

The steps involved are:

1) Since we use a concatenation of two image features (HOG and spatial bins of colors), we have to scale the features. This is done as both are on a different scale.
2) Split the image set into training and validation set in ratio 80:20.
3) Generate a classifier using the training set.
4) Calculate the classifier's performance(accuracy) on a test set

The accuracy of the classifier and it's performance on test images works as a feedback mechanism for tuning the parameters used to generate feature list. This is the next step of this project. 

Also to avoid having to re-train the classifier each time, the classifier and scaler are stored as pickled files.

---

## Optimizing parameters to improve classification

The goal of this step is to experiment with the parameters used to create the features( HOG and spatial color bins ) in the first step. A detailed explanation
on the parameters used and the optimized values is given below:

1. color_space = HSV. Can be RGB, HSV, LUV, HLS, YUV, YCrCb. RGB and HLS though useful failed to classify images of cars under certain conditions. LUV, YCrCb did not work well with the normalization to zero mean. YUV was not tried.

2. orient = 12. This tells the number of bins pixel gradients in a 8*8 cell must be binned into. 9 and 12 worked well. theorectically 12 should be better as it provides more bins but no significant improvement was noticed.

3. pix_per_cell = 8. Number of pixels in a cell. This worked well and was not experimented with.

4. cell_per_block = 2. Number of cells to be combined to form a block. This worked well and was not experimented with.

5. hog_channel = 'ALL'. ALL means using all channels of the image to generate HOG features. Alternative was to use either H, S or V from an HSV image. Using ALL channels gave a bigger feature set and hence is used.

6. spatial_size = (24, 24). This is used for spatial binning of colours. Basically a 64*64 image is re-sized into 24*24 image and then unrolled into a 1D array. This is then concatenated with the HOG feature set to generate a final feature set. classroom uses 32*32. I experimented with 16*16, 24*24 and 32*32. 24*24 seemingly gave the best performance.

7. hist_bins = 32. Other than HOG and spatial another feature set is also possible by generating a histogram over the individual pixel colors ( not gradients ) in each of the three channels of an image. This feature gave no signficant improvement and is hence not used.

8. spatial_feat = True. Tells if we are to use spatial binning of pixels. Yes it is used.

9. hist_feat = False. Histogram over color pixels is not used in this project.

10. hog_feat = True. Histogram of pixel gradients is used.

The above list is obtained after a lot of experimentation both at the time of generating a classifier and later using the classifier to locate cars in an image. Using the above combination gave an accuracy score of 99%, so it seems decent. 

Since the image sample( car/non-car ) set is very small and constains several serial images from a stream, this classifier maybe prone to overfitting. But we do not have a better choice unless we could capture more images.

---

## sliding windows over an image of interest

In this, I used the same function ( find_cars ) as described in class with a few minor changes. The following is what the function does:

1) Generate a sub-region in an image most likely to contain cars. Here the y values corresponding to the top and bottom of the sub-image region are variable (  but between 390 and 650 ). But we start the with the x value for left as 450. We are unlikely  to contain cars in any region other than this.

2) Transform the image into the same color space as used in the classification i.e. HSV.

3) Re-size the sub-image to a scale. This is helpful as the car will appear smaller far from ht camera and biger closer to the camera. So re-scaling helps in the following manner. If an image is scaled at a > 1 scale( say 2 ), it will be compressed by half. Then the block chosen, when resized ( to 64 * 64 ) will preseve the original scaled image. This will help in detecting big cars i.e. cars true to size and hence closer to camera. If re-sized to a lower scale ( say 0.5 ), it will greatly magnify the block and thus help in searching for cars further away from the camera i.e. those that appear smaller.

4) From the scaled-sub image calculate the HOG for all 3 channels. Note, as this sub-image is a sub-region from a main image (step 1), we are calculating HOG over a small region of the actual image.

5) We then divide the sub-image into blocks( of pixel hogs ). This block size is based on pixels_per_cell and block_size.

6) Now this block is re-sized to 64*64 to exactly match the image shape of image set used to train the SVM classifier.

7) We now run the already trained scaler/classifier over this block to predict if it is a car/non-car. 

8) If it is a car, we save the bounding box co-ordinates.

9) We repeat this for all the pixel_per_cell size blocks. Note after we finish a block, we do not skip the entire pixel count in that axis. We keep an overlap to improve detection. 

## Detect cars using sliding window approach

In the previous step, we implemented a sliding window function to provide input images for a classifier. In this step we decide which scale to use for sliding window and over which region of the image.

We require a smaller scale at top of the sub-image and a larger scale to search for bigger cars at the bottom of the sub-image. So we used the following combinations of y-regions and scale:

1) 
y_start=380
y_end=480
scale=1.00

2) 
y_start=420
y_end=550
scale=1.25

3)
y_start=430
y_end=570
scale=1.50

4)
y_start=460
y_end=610
scale=1.75

5)
y_start=470
y_end=690
scale=2.00

I have also printed the above bounding boxes on a sample image to visualize it.

![sub sample region][image3]


Using the above approach, I got a decent detection of cars in a sample test image. This is shown as below:

![Car detection][image4]


## Reducing False positives

In the above image, it can be seen that a lot of positives were generated i.e. regions containing no cars were still marked as containing cars. The goal of this step is to reduce the number of false positives.

We do this by generating a heat map and then applying a threshold for minimum number of heat counts for a particular pixel. This can be explaied as below:

1) For each pixel within a bounding box, increment it's pixel count ( in a blank image ) by 1.

2) Repeat the same for all bounding boxes in an image.

This will generate a heatmap i.e. pixels will be coloured for all detections and zero( i.e. black ) for all non-detections. For pixels detected inside multiple boxes, the heat map will be brighter as its count would be higher. This can be seen as below:

![Heat map][image5]

3) As can be seen in the above image, 3 cars were detected though only 2 cars were present. We notice that the left most heat region is a very lightly colored region. This is because it was detected only by 1 box. We can remove this by rejecting all pixels in a heat map whose count is below a threshold. We chose a threshold of 1 for this project as it worked optimally. The thresholded image can be seen below:

![Heat map with thresholds][image6]


4) Using a API from scipy.ndimage.measurements library, we convert the heatmap into a label image. This basically converts a sequence of neighboring elements in a 2D array into an image. For this all pixels adjacent row wise and column wise a single label is generated. This label is now the bounding region for our car and we store this. The result of a heatmap to a label conversion can be seen below:

![Heat map to label][image7]

5) By concatenating the original image with the bounding region identified by labels image, we get our final result i.e. an image where a car is bounded by a box. This can be seen as below:

![Result image][image8]


##Applying pipeline to a video stream

This step simply consists of applying the "Car detection using sliding window" and "Reducing false positives" steps over a stream of images. However there is a slight caveat. As the car moves in a video stream, the size of bounding box will change and thus make it wobbly. 

To overcome this we do the folowing:

1) Instead of using the heatmap over the bounding boxes in the latest image, we generate a heatmap over the last 10 bounding boxes ( even if it is over the previous images )

2) We then appropriately modify the threshold to use a value proportional to the bounding boxes list instead of 1.

The above has an effect of smoothening the image stream though it is not perfect as it will also do the smoothening for false positives ( if any ).
​​

Below is the result of the above exercise being applied to detect vehicles in a video. Clicking on below will open a new window.
Click on "View Raw" after that to download and play video.

![Video](./test_videos_output/project_video.mp4)

## Issues faced/limitations of solution


1) The above algorithm simply did not work on Linux. This was probably due to maximum RAM size set in the virtual machine. So I moved to windows.

2) The colorspace had to be chosen carefully. All colorspaces had some or the other limitations. RGB was smoothest but had issues detecting the white car.

3) Deciding the regions and scales to be used for each region for sliding window approach was a challenge. This was solved by printing all the bounding boxes.

4) Also I chose to remove all pixel values with x less then 450 as my car was in the leftmost lane. So this algorithm might not work for all scenario's

Possible further improvements:

1) Augmenting the image set. As the image set was small and many images were from a serial stream, the training might not have been perfect and might actually be overfitting. 

2) Need to better identify the color pixels and histogram. The color space chosen was HSV. We might be better off by choosing of a combination of channels like B from RGB, S from HLS etc.

3) The current solution does not separate cars close to each other as seen in the video stream. They are displayed as one giant box. This might be possible room for improvement.
