# File to implement model for behavioral cloning
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from keras.model import Sequential
from keras.layers import Flatten, Dense, Dropout, Activation, Lambda, Cropping2D, Convolution2D 

path_data='./Data'
csv_fileName='driving_log.csv'
images=[]
steeringangles=[]
correction=0.25

def NVidiaModel():
    model = Sequential()
    #Normalize the image
    model.add(Lambda(lambda x: (x/255.0)-0.5, input_shape=(160,320,3)))
    #Crop the images
    model.add(Cropping2D(cropping=((50,20), (0,0)), input_shape=(3,160,320)))
    
    #Add convn layers
    model.add(Convolution2D(24, 5, 5, subsample=(2, 2), border_mode="same", activation="relu"))
    model.add(Convolution2D(36, 5, 5, subsample = (2, 2), border_mode="same", activation="relu"))
    model.add(Convolution2D(48, 5, 5, subsample=(2, 2), border_mode="same", activation="relu"))
    model.add(Convolution2D(64, 3, 3 , border_mode='same', activation='relu'))
    model.add(Convolution2D(64, 3, 3 , border_mode='same')) # Manbir - check if activation makes a difference here
    
    #Add fully connected layers
    model.add(Flatten())
    model.add(Dense(100), activation='relu')
    model.add(Dropout(0.5))
    model.add(Dense(50), activation='relu')
    model.add(Dropout(0.5))
    model.add(Dense(10))
    model.add(Dense(1)) # We want only one steering angle based on input images
    return model

## Read the features( cols 0,1,2 ) and label ( col 3 )
data = pd.read_csv( path_data + '//' + csv_fileName,
                        usecols=[0,1,2,3],
                        dtype={3:np.float32})


#Split the data into features and labels
#X = data[['center','left','right']]
#Y = data[['steering']]

#Create image data
for row in data.values:
    #First append center camera images
    images.append(path_data + '/' + row[0])
    steeringangles.append(row[3])
    #Next append left camera images
    images.append(path_data + '/' + row[1])
    steeringangles.append(row[3]+correction)
    #Next append right camera images
    images.append(path_data + '/' + row[2])
    steeringangles.append(row[3] - correction)

#Convert to numpy arrays
img_features = np.array(images)
angles_labels = np.array(steeringangles)

#Combine them so that we can randomize them together later 
Samples = list(zip(img_features, angles_labels))

#Split the data into training set and validation set
X_train, X_test= train_test_split(Samples, test_size=0.2, random_state=0)

#Build model
model = N