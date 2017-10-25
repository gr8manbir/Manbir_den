# File to implement model for behavioral cloning
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split

path_data='./Data'
csv_fileName='driving_log.csv'

#Method to Read CSV file and preserve steering
def ReadCSV():
    data = pd.read_csv( path_data + '//' + csv_fileName,
                        usecols=[0,1,2,3],
                        dtype={3:np.float32})
    return data

# Read the features( cols 0,1,2 ) and label ( col 3 )
data = ReadCSV()

#Split the data into training set and validation set
X_train, X_test = train_test_split(data, test_size=0.2, random_state=0)