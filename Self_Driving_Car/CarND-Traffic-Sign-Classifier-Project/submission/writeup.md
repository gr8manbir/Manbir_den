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
# Contents:

- Dataset summary & Exploration of dataset
- Preprocessing techniques used
- Data Augmentation
- Model Architecture
- Model Training
- Model Performance
- Performance on new Images
- Model probabilities ( softmax predictions )


---
## Data Set Summary & Exploration of dataset

1. The data set contains about 50000 images of various road signs in about 43 different categories. A description of the data set can be found at the following location:

http://benchmark.ini.rub.de/?section=gtsrb&subsection=dataset

2. The images for this project were taken from 

https://d17h27t6h515a5.cloudfront.net/topher/2017/February/5898cd6f_traffic-signs-data/traffic-signs-data.zip

3. The following image shows one image each of the 43 different categories

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
## Preprocessing techniques used

1. First the images were converted to YUV color space. As mentioned in the lessons, RGB color provides no real value during classification.

Formula taken from: https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale

Idea taken from: https://stackoverflow.com/questions/12201577/how-can-i-convert-an-rgb-image-into-grayscale-in-python

2. Next the images are normalized using the formula learnt in class i.e. 
ImageData = (ImageData-grayMin)*(1-0)/(255-0)
i.e. ImageData = ImageData/255.0

3. After this the histogram of the images is adjusted for exposure to equalize the brightness across the image. This is taken from:

http://scikit-image.org/docs/dev/api/skimage.exposure.html#skimage.exposure.equalize_adapthist


The following images show the pre-processed equivalent of the images shown earlier. As can be seen even visually, the images are more distinguishable.

![Pre-processed images][image3]

---
## Data Augmentation

As the image set was small and had few images in certain categories, the image set was augmented by a factor of two. Another set was created by the ImageDataGenerator
API of keras library. This was used to apply random zoom, rotation, shear, horizontal, vertical shift. The following was the API call:

ImageDataGenerator(
        rotation_range=17,
        width_shift_range=0.1,
        height_shift_range=0.1,
        shear_range=0.3,
        zoom_range=0.15,
        horizontal_flip=False,
        dim_ordering='tf',
        fill_mode='nearest')

More details of the API can be found at: https://keras.io/preprocessing/image/


## Model Architecture

The following is the architecture of the adapted LeNet architecture implemented in this classifier:


|                   | First Conv Layer | Second Conv Layer  | First fully connected layer | Second fully connected layer  | Third fully connected layer |
| ------------------|------------------|:------------------:|----------------------------:|------------------------------:|----------------------------:|
| Filter:           |  5x5             |   5x5              |     NA                      |     NA                        |    NA                       |
| Features:         |  12              |   24               |     NA                      |     NA                        |    NA                       |
| Strides           |  1,1,1,1         |   1,1,1,1          |     NA                      |     NA                        |    NA                       |
| ksize :           |  1,2,2,1         |   1,2,2,1          |     NA                      |     NA                        |    NA                       |
| Max pool stride   |  1,2,2,1         |   1,2,2,1          |     NA                      |     NA                        |    NA                       |
| weights           |  NA              |   NA               |     600x350                 |     350x184                   |    184x43                   |
| bias              |  NA              |   NA               |     350                     |     184                       |    43                       |
| Activation        |  RELU            |   RELU             |     RELU                    |     RELU                      |    None                     |
| Droput            |  NA              |   NA               |     50%                     |     50%                       |    None                     |

As can be seen, the above architecture is a slight modification of the original LeNet architecture developed by Yann LeCunn
http://yann.lecun.com/exdb/publis/pdf/lecun-98.pdf

The primary change is an increase in the number of feature layers depth. This helps in classifying the images better  ( gave a 2% increase in accuracy )

## Model Training

As mentioned above the model used was an adaptation of LeNet Architecture. The logits obtained from the neural network were fed to a softmax function.
The probabilities from the softmax o/p was compared against the one-hot encoded labels to obtain cross entropy. Loss in the model is defined as the mean
of cross entropy. The loss was then optimized by using adam optimizer. Following are the hyper parameters used:
learning rate: 0.001. Rationale being that with augmentation the model was fitting good enough with such a rate. So it was not reduced to improve accuracy.
EPOCHS: 20
BATCH_SIZE: 150. Gave good mix of speed and accuracy.

## Model Performance

Accuracy on validation set: 0.963 ( after 20 EPOCHS )
Accuracy on test set:       0.952

## Performance on new Images

As required by project rubric, 5 images of german traffic signs, were taken from the internet and cropped to 32x32 size. These are the following:

![Right of way][image4]

![Speed 30][image5]

![Speed 60][image6]

![Speed 70][image7]

![Work Ahead][image8]

These are simple images and do not seem visually difficult to classify. After preprocessing and running them through the trained network was confirmed true. 
The accuracy was 1.0 i.e. all images were correctly classified.

## Model probabilities ( softmax predictions )

From the below data, it is clear that the images were rightly classified:

Neural network Predictions:  [25 11  1  3  4]
Actual classes            :  [25 11  1  3  4]

Top 5 softmax predictions:( each row represents one image, indices=image category) :
[[25 24 29 28 22]

 [11 27 30 21 25]

 [ 1  5  0  6 31]

 [ 3  2  4  8  5]

 [ 4  1 18 24  0]]

Top 5 softmax probabilties: 
[[  9.99235272e-01   7.58760259e-04   3.25825886e-06   2.75790330e-06
    6.37137454e-09]

 [  9.99848366e-01   6.93130278e-05   5.20531612e-05   2.95314512e-05
    5.80517167e-07]

 [  9.99997973e-01   5.92441779e-07   4.39204342e-07   4.14959601e-07
    1.52277053e-07]

 [  9.86739397e-01   8.09092540e-03   2.65885820e-03   1.28167518e-03
    1.16214855e-03]

 [  9.99728620e-01   2.28598452e-04   3.29473078e-05   7.16680324e-06
    1.73314629e-06]]


The project successfully detects images with an accuracy of around 96%. 


One potential shortcoming possibly is that the above model might not be good enough for a real world application wherein the images have to be detected correctly
100% of the times. Also this might be due to sample size being small of only 50000 images. Whereas the thumb of rule in machine learning says there should be 50000
samples of each category to properly classify data. 
