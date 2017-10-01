# TensorFlow Neural Network Lab
[![Udacity - Self-Driving Car NanoDegree](https://s3.amazonaws.com/udacity-sdc/github/shield-carnd.svg)](http://www.udacity.com/drive)

[<img src="http://yaroslavvb.com/upload/notMNIST/nmn.png" alt="notMNIST dataset samples" />](http://yaroslavvb.blogspot.com/2011/09/notmnist-dataset.html)

We've prepared a Jupyter notebook that will guide you through the process of creating a single layer neural network in TensorFlow.
Explains pickle, tensorflow, anaconda setup etc.

## OS X and Linux Instructions
#### Install Anaconda
This lab requires [Anaconda](https://www.continuum.io/downloads) and [Python 3.4](https://www.python.org/downloads/) or higher. If you don't meet all of these requirements, install the appropriate package(s).
#### Run the Anaconda Environment
Run these commands in your terminal to install all the requirements:
```sh
$ git clone https://github.com/udacity/CarND-TensorFlow-Lab.git
$ conda env create -f CarND-TensorFlow-Lab/environment.yml
$ conda install --name CarND-TensorFlow-Lab -c conda-forge tensorflow
```
#### Run the Notebook
Run the following commands from the same directory as the commands above.
```sh
$ source activate CarND-TensorFlow-Lab
$ jupyter notebook
```
#### View The Notebook
Open a browser window and go [here](http://localhost:8888/notebooks/CarND-TensorFlow-Lab/lab.ipynb).  This is the notebook you'll be working on.  The notebook has 3 problems for you to solve:
 - Problem 1: Normalize the features
 - Problem 2: Use TensorFlow operations to create features, labels, weight, and biases tensors
 - Problem 3: Tune the learning rate, number of steps, and batch size for the best accuracy
