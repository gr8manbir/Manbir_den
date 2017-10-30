# **Behavioral Cloning** 

## Writeup on the technical challenges and approach

---

**Behavioral Cloning Project**

The goals / steps of this project are the following:
* Use the simulator to collect data of good driving behavior
* Build, a convolution neural network in Keras that predicts steering angles from images
* Train and validate the model with a training and validation set
* Test that the model successfully drives around track one without leaving the road
* Summarize the results with a written report


[//]: # (Image References)

[image1]: ./examples/left_2016_12_01_13_30_48_287.jpg "Left camera image"

[image2]: ./examples/center_2016_12_01_13_30_48_287.jpg "Center camera image"

[image3]: ./examples/right_2016_12_01_13_30_48_287.jpg "Right camera image"

[image4]: ./examples/Model1.PNG "Model (part 1)"

[image5]: ./examples/Model2.PNG "Model (part 2)"

[image6]: ./examples/Model3.PNG "Model (part 3)"

[image7]: ./examples/Training_loss.bmp "Training loss"

[image8]: ./examples/validation_loss.bmp "Validation loss"

---

This is the third project of the udacity self driving car nano-degree. It is the first neural network regression networkthat I have successfully trained
and used to autonomously drive a car around a track.

---
# Contents:

- Architecture and training strategy
- Model details
- Data Augmentation
- Model Architecture
- Model Training
- Model Performance
- Performance on new Images
- Model probabilities ( softmax predictions )


---
## Architecture and training strategy

1. The training set consists of a collection of triad images ( left camera, center camera and right camera). This is the feature set for regressor. An example of the training set images is seen below:

Left camera image
![Left camera image][image1]


Center camera image
![Center camera image][image2]


Right camera image
![Right camera image][image3]


All the images are treated as a part of the data set irrespective of the camera it came from. A correction factor of -0.25 is applied to the right camera image as position of camera is mounted way on the car's right side. Similarly a correction factor of +0.25 is applied to the left camera. This is to treat as to correct distortions caused by angle of camera placement.


2. Alongwith the trio of images is a label called steering angle.

3. The goal is to create a model that will take images as input feature set and predict the steering angle.

4. The NVIDIA model chosen is taken from the below paper has been used to build a model with slight customizations explained later on:

http://images.nvidia.com/content/tegra/automotive/images/2016/solutions/pdf/end-to-end-dl-using-px.pdf

5. The data set could be created by running the simulator in training mode or by using a pre-recorded set provided by udacity. 

6. For my project, I have chosen to use the dataset provided by udacity.

7. The model has been trained on Amazon EC2 instance and then copied to my laptop.

8. Using the model, I have recorded a video in autonomous driving mode.

9. I made use of Amazon EC2 and generators in python as data set was very large to deal with.

10. There is no learning rate as adam optimizer has been used.

---
## Model Details

Below is the model detail

|                   | Type             | Purpose           | Input                       | Output/filter size                 | Remarks                                     |
|:-----------------:|:----------------:|:-----------------:|----------------------------:|-----------------------------------:|--------------------------------------------:|
| First Layer       |  Lambda          |   Normalization   |     Image(160,320,3)        |     Normalized Image               |    (x/127.5)-1.0                            |
| Second Layer      |  Cropping        |   Remove pixels   |     Image(160,320,3)        |     Image -50,20px from top,bottom |    NA                                       |
| Third Layer       |  Conv2D          |   convolution     |     Image from second layer |     filter(5x5, 24 depth)          |    2x2 stride,valid padding, ELU activation |
| Fourth Layer      |  Conv2D          |   convolution     |     From third  layer       |     filter(5x5, 36 depth)          |    2x2 stride,valid padding, ELU activation |
| Fifth Layer       |  Conv2D          |   convolution     |     From fourth layer       |     filter(5x5, 48 depth)          |    2x2 stride,valid padding, ELU activation |
| Sixth Layer       |  Conv2D          |   convolution     |     From fifth layer        |     filter(3x3, 64 depth)          |    valid padding, ELU activation            |
| Seventh Layer     |  Conv2D          |   convolution     |     From sixth layer        |     filter(3x3, 64 depth)          |    valid padding, ELU activation            |
| Eight Layer       |  FLATTEN         |   flatten         |     From seventh layer      |     fully connected layer          |                                             |
| Ninth Layer       |  DENSE           |   hidden layer    |     From flattened layer    |     100 output                     |    0.25 keep probability, ELU activation    |
| Tenth Layer       |  DENSE           |   hidden layer    |     From ninth layer        |     50 output                      |    0.40 keep probability, ELU activation    |
| Eleventh Layer    |  DENSE           |   hidden layer    |     From tenth layer        |     10  output                     |    ELU activation                           |
| Twelth Layer      |  DENSE           |   output layer    |     From eleventh layer     |     1 steering angle prediction    |    NA                                       |

Though dropout is not mentioned in NVIDIA paper, it was added to avoid overfitting. It also led to a better autonomous driving. The model can also be pictorially seen below. 
These images were taken from a graph shown by tensorboard. The image is split into three for better alignment as tensorboard graph is very large.

Part 1:
![Part 1][image4]


Part 2:
![Part 2][image5]


Part 3:
![Part 3][image6]


---
## Data Augmentation and overfitting

As the data provided was generic to a single video, the model might overfit. So two things were done to better train and avoid overfitting:

1. For every image in a single batch, an extra image was added which is a flipped copy of the original image. Also the steering angle was compensated by multiplying by -1.0.

2. Two dropouts were added in ninth and tenth layer. An improvement was seen in the autonomous driving.

---
## Overcoming failures

Several failures were seen while working on this project. Some of them are:

1. The car would drive off the road randomly. This was improved by changing padding from "Same" to "Valid". As an afterthought, this makes sense as we do not want to pad
the image with zeros at the edges.

2. At the part where there is no edge marking, the car drove off road. The earlier normalization was used /255. This is incorrect as images are colour and not black and white.
This was corrected to /127.

3. In the last phase, the car would zigzag a lot though remaining on track. As seen in the training process, it was noticed that the loss was reducing drastically but 
validation loss was stuck. Most likely this was an overfitting. So I introduced two dropout layers and this improved a lot. 

---
## Final result

The training and validation loss of the model can be seen below as progressing between different EPOCH's. 

Training loss
![Training loss][image7]

Validation loss
![Validation loss][image8]


Final result was a very pleasing car driving along the track:

![Behavioral Cloning](./run1.mp4)
